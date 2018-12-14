#pragma once
#include <functional>

class Motor{
private:
  int power=0;
  int rot=0;
  int prev_time=0;
  std::function <int(int,int)> output;
public:
  void SetPower(int pow){power=pow;}
  void SetRot(int r){rot=r;}
  int GetRot(){return rot;}
  int GetPower(){return power;}
  void SetModel(std::function <int(int,int)> o){output=o;}
  void Update(int time){
    rot+=output(power,time-prev_time);
    prev_time=time;
  }
  Motor(int pow,std::function <int(int,int)> o):power(pow),output(o){}
};
