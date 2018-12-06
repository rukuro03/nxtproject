/*
  ログ出力系関数
  ディスプレイの占有権を待つので恐らく優先度逆転が起こります。
*/
#include "display.h"
#include "monoatume_cfg.h"
#include "kernel_id.h"

int g_log=HEADER+1;

void ClearLog(){
  g_log=HEADER+1;
  wai_sem(Sdisp);
  display_clear(0);
  sig_sem(Sdisp);
}

void LogString(char* str){
  wai_sem(Sdisp);
  display_goto_xy(2,g_log);
  display_string(str);
  sig_sem(Sdisp);
  g_log++;
  if(g_log>=FOOTER)
    g_log=HEADER+1;
}

void LogInt(int dat){
  wai_sem(Sdisp);
  display_goto_xy(2,g_log);
  display_int(dat,4);
  sig_sem(Sdisp);
  g_log++;
  if(g_log>=FOOTER)
    g_log=HEADER+1;
}
