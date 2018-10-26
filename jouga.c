/*
 *	TOPPERS/JSPを用いたライントレーサーのサンプルコード
 */ 

#include "display.h"	// 変更したバージョンを使うために先頭でinclude

#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "ecrobot_base.h"

#include "jouga_cfg.h"
#include "jouga.h"
#include "music.h"
#include "button.h"
#include "graphics.h"

#define ARRAYSIZE(A)	(sizeof((A)) / sizeof((A)[0]))
//1-4 x 3-3 x 3-10x 6-8
#define STRAIGHT 50//まっすぐ走ってると認識する幅 
/* 型や関数の宣言 */
typedef void (*MFunc)(void);
typedef struct _NameFunc {
  char *name;
  MFunc func;
} NameFunc;

void calibration_func(void);
void sync_motor(void);
void setting_func(void);
void jouga_light(void);
void jouga_color(void);
void jouga_dual(void);
void algorithm_light(void);
void algorithm_color(void);
void algorithm_dual(void);
void algorithm_straight2(void);
void jouga_straight(void);

/* 外部変数の定義 */
char name[17];
int lval, cval;
int llow = LOWVAL, lhigh = HIGHVAL;
int clow = LOWVAL, chigh = HIGHVAL;
int highpower=50;
int pgain_low=2,pgain_high=5,dgain=3;
int dbg1, dbg2;//デバッグ用の変数
void (*jouga_algorithm)(void) = algorithm_dual;	// デフォルトの設定

NameFunc MainMenu[] = {
  {"Main Menu", NULL},
  {"Calibration", calibration_func},	// センサーのキャリブレーション
  {"Sync_Motor",sync_motor},            // モータの確認
  {"Setting", setting_func},            // ゲイン等の設定を行う
  {"Dual", jouga_dual},			// 2つを使うアルゴリズムを選択
  {"Start", NULL},			// ライントレースの開始
  {"Straight", jouga_straight},		// ペナルティ計測用
  {"Exit", ecrobot_restart_NXT},	// OSの制御に戻る
//  {"Power Off", ecrobot_shutdown_NXT},	// 電源を切る
};


/* ライトセンサーやカラーセンサーの値を0-1023で読み込む(小さいほど暗い) */
int
get_light_sensor(int Sensor)
{
  if (Sensor == Light) {
    return 1024 - 1 - ecrobot_get_light_sensor(Sensor);
  } else {
    return ecrobot_get_nxtcolorsensor_light(Sensor);
  }
}

/* メニューを表示して選択されるのを待つ */
void
func_menu(NameFunc *tbl, int cnt)
{
  int i;
  static int menu = 1;
  nxtButton btn;

  for (;;) {
    display_clear(0);
    display_goto_xy(0, 0);
    display_string_inverted(tbl[0].name);	// 変更済みのdisplay.cのみ
    for (i = 1; i < cnt; i++) {
      if (i == menu) {
        nxt_display_dish(nxt_display_set, 6, i * 8 + 3, 3);
      }
      display_goto_xy(2, i);
      display_string(tbl[i].name);
    }
    display_update();
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      if (tbl[menu].func == NULL)
	break;
      tbl[menu].func();		// メニューの項目を実行
      continue;
    case Cbtn:	// グレーボタン == キャンセル
      continue;
    case Rbtn:	// 右ボタン == 次へ
      menu++;
      if (menu >= cnt) menu = 1;
      continue;
    case Lbtn:	// 左ボタン == 前へ
      --menu;
      if (menu <= 0) menu = cnt - 1;
      continue;
    default:	// 複数が押されている場合
      continue;
    }
    break;
  }
}

