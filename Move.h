/*
  移動用APIのプロトタイプ宣言
*/
#ifndef MOVE_H
#define MOVE_H

#include "monoatume_cfg.h" //DeviceConstantsがいるから 今後変えようかな

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
FLGPTN MoveTurn(int,int,DeviceConstants);//今後これがintだけになるかも
void CheckLength(int);
void SetTimeOut(int);
FLGPTN WaitForOR(FLGPTN);
FLGPTN WaitForAND(FLGPTN);

#endif
