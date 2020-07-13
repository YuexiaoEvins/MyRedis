#include "myRedis.cpp"
#include <iostream>
#include <unistd.h>
#include <string>
#include <thread>
void th_fun(int i,std::string &key){
  MyRedis& test = MyRedis::Inst();
  printf("thread %d ",i);
  test.SearchKV(key);
}
int main(){
  MyRedis& test = MyRedis::Inst();
  test.Start();
  int expired_time = 5;
  std::string s1 = "s1";
  std::string s2 = "value1";
  test.SaveKV(s1,s2,expired_time);
  for(int i=0;i<10;i++){
    std::thread tthread(th_fun,i,std::ref(s1));
//    std::thread tthread2(th_fun,i,std::ref(s1));
//    std::thread tthread3(th_fun,i,std::ref(s1));
    sleep(1);
    tthread.join();
//    tthread2.join();
//    tthread3.join();
  }
  //test.SearchKV(s1);
  return 0;
}
