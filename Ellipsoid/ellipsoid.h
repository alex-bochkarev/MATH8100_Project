/*
  ellipsoid.h
  The main file describing the Ellipsoid method algorithm

*/
#include <armadillo>
#include <stdexcept>

using namespace std;
using namespace arma;

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
  ~LPSolver();
  void setModel(colvec &costs, mat &coeffs, colvec &rhs){
    c = &costs;
    A = &coeffs;
    b = &rhs;
    n = size(*A)[1];
    m = size(*A)[0];
  };
  virtual colvec solve();
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
  Ellipse getFirstEllipse(); // TODO
  inline Ellipse updateEllipse(colvec wt){
    Ellipse E;
    E.o = E.o - (1/(n+1))*E.H*wt / (sqrt(trans(wt)*E.H*wt)); // update the center
    E.H = ((n^2)/((n^2)-1))*(E.H - (2/(n+1))*(E.H*wt*trans(wt)*E.H)/(trans(wt)*E.H*wt)); // update the shape
    return E;
  };
  inline bool stopCriterion(colvec &w, Ellipse &E){
    return(sqrt(dot(w,E.H*w) <= eps)); // see Dr. Boyd's notes (TODO: citation in the report)
  };
};

colvec EllipsoidSolver::solve()
{
  if (A==NULL || b==NULL || c==NULL || n==0) throw std::invalid_argument("Model parameters A,b,c are not fully specified (consider calling setModel(...)) first)");
  
  Ellipse E = getFirstEllipse();
  colvec wt;

  do{
    uvec S = checkFeasibility(E.o);
    if(accu(S) == 0){ // no constraints are violated
      // the center is in the feasible set
      wt = *c; // as c is basically the gradient vector for the linear objective
    }else{
      // the new center is not in the feasible set
      // find the constraint that was violated
      for (int i=0;i<m; i++){
        if(S[i] != 0 ){
          // we have found a violated constraint;
          wt = A->row(i);
          break;
        }
      }
    }
    
    E = updateEllipse(wt);
    if (valueAt(E.o)<=bestObjective){
      // we have found a better solution
      bestObjective = valueAt(E.o);
      bestPoint = E.o;
    }
  }while(!stopCriterion(wt, E));
  return bestPoint;
}
