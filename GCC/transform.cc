#include <iostream>
#include <string>
#include <algorithm>

int main()
{
  std::string a = "AbCdeFGHIj";
  // 将大写字母转成小写
  //std::transform(a.begin(), a.end(), a.begin(), ::tolower);
  std::transform(a.begin(), a.end(), a.begin(), [](char ch){ return static_cast<char>((ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A'): ch); });
  std::cout<< a << std::endl;
  return 0;
}
