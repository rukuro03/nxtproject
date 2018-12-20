/*
  全体で必要な定義を行うファイル
*/

#ifndef MONOATUME_H
#define MONOATUME_H

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
色(3bit)>左タッチセンサ>右タッチセンサ>移動タイムアウト>移動到着
000000000ccclrta
*/
typedef enum{
  efEndMove = 1<<0,
  efTOMove = 1<<2,
  efLtouch = 1<<3,
  efRtouch = 1<<4,
  efBlack = 0x00<<5,
  efBlue = 0x01<<5,
  efGreen = 0x02<<5,
  efYellow = 0x03<<5,
  efOrange = 0x04<<5,
  efRed = 0x05<<5,
  efWhite = 0x06<<5,
  efUnknown = 0x07<<5
} SensorFlugs;


//定義・宣言群
#define ARRAYSIZE(A)	(sizeof((A)) / sizeof((A)[0]))
#define MACHINE_NAME "GOHAN"
//標準パワー?
#define BASE_POWER 50
//タイマー初期値
#define TIME_LEFT 120

//アームを上げる/下げるときの方向
#define ARM_POWER_UP -1
#define ARM_POWER_DOWN 1
//アームを上げるか下げるかの選択
#define ARM_UP 1
#define ARM_DOWN 0
//ヘッダー・フッター位置 
#define HEADER 0
#define FOOTER 7
//ログ総数
#define LOGNUM FOOTER-HEADER-2
//数値ログ位置
#define LOGINT LOGNUM+1


//monoatume.cで定義される関数の宣言
void Strategy();
void RedBall();
void Tire();
#endif

#endif
