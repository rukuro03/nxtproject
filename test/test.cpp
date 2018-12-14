/*
  MoveTskのアルゴリズム検証用プログラムです
  実行ファイルに引数を渡すことで動作します
  コンパイルコマンドは g++ test.cpp -lm -Wall
  引数: パワー turn pgain dgain igain
*/

#include "Motor.hpp"
#include <iostream>
#include <cmath>
#include <string>

using namespace std;
void Algorithm(Motor&,Motor&);

int main(int argc, char* argv[]){
  int mrot,srot,tmp,cur_spow,ireset=0,isReverse=0,wait;
  int power=stoi(argv[1]);
  int turn=stoi(argv[2]);
  int time=0;
  int pgain=stoi(argv[3]),dgain=stoi(argv[4]),igain=stoi(argv[5]);
  double val,error=0,error_d=0,error_i=0;
  //turnの値は「外側のタイヤに対し内側のタイヤは(100-turn)%回る」という意味
  if(turn<0)
    turn=-turn;
  // 待ち時間はパワーの絶対値に反比例させる
  wait=10000/power;
  if(wait<0)
    wait=-wait;
  
  Motor master(power,[](int p,int t){return t*p/100;});
  Motor slave((100-turn)*power/100,[](int p,int t){return t*p/100+p;});
  
  cur_spow=(100-turn)*power/100.0;
  if(cur_spow<0)
    isReverse=1;
  for(int i=0;time<=5000;i++){
    mrot=master.GetRot();
    srot=slave.GetRot();
    time+=wait;
    master.Update(time);
    slave.Update(time);
    mrot=master.GetRot()-mrot;
    srot=slave.GetRot()-srot;
    //mrotとsrotの比(%)を取る 100*450/900=50
    if(mrot==0)
      mrot=1;
    val=(double)100*srot/mrot;
    if(val<0)
      val=-val;
    error_d=error;
    if(turn>100){//信地旋回以上
      //turnが200ならvalが100になったときにerror=0
      //turnが170ならvalが70になったときにerror=0
      error=(turn-100)-val;
    }
    else{
      //turnが30ならvalが70になったときにerror=0
      //turnが90ならvalが10になったときにerror=0
      error=(100-turn)-val;
    }
    
    //エラーの方向補正
    if(power<0 && turn<=100)
      error=-error;
    else if(power>=0 && turn>100)
      error=-error;

    error_i+=error;
    error_d=error-error_d;
    
    cout<<"------"<<endl;
    cout<<"time  :"<<time<<endl;
    cout<<"mrot  :"<<mrot<<",mpow:"<<master.GetPower()<<endl;
    cout<<"srot  :"<<srot<<",spow:"<<slave.GetPower()<<endl;
    cout<<"val    :"<<val<<endl;
    cout<<"error  :"<<error<<endl;
    cout<<"error_i:"<<error_i<<endl;
    cout<<"error_d:"<<error_d<<endl;
    tmp=0;
    tmp+=pgain*error;
    tmp+=igain*error_i;
    tmp+=dgain*error_d;
    cur_spow+=tmp/100;
    // 逆走防止
    if(isReverse==1 && cur_spow>0)
      cur_spow=0;
    if(isReverse==0 && cur_spow<0)
      cur_spow=0;
    
    cout<<"cur_spow:"<<cur_spow<<endl;
    
    slave.SetPower(cur_spow);
    ireset+=wait;
    if(ireset>2000){
      error_i=0;
      ireset=0;
    }
  }
  return 0;
}


