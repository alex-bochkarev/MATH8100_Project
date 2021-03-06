/*
  ellipsoid.h
  The main file describing the Ellipsoid method algorithm
  For the MATH8100 course project at Clemson University
  Author: A. Bochkarev
  Date: Nov-Dec 2018
*/

#include <armadillo>
#include <stdexcept>

#define _USE_MATH_DEFINES       // use constants from cmath
#include <cmath>

using namespace std;
using namespace arma;

// module-specific constants
#define Rbig 1e10            // big-enough ball radius for the initial ellipsoid
#define ERR_FACTOR 1e3
#define FEASIB_EPS 1e-100       // practical "error margin" for unboundedness and infeasibility test
#define UNBOUND_EPS 1e-200
#define MAX_STEP 50000

#ifndef SET_THICKNESS
#define SET_THICKNESS 1e-8 // a constant for non-full dimensional cases
#endif

// problem status flags
#define IS_UNSOLVED    0
#define IS_OPTIMAL     1
#define IS_UNBOUNDED   2
#define IS_INFEASIBLE -1
#define IS_FEASIBLE    3 // status for 'in progress'

// update methods
#define UPD_KHACHIYAN  0
#define UPD_PLAIN      1
/* *******************************************************************
  Basic structure for an ellipsoid representation
*/
struct Ellipse{
  mat H; // shape
  mat B; // shape (decomposed)
  colvec o; // center point
  double vol;
};

/* *******************************************************************
   Base class for LPSolver
   (common methods & interfaces to be implemented by solvers)
*/
class LPSolver{
protected:
  // model parameters
  // feasible set is given by Ax <= b
  colvec *c; // costs vector
  colvec *b; // right-hand side
  mat *A;// feasible set (a polytope)
  int n,m;
  double fStar;
  bool fKnown;
  int status;
 public:
  LPSolver(){ c = NULL; A = NULL; b = NULL; n = 0; m=0; fStar = std::numeric_limits<double>::infinity(); fKnown = false; status = IS_UNSOLVED;}
  ~LPSolver(){
  };
  void setModel(colvec &costs, mat &coeffs, colvec &rhs){
    c = &costs;
    A = &coeffs;
    b = &rhs;
    n = size(*A)[1];
    m = size(*A)[0];
  };
  void setFstar(double f){ fStar = f; fKnown = true; }; // set the known solution (if known) -- for the check after solution
  virtual colvec solve(bool verbose = false) = 0; // has to be defined for specific solvers, obviously
  inline uvec checkFeasibility(colvec &x){
    return( (*A) * x > (*b) + SET_THICKNESS); // no constraints are violated
  };

  double valueAt(colvec &x){
    return dot(*c,x);
  }
  int getStatus(){ return status; };
};

/* *******************************************************************
   Core class for the ellipsoid method implementation
*/
class EllipsoidSolver: public LPSolver{
  double bestObjective;
  colvec bestPoint;
  double eps;
  int updateMethod;
  bool verboseMode;
public:
  EllipsoidSolver(double precision, int updMethod = UPD_KHACHIYAN) {
    bestObjective = std::numeric_limits<double>::infinity(); // set the objective to +inf
    eps = precision;
    updateMethod = updMethod;
    verboseMode = false;
  };
colvec solve(bool verbose = false);
bool isUnbounded(colvec &d); // checks if the problem is unbounded
// algorithm-specific
Ellipse getFirstEllipse();

// Ellipsoid update -- naive version
  inline Ellipse updateEllipse(colvec &wt, Ellipse &E){
    Ellipse newE;
    newE.o = E.o - (1.0/(n+1))*E.H*wt / sqrt(dot(wt,E.H*wt)); // update the center
    newE.H = (pow(n,2)/(pow(n,2)-1))*(E.H - (2.0/(n+1.0))*(E.H*wt*trans(wt)*E.H)/dot(wt,E.H*wt)); // update the shape
    newE.B = newE.H; // for stopping criterion to work correctly
    newE.vol = E.vol* (n/(n-1.0))*pow((pow(n,2)/(pow(n,2)-1)),(n-1.0)/2.0); // formula (2.11) of Bland et al. 1981
    return newE;
  };

  // Ellipsoid update -- Khachivyan 1980 version
  inline Ellipse updateEllipseKhachiyan(colvec &wt, Ellipse &E){
    Ellipse newE;
    colvec eta = trans(E.B)*wt;
    mat etaHat(n,n);

    for (int i=0;i<n;i++)
      for (int j=0;j<n; j++)
        etaHat(i,j) = eta(i)*eta(j);

    newE.o = E.o - (1.0/(n+1.0))*E.B*eta / sqrt(dot(eta,eta));
    newE.B = (1 + 1.0/(16.0*n*n))*n/sqrt(n*n-1)*(E.B + (sqrt((n-1.0)/(n+1.0))-1)*E.B*etaHat / dot(eta,eta));
    newE.H = newE.B * trans(newE.B);
    newE.vol = E.vol * exp(-1.0/(2.0*(n+1.0)));
    return newE;
  };

  ///
  inline bool stopCriterion(colvec &w, Ellipse &E){
    cout << "w=" << endl << w << endl << "H=" << endl << E.H << endl;
    cout << "Criterion value: " << sqrt(dot(*c,E.H*(*c))) << "; while eps/ERR_F =" << eps/ERR_FACTOR << endl;
    return(sqrt(dot(*c,E.H*(*c))) < eps/ERR_FACTOR); // see Dr. Boyd's notes (TODO: citation in the report)
  };
};

