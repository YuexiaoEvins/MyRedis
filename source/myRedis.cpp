#include "myRedis.h"

using namespace std::chrono;
MyRedis::MyRedis(){
  int err;
}

MyRedis& MyRedis::Inst(){
  static MyRedis instance;
  return instance;
}

MyRedis::~MyRedis(){}
int MyRedis::SaveKV(std::string &key,std::string &value,int expired_time){
  RedisValue kv = { expired_time,value,system_clock::now()};
  //should make sure if key is existed
  //
  //find key whether exist logic
  //
  //if not insert


  if(SearchKV(key)) {//redis has not the same key
    //RAII-Style rwlock
    WriteGuard wlock(rwlock);
    store[key]=kv;
    printf("change key with new value success \n");
  }
  else {//redis has the same key
    WriteGuard wlock(rwlock);
    store.insert(std::map<std::string,RedisValue>::value_type (key,kv));
    printf("save success with new KV\n");
  }
  return 1;
}

int MyRedis::SearchKV(std::string &key){
  //check key is expired or not
  //1. if key is not exist 
  //  return null
  //2. if key is exist
  //  if expired_time is greater than STL-Duration-span
  //    return value
  //  else return null and delete KV
  
  std::map<std::string,RedisValue>::iterator iter;
  int del_flag = 0;
  {
    ReadGuard rlock(rwlock);
    iter = store.find(key);
    if (iter!=store.end()) 
    {
      auto end_time = system_clock::now();
      duration<int> elapsed = (duration_cast<duration<int>>(end_time - iter->second.start_time));
      if (elapsed.count()<=iter->second.expired_time) 
      {
        std::cout<<"exist-time is "<<elapsed.count()<<" and expired_time is "<<iter->second.expired_time<<std::endl;
        std::cout<<"iter->second.value = "<<iter->second.value<<std::endl;
        return 1;
      }
      else 
      {
        del_flag = 1;
      }
    }
  }
  if(del_flag){
    WriteGuard wlock(rwlock);
    store.erase(iter);
    std::cout<<"kv is expired and delete"<<std::endl;
    return 0;
  }
  else {
    printf("kv is not exist \n");
    return 0;
  }
}

int MyRedis::AutocheckExpired()
{
  system_clock::time_point start_time = system_clock::now();
  system_clock::time_point end_time;
  duration<int> sys_runtime;
  duration<int> exist_time;

  while (1) {
    end_time = system_clock::now();
    sys_runtime = (duration_cast<duration<int>>(end_time - start_time)); 
    std::vector<std::string> expired_key;
    std::map<std::string,RedisValue>::iterator iter;
    {//find all expired key and delete
      ReadGuard rlock(rwlock);
      iter = store.begin();
      for (iter=store.begin(); iter!=store.end(); iter++) {
        exist_time = (duration_cast<duration<int>>(end_time - iter->second.start_time));
        if (exist_time.count()>iter->second.expired_time) {
          expired_key.push_back(iter->first);
        }
      }
    }
    if (!expired_key.empty()) {//delete all expired_key
      WriteGuard wlock(rwlock);
      for (auto it = expired_key.begin(); it!=expired_key.end(); it++) {
        store.erase(*it); 
        printf("delete expired KV\n");
      }
    }
    sleep(1);
  }
}

int MyRedis::Start()
{
  std::thread auto_thread(std::bind(&MyRedis::AutocheckExpired,this));  
  auto_thread.detach();
  return 0;
}

