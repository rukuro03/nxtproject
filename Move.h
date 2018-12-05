/*
  移動用APIのプロトタイプ宣言
*/
#ifndef MOVE_H
#define MOVE_H

#include "monoatume.h" //DeviceConstantsがいるから 今後変えようかな
int GetPower();
int GetTurn();
int GetLength();
int GetPgain();
int GetIgain();
int GetDgain();
void SetPgain(int);
void SetIgain(int);
void SetDgain(int);
void GetMasterSlave(DeviceConstants*,DeviceConstants*);
void MoveSetPower(int);
void MoveSetSteer(int);
void MoveActivate();
void MoveTerminate();
FLGPTN MoveLength(int,int,int);
FLGPTN MoveTurn(int,int,DeviceConstants);//今後これがintだけになるかも
void CheckLength(int);
void SetTimeOut(int);
FLGPTN WaitForOR(FLGPTN);
FLGPTN WaitForAND(FLGPTN);

#endif
