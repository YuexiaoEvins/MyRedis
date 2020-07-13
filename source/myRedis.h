#include <thread>
#include <vector>
#include <pthread.h>
#include <iostream>
#include <map>
#include <unistd.h>
#include <functional>
#include <string>
#include <chrono>
#include "RWLock.cpp"
using namespace std::chrono;
struct RedisValue {
  int expired_time;
  std::string value;
  system_clock::time_point start_time;
};
class MyRedis{
  public:
    ~MyRedis();
    MyRedis(const MyRedis&)=delete;
    MyRedis& operator=(const MyRedis&)=delete;
    static MyRedis& Inst();
    int SaveKV(std::string &key,std::string &value,int expired_time);
    int SearchKV(std::string &key);
    int Start();
    int AutocheckExpired();
    int PrintAllKV();
  private:
    std::map<std::string,RedisValue> store;  
    RWLock rwlock;
    pthread_rwlock_t redis_rw_lock;
    MyRedis();
};
