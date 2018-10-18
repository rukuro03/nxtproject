/*
アルゴリズムのテストができるプログラムです。
アルゴリズム関数を変更したりテストケースを追加したりして
よりよいアルゴリズムを考えましょう！
 */


#include <stdio.h>
#define HIGHPOWER 100
#define GAIN 1
int lhigh=521,llow=293;
int chigh=487,clow=312;

//左側にライトセンサ、右側に色センサがあると課程
void
algorithm_dual(int lval,int cval)
{
  //白＝>うごいて
  //ライトセンサー 白＝小さい
  //色センサー 白=大きい
  int lmid=llow+(lhigh-llow)/2;
  int cmid=clow+(chigh-clow)/2;
  int lturn,cturn;
  
  lturn = GAIN * 100 * (lval - lmid)/(lhigh - lmid);
  cturn = GAIN * 100 * (cval - cmid)/(chigh - cmid);
  if(lturn>HIGHPOWER)
    lturn=HIGHPOWER;
  if(cturn>HIGHPOWER)
    cturn=HIGHPOWER;
  
  printf("%d\n",lturn);
  printf("%d\n",cturn);
  //motor_set_speed(Rmotor, HIGHPOWER-lturn, 1);
  //motor_set_speed(Lmotor, HIGHPOWER+cturn, 1);
  printf("LEFT:%d\n",HIGHPOWER-lturn);
  printf("RIGHT:%d\n",HIGHPOWER+cturn);
  printf("------\n");
}

void main(){
  printf("middle:(%d,%d)\n",llow+(lhigh-llow)/2,clow+(chigh-clow)/2);
  algorithm_dual(400,400);//中間：まっすぐはしれ
  algorithm_dual(500,480);//左にまがれ=>左ストップ右多く
  algorithm_dual(320,320);//逆
  algorithm_dual(500,400);//左折だがそこまで右は速くならない
  algorithm_dual(400,320);//逆
  algorithm_dual(500,320);//車体が斜めになったとか　想定したくない状況
  algorithm_dual(350,500);//cmaxをはみ出す場合
  algorithm_dual(180,350);//llowをはみ出す場合
}
