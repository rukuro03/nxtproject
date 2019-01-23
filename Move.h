/*
  移動用APIのプロトタイプ宣言
*/
#ifndef MOVE_H
#define MOVE_H

#include "monoatume_cfg.h"

#define MOVE_WAITMAX 500
//回転方向　符号で表すが、MoveLengthに使用するTurnと同一の方向
typedef enum{
  CLOCKWISE =1,
  COUNTER_CLOCKWIZE=-1*CLOCKWISE,
} RotateDirections;

int GetPower();
int GetTurn();
int GetLength();
int GetPgain();
int GetIgain();
int GetDgain();
int GetShaftLength();
int GetWheelRadius();
void SetPgain(int);
void SetIgain(int);
void SetDgain(int);
void SetWheelRadius(int);
void SetShaftLength(int);
void GetMasterSlave(DeviceConstants*,DeviceConstants*);
void MoveSetPower(int);
void MoveSetSteer(int);
void MoveActivate();
void MoveTerminate();
FLGPTN MoveLength(int,int,int);
FLGPTN MLIgnoreTouch(int,int,int);
FLGPTN MoveTurn(int,int);
FLGPTN MoveCurve(int,int,int);
void CheckLength(int);
FLGPTN WaitForOR(FLGPTN);
FLGPTN WaitForAND(FLGPTN);
#endif