// 白と黒のセンサーの読み取り値を校正 
void
calibration_func(void)
{
  nxtButton btn;
  int lmin, lmax;
  int cmin, cmax;
  int i;

  display_clear(0);
  motor_set_speed(Lmotor, LOWPOWER / 3 + 10, 1);
  motor_set_speed(Rmotor, LOWPOWER / 3 + 10, 1);
  lmin = lmax = get_light_sensor(Light);
  cmin = cmax = get_light_sensor(Color);

  // しばらくの間データを取得
  for (i = 0; i < 150; i++) {
    dly_tsk(20);
    lval = get_light_sensor(Light);
    cval = get_light_sensor(Color);
    if (lval < lmin) lmin = lval;
    if (lval > lmax) lmax = lval;
    if (cval < cmin) cmin = cval;
    if (cval > cmax) cmax = cval;
    display_goto_xy(0, 1);
    display_string("cur: ");
    display_int(lval, 4);
    display_int(cval, 4);
    display_goto_xy(0, 3);
    display_string("min: ");
    display_int(lmin, 4);
    display_int(cmin, 4);
    display_goto_xy(0, 5);
    display_string("max: ");
    display_int(lmax, 4);
    display_int(cmax, 4);
    display_update();
  }
  // データ取得終了
  motor_set_speed(Lmotor, 0, 0);
  motor_set_speed(Rmotor, 0, 0);
  display_goto_xy(3, 7);
  display_string("Is this OK?");
  display_update();
  do {
    btn = get_btn();
    if (btn == Cbtn) return;
  } while (btn != Obtn);
  llow = lmin;
  lhigh = lmax;
  clow = cmin;
  chigh = cmax;
}

void
sync_motor(){
  /*
    左右のモータのシンクロ率を測ります
    パワーを0から90まで上げながら、
    左右のモータの回転速度(度/10ミリ秒)を表示します
    0.5秒計測する→表示する→オレンジ押す→パワー上がる…
    のループです　
  */

  int pow=0,sum=0;
  int i;
  int ldat[10]={0};
  int rdat[10]={0};
  nxtButton btn;
  for(i=0;i<10;i++){
    //パワーを上げながら値(度)を取得していきます
    pow = i*10;
    //わかってる値を表示
    display_clear(0);
    display_goto_xy(0,0);
    display_string("Testing...");
    display_goto_xy(0,1);
    display_string("Pow  L  R");
    display_goto_xy(0,2);
    display_int(pow,4);
    display_update();
    //モータ回転開始
    motor_set_speed(Rmotor, pow, 1);//パワーを与え
    motor_set_speed(Lmotor, pow, 1);
    nxt_motor_set_count(Rmotor, 0);//回転角初期化？
    nxt_motor_set_count(Lmotor, 0);
    dly_tsk(1000);//1s待機
    rdat[i]=nxt_motor_get_count(Rmotor);//回転角度取得
    ldat[i]=nxt_motor_get_count(Lmotor);
    //回転速度表示
    display_string(" ");
    display_int(ldat[i],4);
    display_string(" ");
    display_int(rdat[i],4);
    display_goto_xy(0,3);
    display_string(">Press Orange");
    display_update();
    do{
      btn = get_btn();//オレンジボタンで次に進む
    }while(btn!=Obtn);
    /*
      Testing...
      Pow  L  R
      10  1300 1000
      >Press Orange
      みたいな表示が出ると思う
    */
  }
  //一応ひと目でわかるように適当な誤差値を出すようにする
  //とりあえず小さいほどいい感じ
  for(i=0;i<10;i++){
    sum += (rdat[i] - ldat[i]) * (rdat[i] - ldat[i]);
  }
  display_clear(0);
  display_goto_xy(0,0);
  display_string("Result:");
  display_int(sum,4);
  display_goto_xy(0,1);
  display_string("Press Orange");
  display_update();
  /*この間は
    Result:1000
    Press Orange
    　みたいな表示が出ている
  */
  do{
    btn = get_btn();
  }while(btn != Obtn);
  motor_set_speed(Rmotor, 0, 1);
  motor_set_speed(Lmotor, 0, 1);
}

