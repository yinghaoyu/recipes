#include <iostream>

using namespace std;

int main()
{
  uint32_t a = 0;
  uint32_t b = 1u << 31;
  if(a < b)
  {
    cout << "less" << endl;
  }
  if(a > b)
  {
    cout << "more" << endl;
  }
  cout << a <<" " << b <<" "<<-b << endl;
  return 0;
}
