#include "ObjectPool.h"

#include <iostream>
#include <vector>
#include <map>

using namespace std;

int alive = 0;

class A
{
public:
    A()
    {
        alive++;
    }
    ~A()
    {
        alive--;
    }
};

template<>
struct PoolTraits<A>
{
    static const PoolType kType = PoolType::Disabled;
};

class B
{
public:
    B()
    {
        //cout << ++alive << endl;
    }
    ~B()
    {
        //cout << --alive << endl;
    }
};

template<>
struct PoolTraits<B>
{
    static const PoolType kType = PoolType::ThreadLocal;
    static const int kLowWaterMark = 16;
    static const int kHighWaterMark = 128;
    static const long long kMaxIdle = 1000;  // ms
};

class C
{
public:
    unsigned long long x;
    unsigned long long y;
    unsigned long long z;
    C()
    {
        //cout << ++alive << endl;
    }
    ~C()
    {
        //cout << --alive << endl;
    }
};

template<>
struct PoolTraits<C>
{
    //static const PoolType kType = PoolType::Disabled;
    static const PoolType kType = PoolType::ThreadLocal;
    static const int kLowWaterMark = 16;
    static const int kHighWaterMark = 128;
    static const long long kMaxIdle = 3000;  // ms
};

typedef struct tag_train{
    unsigned int groupId;
    unsigned int issi1_state;     //车台1开关机状态
    unsigned int issi2_state;     //车台2开关机状态
    unsigned int gssi;		//组号
    unsigned int cache_gssi;			//延时转组保留的gssi		2020/11/2
    //wchar_t groupLabel[MAX_TRAIN_GROUPLABEL_LEN]; 
    unsigned int relate_headrcid;//关联车头电台资源
    unsigned int relate_tailrcid;//关联车尾电台资源
    std::vector<int> relate_otherRcid;//关联第三个及以上的电台资源
    std::map<int, int> section_gssiMap;

    int pathId;        //交路号
    std::string  trackName;      //轨道名称 10字节,第11字节是字符串结束符号
    std::string  map_trackName;      //
    int coachNumber;	//车次号
    int driverId;		//司机号
    byte sectionId;		//区段：正线、车辆段、停车场
    byte direction;		//方向：上行、下行
    int position;		//如果高字节为0，则表示车站号(1~33)；
    //如果高字节非0，则列车位于高字节代表的车站和低字节代表的车站中间
    std::string position_name;
    std::string station_area_name;	//所在的站区名
    std::string did;         //目的地ID
    int destStation; //终点站号
    std::string destStation_name;
    byte atsStatus; //ATS状态 0关闭 1打开
    byte broadcastStatus;
    byte headNum;  //0:无车头 1:1号车台为车头 2:2号车台为车头
    //byte radio_status; //0:都关闭；1：车台1开机；2：车台2开机；3：都开机
    int pre_station;
    std::string pre_station_name;
    int  next_station;
    std::string next_station_name;
    std::string driveMode;
    std::string speedProfile;
    byte fastSlowFlag;
    unsigned char assignMode; //永久加载，或动态加载
}RDS_TRAIN;

template<>
struct PoolTraits<RDS_TRAIN>
{
    //static const PoolType kType = PoolType::Disabled;
    static const PoolType kType = PoolType::ThreadLocal;
    static const int kLowWaterMark = 16;
    static const int kHighWaterMark = 128;
    static const long long kMaxIdle = 0;  // ms
};

void testWashOut()
{
    std::vector<PooledPtr<C>> ptrs;
    for (int i = 0; i < 1000; i++)
    {
        ptrs.push_back(object_pool::Get<C>());
        // cout << ptrs[i].Get() << endl;
    }
    ptrs.clear();
    for (int i = 0; i != 200; ++i)
    {
        Sleep(10);
        object_pool::Get<C>();  // Trigger wash out if possible.
    }
    cout << alive << endl;
}

void diff()
{
    for (int i = 0; i < 1000; i++)
    {
        C* c = (C*)object_pool::Get<C>().Leak();
        B* b = (B*)object_pool::Get<B>().Leak();
        delete c;
        delete b;
    }
}

void benchmark()
{
    const int times = 3000000;
    unsigned long long start, end;
    start = object_pool::tls::GetCurrentTimestamp();
    for (int i = 0; i < times; i++)
    {
        RDS_TRAIN* c = new RDS_TRAIN();
        delete c;
    }
    end = object_pool::tls::GetCurrentTimestamp();
    cout << end - start <<endl;
    std::vector<PooledPtr<RDS_TRAIN>> ptrs;
    for (int i = 0; i < times; i++)
    {
        ptrs.push_back(object_pool::Get<RDS_TRAIN>());
        // cout << ptrs[i].Get() << endl;
    }
    ptrs.clear();
    start = object_pool::tls::GetCurrentTimestamp();
    for (int i = 0; i < times; i++)
    {
        object_pool::Get<RDS_TRAIN>();
    }
    end = object_pool::tls::GetCurrentTimestamp();
    cout << end - start <<endl;
}

int main()
{
    //testWashOut();
    //diff();
    benchmark();
    return 0;
}