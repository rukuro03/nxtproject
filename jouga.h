/*
 * jouga.h
 *	ロボットに関する定数などを保持
 */

#ifndef JOUGA_H
#define JOUGA_H

#ifndef _MACRO_ONLY
#include "ecrobot_interface.h" // ヘッダファイル中にガードあり

extern int lval, cval, llow, lhigh, clow, chigh;
int get_light_sensor(int);
#endif

// 一番最初のプログラム変更はおそらくこれらの値の変更
#define	LOWVAL (470)
#define	HIGHVAL (640)
#define LOWPOWER (20)
#define HIGHPOWER (30)

#define BASEPOWER 40

#ifndef _MACRO_ONLY
// ポートに関する定数値を列挙型として登録
typedef enum {
	Rmotor = NXT_PORT_B,
	Lmotor = NXT_PORT_C,
	Arm=NXT_PORT_A,
	Rtouch = NXT_PORT_S1,
	Ltouch = NXT_PORT_S4,
	Sonar = NXT_PORT_S2,
	Color  = NXT_PORT_S3,
	Light = 0,
} DeviceConstants;
#endif

// モータの向きが変わったときにプログラムを大幅変更しなくて済むように
#ifdef REVERSE
# define motor_set_speed(M, P, F)	nxt_motor_set_speed((M), (-(P)), (F))
#else
# define motor_set_speed(M, P, F)	nxt_motor_set_speed((M), (P), (F))
#endif

#endif
