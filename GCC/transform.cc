#include <iostream>
#include <string>
#include <algorithm>

int main()
{
  std::string a = "AbCdeFGHIj";
  // 将大写字母转成小写
  //std::transform(a.begin(), a.end(), a.begin(), ::tolower);
  std::transform(a.begin(), a.end(), a.begin(), [](char ch){ return static_cast<char>((ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A'): ch); });
  // 根据ASCII码特性，大小写互相转换可以直接^32
  char a = 'A';
  char A = a ^ 32; // A = 'A'
  char B = 'B';
  char b = B ^ 32; // b = 'b'
  std::cout<< a << std::endl;
  return 0;
}
