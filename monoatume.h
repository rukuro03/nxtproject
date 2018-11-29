#ifndef MONOATUME_H
#define MONOATUME_H


#define BASEPOWER 40
// モータの向きが変わったときにプログラムを大幅変更しなくて済むように
#ifdef REVERSE
# define motor_set_speed(M, P, F)	nxt_motor_set_speed((M), (-(P)), (F))
#else
# define motor_set_speed(M, P, F)	nxt_motor_set_speed((M), (P), (F))
#endif

#ifndef _MACRO_ONLY

#include "ecrobot_interface.h" 
int get_light_sensor(int);
// ポートに関する定数値を列挙型として登録
typedef enum {
  Rmotor = NXT_PORT_B,
  Lmotor = NXT_PORT_C,
  Arm = NXT_PORT_A,
  Rtouch = NXT_PORT_S1,
  Ltouch = NXT_PORT_S4,
  Sonar = NXT_PORT_S2,
  Color  = NXT_PORT_S3,
} DeviceConstants;

// イベントフラグ制御用の列挙型
/*
SensorFlugs:センサ用フラグ
色(?bit)>左タッチセンサ>右タッチセンサ>移動タイムアウト>移動到着
*/
typedef enum{
  efEndMove = 1<<0,
  efTOMove = 1<<2,
  efLtouch = 1<<3,
  efRtouch = 1<<4,
} SensorFlugs;

typedef enum{
  Black = 0x00,
  White = 0xff,
} Colors;
  
#endif

#endif