void
setting_func(){//ゲイン設定用変数
  /*
    使い方：
    1.右左ボタンで設定したい変数を選ぶ
    2.オレンジボタンを押すと設定モードに切り替わる
    3.右左ボタンで値を変更
    4.オレンジボタンを押すと決定 
  */
  int state=0,pos=0;//state:0=変更値選択,1=値設定 pos:現在の選択位置
  int local_pgain_low=pgain_low;
  int local_pgain_high=pgain_high;
  int local_dgain=dgain;
  int local_highpower=highpower;
  int max_column=5;//行数
  nxtButton btn;
  for(;;){
    display_clear(0);
    display_goto_xy(1, 0);
    display_string("PGAIN_LOW");
    display_int(local_pgain_low,4);
    display_goto_xy(1, 1);
    display_string("PGAIN_HIGH");
    display_int(local_pgain_high,4);
    display_goto_xy(1, 2);
    display_string("DGAIN");
    display_int(local_dgain,4);
    display_goto_xy(1, 3);
    display_string("HIGHPOWER");
    display_int(local_highpower,4);
    display_goto_xy(1, 4);
    display_string("END");
    display_goto_xy(0,pos);//現在位置のカーソル表示
    display_string(">");
    display_update();
    /*
      画面は
      >PGAIN_LOW
       PGAIN_HIGH
       DGAIN
       HIGHPOWER
       END
       みたいな表示
     */    
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 決定
      if (state == 0 ){//位置選択中
	if(pos!=4)//ENDの位置に無い
	  state=1;
	else
	  return;
      }
      else {//設定中　値を適用
	state=0;//位置選択中に戻る
	switch(pos){
	case 0://PGAIN_LOW
	  pgain_low=local_pgain_low;
	  break;
	case 1://PGAIN_HIGH
	  pgain_high=local_pgain_high;
	  break;
	case 2://DGAIN
	  dgain=local_dgain;
	  break;
	case 3://HIGHPOWER
	  highpower=local_highpower;
	  break;
	default://ENDのとこ？エラーだべ・一応抜けとく
	  return;
	}
      }
      continue;
    case Cbtn:	// グレーボタン 終了/何もしない
      if(state==0)
	return;
      continue;
    case Rbtn:	// 右ボタン 下に移動/値プラス
      if(state == 0){
	if(pos<max_column)
	  pos++;
      }
      else{
	switch(pos){
	case 0://PGAIN_LOW
	  local_pgain_low++;
	  break;
	case 1://PGAIN_HIGH
	  local_pgain_high++;
	  break;
	case 2://DGAIN
	  local_dgain++;
	  break;
	case 3://HIGHPOWER
	  local_highpower++;
	  break;
	default://END 何もしない
	  break;
	}
      }
      continue;
    case Lbtn:	// 左ボタン 上に移動/値マイナス
      if(state == 0){
	if(pos>0)
	  pos--;
      }
      else{
	switch(pos){
	case 0://PGAIN_LOW
	  local_pgain_low--;
	  break;
	case 1://PGAIN_HIGH
	  local_pgain_high--;
	  break;
	case 2://DGAIN
	  local_dgain--;
	  break;
	case 3://HIGHPOWER
	  local_highpower--;
	  break;
	default://END 何もしない
	  break;
	}
      }
      continue;
    default:	// 複数が押されている場合
      continue;
    }
  }
}

/*
 * アルゴリズム選択関数群
 *	ライントレースに使うアルゴリズムを選択する
 *	メニュー実現用として利用
 */

void
jouga_dual(void)
{
  jouga_algorithm = algorithm_dual;
}
void jouga_straight(void){
  jouga_algorithm=algorithm_straight2;
}


/*
 * アルゴリズム実現関数群
 *	実際に機体を動かす
 *	周期タイマがセマフォを操作することで定期的に起動される
 *	ここを直すことで考えているアルゴリズムを実現できる
 */
void
algorithm_light(void)
{
  for(;;) {
    wai_sem(Stskc);	// セマフォを待つことで定期的な実行を実現
    lval = get_light_sensor(Light);
    if (lval > (lhigh + llow) / 2) {		// 閾値より大きいとき
      motor_set_speed(Rmotor, HIGHPOWER, 1);
      motor_set_speed(Lmotor, LOWPOWER, 1);
    } else {					// 閾値より小さいとき
      motor_set_speed(Lmotor, HIGHPOWER, 1);
      motor_set_speed(Rmotor, LOWPOWER, 1);
    }
  }
}

void
algorithm_color(void)
{
  for(;;) {
    wai_sem(Stskc);	// セマフォを待つことで定期的な実行を実現
    cval = get_light_sensor(Color);
    if (cval > (chigh + clow) / 2) {		// 閾値より大きいとき
      motor_set_speed(Lmotor, HIGHPOWER, 1);
      motor_set_speed(Rmotor, LOWPOWER, 1);
    } else {					// 閾値より小さいとき
      motor_set_speed(Rmotor, HIGHPOWER, 1);
      motor_set_speed(Lmotor, LOWPOWER, 1);
    }
  }
}


