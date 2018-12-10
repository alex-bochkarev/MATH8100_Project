/*
  generate_tests.cpp
  Generates random test cases for the Ellipsoid algorithm
  Author: A. Bochkarev
  Date: Dec 2018
 */

#include <iostream>
#include <cassert>
#include <armadillo>
#include <boost/program_options.hpp>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <gurobi_c++.h>

using namespace std;
using namespace arma;
namespace po = boost::program_options;

#define DEFAULT_PRECISION 0.01
#define DEFAULT_A 10
#define DEFAULT_C 10
#define DEFAULT_B 100
#define DEFAULT_FNAME "randcase"
#define EPS_DET 0.1

// problem status flags
#define IS_UNSOLVED    0
#define IS_OPTIMAL     1
#define IS_UNBOUNDED   2
#define IS_INFEASIBLE -1
#define IS_FEASIBLE    3 // status for 'in progress'


void saveModel(string fileName, string header, mat &A, colvec &b, colvec &c, double &eps, int &status, colvec &sol, double &optObj)
{
  ofstream testcase;
  time_t rawtime;

  testcase.open(fileName);
  if(!testcase.is_open()){
    cerr << "Error opening the file: " << fileName << endl;
    exit(1);
  };

  testcase << header << endl;

  time(&rawtime);

  testcase << "# Generated: " << asctime(localtime ( &rawtime )) << endl;

  testcase << ">epsilon -- precision parameter" << endl;
  testcase << eps << endl;
  testcase << ">A -- coefficients matrix:" << endl << A;
  testcase << ">b -- right-hand side vector:" << endl << b;
  testcase << ">c -- costs vector:" << endl << c;

  switch(status){
  case IS_INFEASIBLE:
    testcase << ">Status flag:" << endl << "INFEASIBLE" << endl;
    break;
  case IS_UNBOUNDED:
    testcase << ">Status flag:" << endl << "UNBOUNDED" << endl;
    break;
  case IS_OPTIMAL:
    testcase << ">x -- known optimal solution" << endl << sol << endl;
    testcase << "# Optimal objective value: " << optObj;
    break;
  default:
    cerr << "WRONG STATUS FLAG RECEIVED: " << status << endl;
    testcase.close();
    exit(1);
  };
  testcase.close();

}
// solve the model using Gurobi
bool solveModel(mat &A, colvec &b, colvec &c, int &status, colvec &sol, double &optObj)
{
  cout << "Solving the problem with Gurobi solver" << endl;
  try {
    GRBEnv env = GRBEnv();

    GRBModel model = GRBModel(env);
    model.set(GRB_StringAttr_ModelName,"testcase");

    // Create variables

    int n = size(A)[1];
    int m = size(A)[0];

    GRBVar x[n];

    cout << "Creating variables...";
    for(int i=0;i<n;i++){
      ostringstream vname;
      vname << "x" << i;
      x[i] = model.addVar(-GRB_INFINITY, GRB_INFINITY, c(i), GRB_CONTINUOUS, vname.str());
      vname.str("");
    }

    // Note: the default is minimization, so I do not need to specify additional parameters for the objective

    // Add constraints Ax <= b

    cout << endl << "Creating constraints...";
    for(int i=0;i<m;i++){
      GRBLinExpr expr = 0;
      for(int j=0;j<n;j++)
        expr += A(i,j)*x[j];

      model.addConstr(expr <= b(i), "c"+to_string(i));
    };

    // Optimize model

    model.getEnv().set(GRB_IntParam_DualReductions, 0);
    cout << endl << "Optimizing...";
    model.optimize();

    // unpack the solution

    cout << endl << "Unpacking the solution..." << endl;
    status = model.get(GRB_IntAttr_Status);

    cout << "STATUS: ";
    switch(status){
    case GRB_OPTIMAL:
      cout << "OPTIMAL" << endl;
      // save the optimal point
      for(int i=0;i<n;i++)
        sol(i) = x[i].get(GRB_DoubleAttr_X);
      optObj = model.get(GRB_DoubleAttr_ObjVal);
      return true;
      break;
    case GRB_INFEASIBLE:
      cout << "INFEASIBLE" << endl;
      return true;
      break;
    case GRB_UNBOUNDED:
      cout << "UNBOUNDED" << endl;
      return true;
      break;
    default:
      cerr << "Wrong status:" << status << endl;
      cerr << "A is " << endl << A << endl;
      cerr << "b is " << endl << b << endl;
      cerr << "c is " << endl << c << endl;
      return false;
    }

  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch(...) {
    cout << "Exception during optimization" << endl;
  }
  cerr << "unknown error (reached the end of the optimization function)" << endl;
  return false;
}

int main(int argc, char **argv){
  cout << "Random test cases generator for the Ellipsoid algorithm implementation" << endl;

  // program parameters
  int n,m,N;
  double al,bl,cl, eps;
  string fname;
  bool nonneg = false;
  bool kmc = false;
  int testcaseType = 0; // default -- just a random case

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "show this help message")
    (",n", po::value<int>(&n)->required(), "number of variables (integer)")
    (",m", po::value<int>(&m)->required(), "number of constraints (integer)")
    (",N", po::value<int>(&N)->required(), "number of test cases to generate")
    (",e", po::value<double>(&eps)->default_value(DEFAULT_PRECISION), "precision (epsilon) -- optional")
    (",a", po::value<double>(&al)->default_value(DEFAULT_A), "limits for a_ij values (-a..a will be generated)")
    (",b", po::value<double>(&bl)->default_value(DEFAULT_B), "limits for b_i values (-b..b will be generated)")
    (",c", po::value<double>(&cl)->default_value(DEFAULT_C), "limits for c_i values (-c..c will be generated)")
    ("filename,f", po::value<string>(&fname)->default_value(DEFAULT_FNAME), "filename prefix")
    ("nonnegative", po::value<bool>(&nonneg)->default_value(nonneg), "add nonnegativity constraints (x>=0)")
    ("kmc",po::value<bool>(&kmc)->default_value(false),"generate <N> Klee-Minty Cubes")
    ("type,t",po::value<int>(&testcaseType)->default_value(0),"generate a specific type test case(s)")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // now we have all the necessary arguments

  cout << "case type = " << testcaseType << endl;

  if (vm.count("help")) {
    cout << desc << endl;
    return 0;
  };

  ostringstream sheader;
  time_t rawtime;
  double m0 = m;
  if(nonneg) m = m0 + n; // add n nonnegativity constraints

  mat A(m,n,fill::zeros);
  mat Az = mat(m,n,fill::ones)*0.5;
  colvec b(m,fill::zeros);

  colvec bz = colvec(m, fill::ones)*0.5;

  colvec c(n);
  colvec cz = colvec(n, fill::ones)*0.5;

  colvec x(n,fill::zeros);

  int status = -1;
  double optObj = -999;

  arma_rng::set_seed_random(); // set the seed for RNG

  string fileName;

  if(kmc){
    status = IS_OPTIMAL;

    for(int D=3;D<N+3;D++){
      // generate a D-dimensional KMC
      A = mat(2*D, D,fill::zeros);
      b = colvec(2*D,fill::zeros);
      c = colvec(D,fill::zeros);

      for (int m=0;m<D; m++){
        for (int i=0;i<D;i++){
          if(i<m) A(m,i) = pow(2,(m+1)-i);
          if(i==m) A(m,i) = 1;
        }

        b(m) = pow(5,m+1);
        c(m) = -pow(2,D-(m+1));
      }

      // add nonnegativity constraints
      colvec o(D, fill::zeros);
      for(int j=0;j<D;j++){
        o(j) = -1;
        A.row(D+j) = trans(o);
        o(j) = 0;
        b(D+j) = 0;
      };

      fileName = fname+"KMC_"+to_string(D)+".data";

      sheader.str("");
      sheader << "# Klee-Minty cube" << endl;
      sheader << "# Dimensionality: " << D;
      x = colvec(D,fill::zeros);
      x(D-1) = pow(5,D);
      optObj = pow(5,D);
      saveModel(fileName, sheader.str(), A, b, c, eps, status, x, optObj);
    }


    return 0;
  }

  cout << vm.count("type") << " and case_type=" << testcaseType << endl;

  if(vm.count("type") && testcaseType!=0){
    // entering the "balanced test case" mode
    cout << "Entering simplex-based case generator" << endl;

    mat X(m,n-m,fill::zeros);
    mat Xz = mat(m,n-m, fill::ones)*0.5;

    mat B(m,m, fill::zeros);
    mat Bz = mat(m,m,fill::ones)*0.5;

    colvec beta(m,fill::zeros);
    colvec cB(m, fill::zeros);

    colvec cN(n-m, fill::zeros);
    colvec rn(n-m);

    colvec xB(m,fill::zeros);
    colvec xN(n-m,fill::zeros);

    switch(testcaseType){
    case 1: // finite-optimal
      if(m>n){
        cerr << "Currently simplex-based case generator implemented only for m<=n!" << endl;
        exit(1);
      }
      // implementation based on the simplex method
      // generate X

      cout << "Generating X matrix: ";
      X.randu();
      X = (X - Xz)*al;
      cout << "done." << endl << "Generating B matrix";

      // generate B
      // we need it to be ``normally'' invertible (with not too small determinant)
      while(abs(det(B))<=EPS_DET){
        cout << ".";
        B.randu();
        B = (B - Bz)*al;
      };

      cout << "; det(B)=" << det(B) <<" --> done." << endl;
      // generate beta
      beta.randu();
      beta = beta*bl;
      cout << "beta is:" << beta;

      // calculate b -- now it is determined
      b = B*beta;

      // calculate A -- now it is also determined
      A = join_horiz(B, B*X);
      // generate c_b
      cB.randu();
      cB = (cB - bz)*cl;

      // generate c_N
      rn.randu(); // generate nonnegative deltas
      cN = trans(X)*cB + rn*cl;
      cout << "cN is " << cN << endl;
      // reconstruct C
      c = join_vert(cB, cN);
      cout << "Reduced costs row is:" << trans(c) - trans(cB)*inv(B)*A;

      // reconstruct the solution
      xB = beta;

      x = join_vert(xB,xN);
      cout << "so, optimal x is:" << x;

      optObj = dot(c,x);
      status = IS_OPTIMAL;

      A = join_vert(A, eye<mat>(n,n)*(-1));
      b = join_vert(b, colvec(n,fill::zeros));

      sheader.str("");
      sheader << "# Random test case with a finite optimum" << endl;
      sheader << "# Produced by the simplex-based test-case generator" << endl;

      fileName = fname+to_string(n)+"x"+to_string(m)+".data";
      cout << "Saving the model..." << endl;
      saveModel(fileName, sheader.str(), A, b, c, eps, status, x, optObj);
      cout << "done" << endl;
      break;
    case 2: // infeasible
      break;
    case 3: // unbounded
      break;
    default:
      cerr << "Error generating test case: " << testcaseType << " -- unknown case type!" << endl;
      cerr << "Valid options are:" << endl;
      cerr << "U -- unbounded" << endl << "I -- infeasible" << endl << "O -- with a finite optimum" << endl;
      exit(1);
    }

    return 0;
  }

  if(!(n>0 && m>0 && N>0)){
    cerr << "Wrong arguments -- n, m and N must be strictly positive" << endl;
    return 1;
  }
  for(int i=0;i<N; i++){
    // generating a test case
    fileName = fname+to_string(n)+"x"+to_string(m)+"_"+to_string(i)+".data";

    sheader.str("");
    sheader << "# Randomly generated test case" << endl;
    sheader << "# Case number: " << i;
    sheader << "; Generated: " << asctime(localtime ( &rawtime )) << endl;
    sheader << "# Parameters: n=" << n << ", m=" << m << ", a=" << al << ",b=" << bl << endl;

    // generate the numbers
    A.randu(); A = (A - Az)*al;
    b.randu(); b = (b - bz)*bl;
    c.randu(); c = (c - cz)*cl;

    colvec o(n, fill::zeros);
    if(nonneg)
      for(int j=0;j<n;j++){
        o(j) = -1;
        A.row(m0+j) = trans(o);
        o(j) = 0;
        b(m0+j) = 0;
      };

    if(!solveModel(A,b,c,status,x,optObj)){
      cerr << "Error during solving the model: " << fileName << endl;
      return 1;
    }

    switch(status){
    case GRB_OPTIMAL:
      status = IS_OPTIMAL;
      break;
    case GRB_INFEASIBLE:
      status = IS_INFEASIBLE;
      break;
    case GRB_UNBOUNDED:
      status = IS_UNBOUNDED;
      break;
    default:
      cerr << "WRONG STATUS FLAG (received from Gurobi): " << status << endl;
      return 1;
    };

    saveModel(fileName, sheader.str(), A, b, c, eps, status, x, optObj);
  };

  return 0;
}
