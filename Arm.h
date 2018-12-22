/*
  アーム制御用APIのプロトタイプ宣言
*/


#ifndef ARM_H
#define ARM_H
//アームを上げる/下げるときの方向(モータの回転方向の符号)
#define ARM_UP_DIR -1
#define ARM_DOWN_DIR 1
//アームを上げるか下げるかの選択
#define ARM_UP 1
#define ARM_DOWN 0
//アームの更新待ち時間
#define ARM_WAIT 5
//アームのタイムアウト(ms)
#define ARM_TIMEOUT 1000

void ArmDown(int);
void ArmUp(int);
#endif
