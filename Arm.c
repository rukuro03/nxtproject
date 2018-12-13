/*
  アーム制御用API
*/

#include "monoatume_cfg.h"
#include "Log.h"

static int g_armup;//一番上まで上がったときの回転角度
void MoveArm(int mode){
  int deg,rot=0,d_rot;
  if(mode==ARM_UP){
    deg=g_armup;
  }
  else{
    deg=0;
  }
  
  if(nxt_motor_get_count(Arm)>deg){
    //今のアーム位置よりも上げなければいけない時
    motor_set_speed(Arm,ARM_POWER_UP,1);
    for(;;){
      d_rot=rot;
      rot=nxt_motor_get_count(Arm);
      if(rot<deg || d_rot-rot==0)
	break;
      dly_tsk(100);
    }
  }
  else{
    //今のアーム位置よりも下げなければいけない時
    motor_set_speed(Arm,ARM_POWER_DOWN,1);
    for(;;){
      d_rot=rot;
      rot=nxt_motor_get_count(Arm);
      if(rot>deg || d_rot-rot==0)
	break;
      dly_tsk(100);
    }
  }
  motor_set_speed(Arm,0,1);
}

void CalibArm(){
  int rot;
  motor_set_speed(Arm,ARM_POWER_DOWN,1);
  LogString("Arm Down");
  for(;;){
    rot=nxt_motor_get_count(Arm);
    dly_tsk(1000);
    rot-=nxt_motor_get_count(Arm);
    if(rot<0)
      rot=-rot;
    if(rot<1){
      nxt_motor_set_count(Arm,0);//一番下に下げた状態でカウントを0にする
      break;
    }
  }
  motor_set_speed(Arm,ARM_POWER_UP,1);
  LogString("Arm Up");
  for(;;){
    rot=nxt_motor_get_count(Arm);
    dly_tsk(1000);
    rot-=nxt_motor_get_count(Arm);
    if(rot<0)
      rot=-rot;
    if(rot<1){
      g_armup=nxt_motor_get_count(Arm);
      break;
    }
  }
  LogString("Arm Down");
  MoveArm(ARM_DOWN);
  motor_set_speed(Arm,0,1);
}
