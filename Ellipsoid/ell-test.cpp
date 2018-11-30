/*
  ell-test.cpp
  A test module for the ellipsoid algorithm
*/

#include "ellipsoid.h"
#include <iostream>
#include <fstream>
#include <vector>

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
        eps = stod(line)/ERR_FACTOR; // notice that I will take +1 order in precision to be sure with rounding errors
        mode='0';
        break;
      case 'x':
        ssX << line << endl;
        answerKnown=true;
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

  cout << "Optimal point found is:\n" << opt;
  cout << "Optimal objective: " << solver.valueAt(opt) << endl;

  if(answerKnown){
    cout << endl << "Known solution check:" << endl;

    cout << "Objective at the point found: " << solver.valueAt(opt) << endl;
    cout << "Objective at the known opt point: " << solver.valueAt(xStar) << endl;
    cout << "Optimality gap: " << solver.valueAt(xStar) - solver.valueAt(opt) << endl;
    cout << "SOLUTION CHECK: ";
    if(abs(solver.valueAt(opt)-solver.valueAt(xStar))<=(eps*ERR_FACTOR)){
      cout << "OK" << endl;
    }else cout << "FAILED" << endl; 
  }
  return 0;
}
