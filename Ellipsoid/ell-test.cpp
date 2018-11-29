/*
  ell-test.cpp
  A test module for the ellipsoid algorithm
*/

#include "ellipsoid.h"
#include <iostream>

int main(int argc, char **argv)
{
  if (argc<5){
    cout << argc << ": wrong number of arguments. Expected 5." << endl;
    cout << "USAGE:" << endl;
    cout << argv[0] << " <A.data> <b.data> <c.data> <epsilon>" << endl;
    cout << "Where *.data files are corresponding problem coefficients and <epsilon> is the required precision (number)" << endl;
    return 1;
      };

  double eps = atof(argv[4]);
  mat A; colvec b; colvec c;
  if(!A.load(argv[1])){
    cerr << "Error reading matrix from the file:" << argv[1] << endl;
    return 1;
  };
  if(!b.load(argv[2])){
    cerr << "Error loading rhs from the file:" << argv[2] << endl;
    return 1;
  };

  if(!c.load(argv[3])){
    cerr << "Error loading costs vector from the file:" << endl;
    return 1;
  };

  if (size(A)[0]!=size(b)[0] || size(A)[1] != size(c)[0]){
    cerr << "Data dimensions do not match! What we've got is" << endl;
    cerr << "A:" << size(A) << endl;
    cerr << "b:" << size(b) << endl;
    cerr << "c:" << size(c) << endl;
    return 1;
  }
  cout << "A matrix is :\n" << A << endl;
  cout << "rhs is: \n" << b << endl;
  cout << "Costs vector is:\n" << c << endl;

  cout << "Epsilon is " << eps << endl;

  EllipsoidSolver solver(eps);
  solver.setModel(c,A,b);

  colvec opt = solver.solve();

  cout << "Optimal point found is:\n" << opt;
  return 0;
}
