/*
  ログ出力系関数
  ディスプレイの占有権を待つので恐らく優先度逆転が起こります。
  特に、ディスプレイ用のセマフォを使用するクリティカルセクション内で
  使用すると、デッドロックが起こります
*/
#include "display.h"
#include "monoatume_cfg.h"
#include "kernel_id.h"

static char* g_logitem[LOGNUM];
static int g_logs=0;

void ClearLog(){
  g_logs=0;
}

void DisplayLog(){
  //画面上にログ出力します
  int i;
  wai_sem(Sdisp);
  display_clear(0);
  for(i=0;i<g_logs;i++){
    display_goto_xy(2,HEADER+1+i);
    display_string(g_logitem[i]);
  }
  sig_sem(Sdisp);
}

void LogString(char* str){
  int i;
  if(g_logs==LOGNUM){
    for(i=0;i<LOGNUM-1;i++){
      g_logitem[i]=g_logitem[i+1];
    }
    g_logitem[g_logs-1]=str;
  }
  else{
    g_logitem[g_logs]=str;
    g_logs++;
  }
  DisplayLog();
}

void LogInt(int dat){
  wai_sem(Sdisp);
  display_goto_xy(2,LOGINT);
  display_string("LOG_VAR:");
  display_int(dat,4);
  sig_sem(Sdisp);
}
