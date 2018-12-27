/*
  アーム制御用API
*/

#include "monoatume_cfg.h"
#include "Arm.h"
#include "Log.h"

static int g_armup=ARM_DEFAULT_UP;//一番上まで上がったときの回転角度

void SetArmUp(int deg){
  g_armup=deg;
}

void ArmDown(int pow){
  //アームを下げます
  int rot,timer=0;
  int power=ARM_DOWN_DIR*pow;//実際のパワー
  motor_set_speed(Arm,power,1);
  for(;;){
    dly_tsk(ARM_WAIT);
    rot=nxt_motor_get_count(Arm);
    /*
      パワーの符号と回転角度の変位の符号は一緒(たぶん)
      だからパワーが負なら回転角度が負になるまで回せば終了
    */

    if(power<0){
      if(rot<=0)
	break;
    }
    else{
      if(rot>=0)
	break;
    }
    if(timer>ARM_TIMEOUT)
      break;
    timer+=ARM_WAIT;
  }
  motor_set_speed(Arm,0,1);
}

void ArmUp(int pow){
  //アームを上げます
  int rot,timer=0;
  int power=ARM_UP_DIR*pow;//実際のパワー
  motor_set_speed(Arm,power,1);
  for(;;){
    dly_tsk(ARM_WAIT);
    rot=nxt_motor_get_count(Arm);
    /*
      パワーの符号と回転角度の変位の符号は一緒(たぶん)
      だからパワーが負なら回転角度が負になるまで回せば終了
    */
    if(power<0){
      if(rot<=g_armup)
	break;
    }
    else{
      if(rot>=g_armup)
	break;
    }
    if(timer>ARM_TIMEOUT)
      break;
    timer+=ARM_WAIT;
  }
  motor_set_speed(Arm,0,1);
}
