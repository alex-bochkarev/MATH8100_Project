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
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);

  if (vm.count("help")) {
    cout << desc << endl;
    return 0;
  };

  po::notify(vm);
  // now we have all the necessary arguments

  assert(n>0 && m>0 && N>0);

  ofstream testcase;
  time_t rawtime;

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
  for(int i=0;i<N; i++){
    // generating a test case
    fileName = fname+to_string(n)+"x"+to_string(m)+"_"+to_string(i)+".data";
    testcase.open(fileName);
    if(!testcase.is_open()){
      cerr << "Error opening the file: " << fileName << endl;
      return 1;
    };
    testcase << "# Randomly generated test case" << endl;
    testcase << "# Case number: " << i;
    testcase << "; Generated: " << asctime(localtime ( &rawtime )) << endl;
    testcase << "# Parameters: n=" << n << ", m=" << m << ", a=" << al << ",b=" << bl << endl;
    // generate the numbers
    A.randu(); A = (A - Az)*al;
    b.randu(); b = (b - bz)*bl;
    c.randu(); c = (c - cz)*cl;

    testcase << ">epsilon -- precision parameter" << endl;
    testcase << eps << endl;
    testcase << ">A matrix data" << endl << A;
    testcase << ">b (rhs) data" << endl << b;
    testcase << ">c (costs vector) data" << endl << c;

    if(!solveModel(A,b,c,status,x,optObj)){
      cerr << "Error during solving the model: " << fileName << endl;
      testcase.close();
      return 1;
    }

    switch(status){
    case GRB_OPTIMAL:
      testcase << ">x -- known optimal solution" << endl << x << endl;
      testcase << "# Optimal objective: " << optObj << endl;
      break;
    case GRB_INFEASIBLE:
      testcase << ">Status flag:" << endl << "INFEASIBLE";
      break;
    case GRB_UNBOUNDED:
      testcase << ">Status flag:" << endl << "UNBOUNDED";
      break;
    default:
      testcase << "WRONG FLAG: " << status << endl;
      testcase.close();
      return 1;
    };
    testcase.close();
  };

  return 0;
}