Ellipse EllipsoidSolver::getFirstEllipse(){ // initialization of E0 (the first ellipsoid)
  Ellipse E0;
  E0.o = colvec(n, fill::zeros);
  E0.H = mat(n,n,fill::eye);
  E0.B = mat(n,n,fill::eye);
  E0.H = E0.H * pow(Rbig,2);
  E0.B = E0.B * Rbig;
  E0.vol = pow(M_PI, n/2.0) * pow(Rbig,n) / tgamma(n/2.0 + 1); // volume of the n-ball
  return E0;
}

bool EllipsoidSolver::isUnbounded(colvec &d)
{
  cout << "Checking for unboundedness..." << endl;
  Ellipse E = getFirstEllipse();
  colvec wt; // a vector for the ellipsoid update
  colvec v;  // infeasibility
  do{
    //cout << "Ellipsoid volume is " << E.vol << endl;
    //cout << "The center is: " << endl << E.o << endl;
    //cout << "c' d = " << dot(*c, E.o) << endl;
    if (dot(*c,E.o) +1 > UNBOUND_EPS){
      // objective improvement violated
      wt = *c;
    }else{
      v = (*A)*E.o;
      if(verboseMode) cout << "v is " << v;
      //cout << "v is " << endl << v << endl;
      // objective improvement satisfied -- let's check for feasibility
      if(max(v) < UNBOUND_EPS){
        // we have found an "unboundedness direction"
        d = E.o;
        return true; // the problem is unbounded
      }else{
        for(int i=0;i<m;i++) // find the first constraint that is violated
          if(v[i]>UNBOUND_EPS){ wt = trans(A->row(i)); break;};
        // if we are here -- something does not work properly
      };
    };
    E = updateEllipseKhachiyan(wt,E);

  }while( (E.vol > FEASIB_EPS) && !E.o.has_nan()); // within the logic of Bland et al 1981
  //cout << "E.vol is " << E.vol << " while FEASIB_EPS is " << FEASIB_EPS << endl;
  //cout << "The problem is found to be bounded" << endl;
  return false;
};

//////////////////////////////////////////////////////////////////////////////
// the core function for the solver
colvec EllipsoidSolver::solve(bool verbose)
{
  if (A==NULL || b==NULL || c==NULL || n==0) throw std::invalid_argument("Model parameters A,b,c are not fully specified (consider calling setModel(...)) first)");
  // first, check unboundedness
  Ellipse E = getFirstEllipse();
  colvec wt = colvec(n,fill::zeros);

  verboseMode = verbose;
  if (isUnbounded(wt)){
    status = IS_UNBOUNDED;
    return wt;
  }

  bool timeToStop = false;
  uvec S;

  int step=0;
  stringstream outp;

  double LB = -pow(Rbig,2);

  status = IS_INFEASIBLE;

  do{
    S = checkFeasibility(E.o);

    if(verbose){
      cout << "STEP #: " << step << endl;
      cout << "Ellipse:" << endl;
      cout << "center:" << endl;
      E.o.save(outp,csv_ascii);
      cout << outp.str();
      outp.str("");
      cout << "shape:" << endl;
      E.H.save(outp,csv_ascii);
      cout << outp.str();
      outp.str("");

      if(accu(S)!=0) { cout << "infeasible" << endl; }else{ cout << "feasible" << endl;};
      cout << "VIOLATED CONSTRAINTS: " << accu(S) << endl;
    };

    if(accu(S) == 0){ // no constraints are violated
      // the center is in the feasible set
      wt = *c; // as c is basically the gradient vector for the linear objective
      if (valueAt(E.o)<=bestObjective){
        // we have found a better solution
        bestObjective = valueAt(E.o);
        bestPoint = E.o;
        status = IS_FEASIBLE;
      }
      //      if(stopCriterion(wt,E)){
      //  timeToStop = true;
      //  status = IS_OPTIMAL;
      // return bestPoint;
      //};

   }else{
     // the new center is not in the feasible set
     // find the constraint that was violated
     for (int i=0;i<m; i++){
       if(S[i] != 0 ){
         // we have found a violated constraint;
         wt = trans(A->row(i));
         break;
       }
       //if (E.vol < FEASIB_EPS) { // was the volume constraint
       //  if(verbose) cout << "E.vol=" << E.vol << " while FEASIB_EPS=" << FEASIB_EPS << endl;
       //  timeToStop = true;
       //  break;
       //}
     }
    };
    if(verbose){
      wt.save(outp,csv_ascii);
      cout << "wt:" << endl << outp.str();
      outp.str("");
    };
    if(updateMethod == UPD_KHACHIYAN){
      E = updateEllipseKhachiyan(wt,E);
    }else{
      E = updateEllipse(wt,E);
    };

    step++;


    //  }while(!timeToStop && !E.o.has_nan() && step <= MAX_STEP);
    LB = max(LB, dot((*c), E.o)-sqrt(dot((*c),E.H*(*c))));
    if(abs(bestObjective - LB)*ERR_FACTOR <= eps){
      if(verbose) cout << "stop criterion satisfied" << endl;
      status = IS_OPTIMAL;
      timeToStop = true;
    }

    if(verbose) {
      cout << "CONV. SUMMARY: Step=" << step << "; criterion=" << bestObjective - LB << "; E.vol=" << E.vol;
      if(fKnown) cout << "; df=" << bestObjective - fStar;
      cout << endl;
    };

  }while(!timeToStop && !E.o.has_nan() && step <= MAX_STEP);

  if (status==IS_FEASIBLE){
    status = IS_OPTIMAL; // seems like we have seen an optimal solution
  };

  return bestPoint;
}