void
algorithm_dual(void)
{
  //##SEARCHDUAL  探すのが面倒なので
  for(;;) {
    //白＝>うごいて
    //ライトセンサー 白＝小さい
    //色センサー 白=大きい
    static int lval_prev;
    static int cval_prev;
    wai_sem(Stskc);	// セマフォを待つことで定期的な実行を実現
    lval = get_light_sensor(Light);
    cval = get_light_sensor(Color);
    /*
      取得した値がカリブレーションしたときより大きくなることもありうる
    */
    if(lval < llow)
      llow = lval;
    else if(lval > lhigh)
      lhigh = lval;
    if(cval < clow)
      clow = cval;
    else if(cval > chigh)
      chigh = cval;
      
    int lmid=llow+(lhigh-llow)/2;
    int cmid=clow+(chigh-clow)/2;
    int lval_dif = 100 * (lval - lval_prev) / (lhigh-llow);
    int cval_dif = 100 * (cval - cval_prev) / (chigh-clow);
    int pgain=pgain_high;
    int is_straight=0;
    if(lval - lval_prev < STRAIGHT && lval - lval_prev > -STRAIGHT){
      if(cval - cval_prev < STRAIGHT && cval - cval_prev > -STRAIGHT){
	pgain=pgain_low;
      }
    }
    /*
      分数のところの説明
      分子：現在の値が中央値からどれだけずれているか
      分母：端っこから端っこまでの距離の絶対値
    */
    int lturn = pgain * 100 * (lval - lmid)/(lhigh - llow) + dgain * lval_dif;
    int cturn = pgain * 100 * (cval - cmid)/(chigh - clow) + dgain * cval_dif;
    lturn /= 2;
    cturn /= 2;    
    if(lturn < -highpower)
      lturn = -highpower;
    if(cturn < -highpower)
      cturn = -highpower;
    lval_prev = lval;
    cval_prev = cval;
    dbg1=is_straight;
    dbg2=0;
    //ライトセンサ:右 カラーセンサ：左
    //3がライト　4がカラー
    motor_set_speed(Rmotor, highpower+lturn, 1);
    motor_set_speed(Lmotor, highpower+cturn, 1);
  }
}


/*
 * ペナルティ計測用の関数
 *	直進して1500mm進んで止まるだけ
 */

