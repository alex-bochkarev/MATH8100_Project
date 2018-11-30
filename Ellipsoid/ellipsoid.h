/*
  ellipsoid.h
  The main file describing the Ellipsoid method algorithm
  For the MATH8100 course project at Clemson University
  Author: A. Bochkarev
  Date: Nov-Dec 2018
*/

#include <armadillo>
#include <stdexcept>

using namespace std;
using namespace arma;

// module-specific constants
#define Rbig 100 // big-enough ball radius for the initial ellipsoid
#define ERR_FACTOR 10

struct Ellipse{
  mat H; // shape
  colvec o; // center point
};

class LPSolver{
protected:
  // model parameters
  // feasible set is given by Ax <= b
  colvec *c; // costs vector
  colvec *b; // right-hand side
  mat *A;// feasible set (a polytope)
  int n,m;

 public:
  LPSolver(){ c = NULL; A = NULL; b = NULL; n = 0; m=0;}
  ~LPSolver(){
  };
  void setModel(colvec &costs, mat &coeffs, colvec &rhs){
    c = &costs;
    A = &coeffs;
    b = &rhs;
    n = size(*A)[1];
    m = size(*A)[0];
  };
  virtual colvec solve() = 0; // pure virtual -- has to be defined for specific solvers, obviously
  inline uvec checkFeasibility(colvec &x){
    return( (*A) * x > (*b)); // no constraints are violated
  };

  double valueAt(colvec &x){
    return dot(*c,x);
  }
};

class EllipsoidSolver: public LPSolver{
  double bestObjective;
  colvec bestPoint;
  double eps;
 public:
  EllipsoidSolver(double precision) {
    bestObjective = std::numeric_limits<double>::infinity(); // set the objective to +inf
    eps = precision;
  };
  colvec solve();
  // algorithm-specific
  Ellipse getFirstEllipse();
  inline Ellipse updateEllipse(colvec &wt, Ellipse &E){
    Ellipse newE;
    newE.o = E.o - (1.0/(n+1))*E.H*wt / (sqrt(dot(wt,E.H*wt))); // update the center
    newE.H = (pow(n,2)/(pow(n,2)-1))*(E.H - (2.0/(n+1.0))*(E.H*wt*trans(wt)*E.H)/dot(wt,E.H*wt)); // update the shape
    return newE;
  };
  inline bool stopCriterion(colvec &w, Ellipse &E){
    return(sqrt(dot(w,E.H*w) < eps)); // see Dr. Boyd's notes (TODO: citation in the report)
  };
};

Ellipse EllipsoidSolver::getFirstEllipse(){
  // to start the ball rolling, I'll implement just a big enough unit ball
  Ellipse E0;
  E0.o = colvec(n, fill::zeros);
  E0.H = mat(n,n,fill::eye);
  E0.H = E0.H * Rbig;
  return E0;
}

colvec EllipsoidSolver::solve()
{
  if (A==NULL || b==NULL || c==NULL || n==0) throw std::invalid_argument("Model parameters A,b,c are not fully specified (consider calling setModel(...)) first)");
  
  Ellipse E = getFirstEllipse();
  colvec wt;

  bool timeToStop = false;

  do{
   uvec S = checkFeasibility(E.o);
    if(accu(S) == 0){ // no constraints are violated
      // the center is in the feasible set
      wt = *c; // as c is basically the gradient vector for the linear objective
      if (valueAt(E.o)<=bestObjective){
        // we have found a better solution
        bestObjective = valueAt(E.o);
        bestPoint = E.o;
      }
      if(stopCriterion(wt,E)) timeToStop = true;
    }else{
      // the new center is not in the feasible set
      // find the constraint that was violated
      for (int i=0;i<m; i++){
        if(S[i] != 0 ){
          // we have found a violated constraint;
          wt = trans(A->row(i));
          break;
        }
      }
    }
    E = updateEllipse(wt,E);
  }while(!timeToStop);
  return bestPoint;
}
