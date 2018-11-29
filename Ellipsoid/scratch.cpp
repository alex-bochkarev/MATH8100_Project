#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

int main()
{
  colvec a(5);
  cout << "The size of a is:" << size(a)[0] << endl;
  return 0;
}
