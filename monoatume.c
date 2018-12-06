#include "display.h"
#include "monoatume_cfg.h" //t_service+monoatume
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "ecrobot_base.h"
#include "button.h"
#include "graphics.h"
//自作API用ヘッダファイル
#include "Log.h"
#include "Move.h"
#include "Arm.h"

typedef void (*MFunc)(void);
typedef struct _NameFunc {
  char *name;
  MFunc func;
} NameFunc;

void func_menu();
void func_calib();
void strategy();

//グローバル変数群　できれば使いたくないが組み込みだからね
//グローバル変数には g_ のプレフィックスをつけてください
NameFunc g_MainMenu[] = {
  {"Start",strategy},
  {"Calibration", func_calib},// センサーのキャリブレーション
  {"Power Off", ecrobot_shutdown_NXT},// 電源を切る
};
int g_white[3]={0,0,0};
int g_black[3]={0,0,0};
void (*g_function)(void)=strategy;


void func_calib(){
  int rot=0;
  //色のカリブレーション
  //アームのカリブレーション
  CalibArm();
}



void strategy(){
  FLGPTN flg;

  //機能テスト用に適当に動作を指定しました
  //しばらくまっすぐすすんで
  act_tsk(Ttimer);
  act_tsk(Tmusc);

  //サンプル
  //緑出てはいるだけ
  /*LogString("Start");
  flg = MoveLength(40,0,100);
  LogString("Turn right");
  flg = MoveLength(50, 200, 100);
  LogString("Turn left");
  flg = MoveLength(50,-200,100);
  LogString("Go back");
  flg = MoveLength(70,0,250);
  LogString("and forth");
  flg = MoveLength(-70,0,250);
  //アームを上げる
  MoveArm(ARM_UP);*/

  //ちゃんと集めるやつ
  //赤ボールを取る
  flg = MoveLength(70, 0, 900);
  //バックする
  flg = MoveLength(-70, 40, 300);
  //タイヤを取りに行く
  flg = MoveLength(50, 0, 400);
  //タイヤの前で 3秒停止　アームを上げる　ゆっくり前に進む
  dly_tsk(3000);
  MoveArm(ARM_UP);
  flg = MoveLength(20, 0, 100);
  //方向転換
  MoveArm(ARM_DOWN);
  flg = MoveLength(70, -200, 80);
  //スタート位置に戻る
  flg = MoveLength(70, 0, 500);
 
  
}

void func_menu(){
  //メニュー起動
  int cnt=ARRAYSIZE(g_MainMenu);
  int i;
  nxtButton btn;
  static int menu = 0;
  for (;;) {
    wait_for_release();
    wai_sem(Sdisp);//ディスプレイの占有権待ち
    display_clear(0);
    for (i = 0; i < cnt; i++) {
      if (i == menu) {
	display_goto_xy(1,i+HEADER+1);
	display_string(">");
      }
      display_goto_xy(2, i+1);
      display_string(g_MainMenu[i].name);
    }
    display_update();
    sig_sem(Sdisp);//ディスプレイの占有権返却
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      g_function=g_MainMenu[menu].func;
      return;
    case Cbtn:	// グレーボタン == キャンセル
      ecrobot_shutdown_NXT();
      break;
    case Rbtn:	// 右ボタン == 次へ
      menu++;
      if (menu >= cnt) menu = 0;
      continue;
    case Lbtn:	// 左ボタン == 前へ
      --menu;
      if (menu < 0) menu = cnt-1;
      continue;
    default:	// 複数が押されている場合
      continue;
    }
    break;
  }
}
