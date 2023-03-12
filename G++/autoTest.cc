#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

struct Obj
{
  explicit Obj(string str)
  {
    this->str = std::move(str);
    printf("Obj Ctor:%p str:%s\n", this, this->str.c_str());
  }
  Obj(const Obj &obj)
  {
    this->str = obj.str;
    printf("Obj CCtor (%p str:%s)from (%p str:%s)\n", this, this->str.c_str(), &obj, obj.str.c_str());
  }
  ~Obj() { printf("Obj Dtor:%p str:%s\n", this, this->str.c_str()); }

  bool operator==(const Obj &other) const { return this->str == other.str; }

  string str;
};

struct hash_Obj
{
  size_t operator()(const Obj &obj) const { return hash<string>()(obj.str); }
};

int main()
{
  unordered_map<Obj, int, hash_Obj> m;
  m.reserve(2);
  m.emplace("kkk"s, 1);
  m.emplace("aaa"s, 2);
  printf("===1===\n");
  for (const pair<Obj, int> &p : m)
  {
    // 这里由于Obj的类型没有与unordered_map的匹配
    // 实际上unordered_map中类型为const Obj&
    // 因此运行会多出Obj的拷贝构造
    cout << &p.first << " : " << p.first.str << "->" << p.second << endl;
  }
  printf("===2===\n");
  for (const auto &p : m)
  {
    // 可见，当你不知道某一个类型的具体类型时，最好使用auto，减少拷贝
    cout << &p.first << " : " << p.first.str << "->" << p.second << endl;
  }
  printf("===3===\n");
  for (const pair<const Obj &, int> &p : m)
  {
    // 无Obj的拷贝构造
    cout << &p.first << " : " << p.first.str << "->" << p.second << endl;
  }
  printf("===end===\n");

  return 0;
}
