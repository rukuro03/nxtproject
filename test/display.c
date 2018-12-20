#include "display.h"
#include <stdlib.h>

static WINDOW* display;

void Init(){
  initscr();
  keypad(stdscr,true);
  noecho();
  curs_set(0);
  display=newwin(DISPLAY_HEIGHT+2,DISPLAY_WIDTH+2,4,4);
  display_goto_xy(0,0);
  refresh();
}
void End(){
  endwin();
}
void Dummy(){
  printf("Dummy is called.\n");
}
void DummyInt(int i){
  printf("DummyInt with parameter %d is called.\n",i);
}
int ReturnInt(){
  return 12;
}
void display_goto_xy(int x,int y){
  wmove(display,y+1,x+1);
}
void display_string(char* str){
  waddstr(display,str);
}
void display_int(int i,int n){
  char str[n*2];//念の為バッファは多く取っておく
  sprintf(str,"%d",i);
  display_string(str);
}
void display_update(){
  box(display,ACS_HLINE,ACS_VLINE);
  wrefresh(display);
}
void display_clear(int i){
  wclear(display);
  if(i==1)
    display_update();
}
nxtButton get_btn(){
  return getch();
}
void ecrobot_shutdown_NXT(){
  End();
  exit(0);
}