void
algorithm_straight2(void){
  //モータのパワーを保存しておく変数
  int L_pow = LOWPOWER;  
  int R_pow = LOWPOWER;
  int lval_prev=0,cval_prev=0;
  int lmid = llow + (lhigh - llow) / 2;
  int cmid = clow + (chigh - clow) / 2;
  int R_rot, L_rot;//回転数を保存する変数
  int Wheel_radius = 30;//タイヤの半径(mm)
  int Wheel_circum = Wheel_radius * 2 * 3.14;//タイヤの円周の長さ(mm)
  int PowerArray[20][2];//パワーの保存
  //左右のモータのスピードが等しくなった時のパワーを保存する変数
  //とりあえず初期値としてLOWPOWERを入れておく
  int L_equal_pow = LOWPOWER;
  int R_equal_pow = LOWPOWER;
  int count=0,pow_count=0;
  int pow_count_max=20;

  //最初に0.2s加速
  motor_set_speed(Lmotor,20, 1);
  motor_set_speed(Rmotor, 20, 1);
  dly_tsk(300); 
  motor_set_speed(Lmotor,30, 1);
  motor_set_speed(Rmotor, 30, 1);
  dly_tsk(300);
  motor_set_speed(Lmotor,40, 1);
  motor_set_speed(Rmotor, 40, 1);
  dly_tsk(300);
  nxt_motor_set_count(Rmotor, 0);//カウントの初期化
  nxt_motor_set_count(Lmotor, 0);
  /***ライン上を走るとき***/
  for (;;) {
    wai_sem(Stskc);
    //白黒値を取得 
    lval = get_light_sensor(Light);
    cval = get_light_sensor(Color);
    //取得した値がカリブレーションしたときより大きくなることもありうる
    if(lval < llow)
      llow = lval;
    else if(lval > lhigh)
      lhigh = lval;
    if(cval < clow)
      clow = cval;
    else if(cval > chigh)
      chigh = cval;
    int lval_dif = 100 * (lval - lval_prev) / (lhigh-llow);
    int cval_dif = 100 * (cval - cval_prev) / (chigh-clow);
    int pgain=2;
    /*
      分数のところの説明
      分子：現在の値が中央値からどれだけずれているか
      分母：端っこから端っこまでの距離の絶対値
    */
    int lturn = pgain * 100 * (lval - lmid)/(lhigh - llow) + dgain * lval_dif;
    int cturn = pgain * 100 * (cval - cmid)/(chigh - clow) + dgain * cval_dif;
    lturn /= 2;
    cturn /= 2;    
    if(lturn < -LOWPOWER)
      lturn = -LOWPOWER;
    if(cturn < -LOWPOWER)
      cturn = -LOWPOWER;
    lval_prev = lval;
    cval_prev = cval;

    //回転数を取得
    R_rot = nxt_motor_get_count(Rmotor);

    R_pow = LOWPOWER + lturn;
    L_pow = LOWPOWER + cturn;
    dbg1=1;
    dbg2=2;
    motor_set_speed(Lmotor, L_pow, 1);
    motor_set_speed(Rmotor, R_pow, 1);
    PowerArray[pow_count][0]=L_pow;
    PowerArray[pow_count][1]=R_pow;
    if(pow_count<pow_count_max)
      pow_count++;
    else
      pow_count=0;
    /***ラインが途絶えたらこのforループを抜ける***/
    if (Wheel_circum * (double)R_rot / 360 > 850)
      break;
  }

  /***ラインが途絶えたあと***/
  pow_count=0;
  for(;;){
    wai_sem(Stskc);
    lval = get_light_sensor(Light);
    cval = get_light_sensor(Color);
    dbg1=2;
    dbg2=1;

    if(lval < lmid || cval < cmid){ //黒を見つけたら9cm進んで止まる
      break;

    }else{
      motor_set_speed(Lmotor, PowerArray[pow_count][0], 1);
      motor_set_speed(Rmotor, PowerArray[pow_count][1], 1);
      if(pow_count<pow_count_max)
	pow_count++;
      else
	pow_count=0;
    }
  } 
  nxt_motor_set_count(Rmotor, 0);//カウントの初期化
  nxt_motor_set_count(Lmotor, 0);
  for(;;){
    //回転数を計測
    wai_sem(Stskc);
    dbg1=3;
    dbg2=1;
    R_rot = nxt_motor_get_count(Rmotor);
    //90mm進む(9cm)
    if (Wheel_circum * (double)R_rot / 360 > 90) {
      //円周x回転角度÷360=進んだ距離
      // モータの停止
      motor_set_speed(Rmotor, 0, 0);
      motor_set_speed(Lmotor, 0, 0);
      break;
    }
  }
}

/*
 * TASK: InitTsk
 *	初期設定を行うタスク
 */
void
  InitTsk(VP_INT exinf)
{
  display_clear(0);	// なにはともあれ、画面をクリア
  display_goto_xy(2, 3);
  display_string("Initializing");
  display_update();
  // カラーセンサーを使う場合にはライトセンサーとして使う
  // REDが一番ダイナミックレンジが広いようなので、あえてWHITEでなくREDで
  ecrobot_set_nxtcolorsensor(Color, NXT_LIGHTSENSOR_RED);
  ecrobot_get_bt_device_name(name);	// システム名の取得

  act_tsk(Tmain);
}

/*
 * TASK: MainTsk
 *	周期起動用のタイマを起動して終了
 */
void
MainTsk(VP_INT exinf)
{
  // ここにくるのにボタンを押しているので、
  // ボタンが押されていない状態になるまで待つ
  wait_for_release();
  wai_sem(Snbtn);	// ボタンに関する権利を取得
  // メインメニューの表示
  func_menu(MainMenu, ARRAYSIZE(MainMenu));
  sig_sem(Snbtn);	// ボタンに関する権利を開放

  // 画面をきれいにする
  display_clear(0);
  display_goto_xy(0, 0);
  display_update();

  // BGM用のタスクを起動
  act_tsk(Tmusc);

  // 移動用のタスクを起動
  act_tsk(Tmove);

  // 表示用のタスクを定期起動するためのタイマを起動
  sta_cyc(Cdisp);
}

