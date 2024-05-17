#include <codecvt>
#include <iostream>
#include <locale>
#include <string>

int main(void) {
  std::wstring str = L"我是中国人yzb";

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  std::string narrowStr = converter.to_bytes(str);

  std::wstring wstr = converter.from_bytes(narrowStr);
  std::cout << narrowStr << std::endl;

  std::wcout << wstr << std::endl;

  return 0;
}
