/*
  ell-test.cpp
  A test module for the ellipsoid algorithm
*/

#include "ellipsoid.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

void showUsage(char *progname)
{
  cout << "USAGE:" << endl;
  cout << progname << " <filename>" << endl;
  cout << "Where *.data files are corresponding problem coefficients and <epsilon> is the required precision (number)" << endl;
}

int main(int argc, char **argv)
{
  if (argc<1){
    cout << argc << ": wrong number of arguments. Expected exactly 1 (filename)." << endl;
    showUsage(argv[0]);
    return 1;
  };


  ifstream file;
  file.open(argv[1]);

  std::string line;

  char mode='0';
  std::vector<double> row;
  double eps = -1.0;
  mat A; colvec b; colvec c;
  colvec xStar; bool answerKnown=false;

  stringstream ssA, ssb, ssc, ssX;
  int knownStatus = IS_UNSOLVED;

  while(getline(file, line)){
    if(line[0]=='#') continue; // commented out line found
    if(line[0]=='>'){mode=line[1];continue;};
    switch(mode){
    case 'A':
        ssA << line;
        ssA << endl;
        break;
    case 'b':
        ssb << line  << endl;
        break;
    case 'c':
        ssc << line  << endl;
        break;
    case 'e':
        eps = stod(line); // notice that I will take +1 order in precision to be sure with rounding errors
        mode='0';
        break;
    case 'x':
        ssX << line << endl;
        answerKnown=true;
        break;
    case 'S':
      switch(line[0]){
      case 'I':
        knownStatus = IS_INFEASIBLE;
        cout << "INFEASIBLE" << endl;
        break;
      case 'U':
        knownStatus = IS_UNBOUNDED;
        cout << "UNBOUNDED" << endl;
        break;
      default:
        cout << "UNKNOWN" << endl;
        cerr << "Error parsing input file:" << endl;
        cerr << "The line starting with '!' is expected to contain a (known) status specifier for the solution:" << endl;
        cerr << "U for UNBOUNDED" << endl << "I for INFEASIBLE" << endl;
        return 1;
      };
      mode = '0';
      break;
    case '0':
      break;
    default:
      cerr << "Unknown mode specifier (first symbol in the line): " << mode << endl;
      return 1;
    };
  };

  if(!A.load(ssA)){
    cerr << "Error parsing the A matrix" << endl;
    return 1;
  };
  if(!b.load(ssb)){
    cerr << "Error parsing the rhs" << endl;
    return 1;
  };

  if(!c.load(ssc)){
    cerr << "Error parsing costs vector" << endl;
    return 1;
  };

  if(answerKnown)
    if (!xStar.load(ssX)){
      cerr << "Error parsing known solution vector" << endl;
      return 1;
    }

  if (size(A)[0]!=size(b)[0] || size(A)[1] != size(c)[0] || (answerKnown && size(A)[1]!=size(xStar)[0])){
    cerr << "Data dimensions do not match! What we've got is" << endl;
    cerr << "A:" << size(A) << endl;
    cerr << "b:" << size(b) << endl;
    cerr << "c:" << size(c) << endl;
    cerr << "x:" << size(xStar) << endl;
    return 1;
  };

  cout << "A matrix is :\n" << A << endl;
  cout << "rhs is: \n" << b << endl;
  cout << "Costs vector is:\n" << c << endl;

  cout << "Epsilon is " << eps << endl;
  if(answerKnown) cout << "Known optimal solution:\n" << xStar << endl;

  EllipsoidSolver solver(eps);
  solver.setModel(c,A,b);
  if(answerKnown) solver.setFstar(solver.valueAt(xStar));
  colvec opt = solver.solve();

  if (answerKnown) cout << "Optimal objective (known): " << solver.valueAt(xStar) << endl;

  switch(solver.getStatus()){
  case IS_OPTIMAL:
    cout << "Optimal objective (found): " << setprecision(3) << fixed << solver.valueAt(opt) << endl;
    cout << "Optimal point (found):" << endl << opt;
    if(answerKnown){
      cout << "Objective value error: " << solver.valueAt(xStar) - solver.valueAt(opt) << endl;
      cout << "SOLUTION CHECK: ";
    if(abs(solver.valueAt(opt)-solver.valueAt(xStar))<=eps){
      cout << "OK" << endl;
    }else cout << "FAILED" << endl;
    }
    break;
  case IS_UNBOUNDED:
    cout << "Optimal objective (found): -INF " << endl;
    cout << "The problem is unbounded. A direction of feasible ray with infinite cost:" << endl;
    cout << opt;
    if(knownStatus == IS_UNBOUNDED){
      cout << "Optimal objective (known): -INF" << endl << "SOLUTION CHECK: OK" << endl;
    }else{
      if(answerKnown) cout << "SOLUTION CHECK: FAILED" << endl;
    }
    break;
  case IS_INFEASIBLE:
    cout << "Optimal objective (found): +INF" << endl;
    cout << "The problem is infeasible." << endl;
    if(knownStatus == IS_INFEASIBLE) {
      cout << "Optimal objective (known): +INF" << endl << "SOLUTION CHECK: OK" << endl;
    }else{
      if (answerKnown) cout << "SOLUTION CHECK: FAILED" << endl;
    }
    break;
  case IS_UNSOLVED:
    cerr << "UNSOLVED" << endl;
    break;
  default:
    cerr << "WRONG SOLVER STATUS: " << solver.getStatus() << endl;
    break;
  };
  return 0;
}