/*
 * TASK: MoveTsk
 *	実際に機体を動かす
 *	周期タイマがセマフォを操作することで定期的に起動される
 *	たぶん、ここを直すことで考えているアルゴリズムを実現できる
 */
void
MoveTsk(VP_INT exinf)
{
  sta_cyc(Cmove);	// 定期的にセマフォを上げるタイマ

  (*jouga_algorithm)();	// 実際の処理
}

/*
 * TASK: MuscTsk
 *	BGMを奏でる
 *	実体はmusic.cにある
 */
void
MuscTsk(VP_INT exinf)
{
  // 延々と大学歌を奏で続ける
  for (;;) {
    play_notes(TIMING_chiba_univ, 8, e_p);
  }
}

/*
 * TASK: DispTsk
 *	通常動作時にシステム内の様子を表示
 *	周期タイマにより定期的に起動される
 */
void
DispTsk(VP_INT exinf)
{
  display_clear(0);

  /* システム名の表示 */
  display_goto_xy(0, 0);
  display_string(name);
  display_string(" status");

  /* センサーの読み取り値の表示 */
  display_goto_xy(3, 3);
  display_string("light:");
  display_int(lval, 4);
  display_goto_xy(3,4);
  display_string("color:");
  display_int(cval, 4);
  display_goto_xy(3,5);
  display_string("dbg1:");
  display_int(dbg1, 4);
  display_goto_xy(3,6);
  display_string("dbg2:");
  display_int(dbg2, 4);
  display_update();
}

/*
 * TASK: IdleTsk
 *	Idle時に動作する（優先順位は低い）
 *	Cyclic Timerでなくdly_tskしているのは、
 *	セマフォで待っているときに複数起動しても意味がないため
 */
void
IdleTsk(VP_INT exinf)
{
  for (;;) {
    wai_sem(Snbtn);		// InitTskとNXTボタンを取り合う
    check_NXT_buttons();
    if (ecrobot_is_ENTER_button_pressed()) {
      stp_cyc(Cmove);
      stp_cyc(Cdisp);
      ter_tsk(Tmove);
      ter_tsk(Tmusc);
      ter_tsk(Tmain);
      nxt_motor_set_speed(Rmotor, 0, 0);
      nxt_motor_set_speed(Lmotor, 0, 0);
      act_tsk(Tmain);
    }
    sig_sem(Snbtn);		// NXTボタンの権利を返却
    dly_tsk(10);
  }
}

/*
 * TASK: ColsTsk
 *	Idle時にカラーセンサー用に値を読み込む
 */
void
ColsTsk(VP_INT exinf)
{
  for (;;) {
    ecrobot_process_bg_nxtcolorsensor();
    dly_tsk(2);
  }
}

/*
 * 周期タイマ
 *	タスクを定期的に起動するだけ
 */
void
MoveCyc(VP_INT exinf)
{
  isig_sem(Stskc);	// MoveTskを進めるためにセマフォを操作
}

void
DispCyc(VP_INT exinf)
{
  iact_tsk(Tdisp);	// DispTskを定期的に起動
}

/* OSにより1msごとに呼び出される */
void
jsp_systick_low_priority(void)
{
  if (get_OS_flag()) {
    isig_tim();		// 今回はタイマを使っているのでこの呼び出しが必要
  }
}

/* システムの初期化ルーチン */
void
ecrobot_device_initialize(void)
{
  nxt_motor_set_speed(Rmotor, 0, 0);
  nxt_motor_set_speed(Lmotor, 0, 0);
  ecrobot_init_nxtcolorsensor(Color, NXT_COLORSENSOR);
  ecrobot_set_light_sensor_active(Light);
}

/* システム停止時に呼ばれるルーチン */
void
ecrobot_device_terminate(void)
{
  nxt_motor_set_speed(Rmotor, 0, 1);
  nxt_motor_set_speed(Lmotor, 0, 1);
  ecrobot_term_nxtcolorsensor(Color);
  ecrobot_set_light_sensor_inactive(Light);
}
