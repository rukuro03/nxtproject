#pragma once
#include <curses.h>
#define HEADER 0
#define FOOTER 7
#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 8
#define ARRAYSIZE(A)	(sizeof((A)) / sizeof((A)[0]))

typedef enum {
  Obtn='y',
  Cbtn='q',
  Rbtn=KEY_RIGHT,
  Lbtn=KEY_LEFT,
} nxtButton;

void Init();
void End();
void Dummy();
void DummyInt(int);
int ReturnInt();
void display_goto_xy(int,int);
void display_string(char*);
void display_int(int,int);
void display_update();
void display_clear(int);
nxtButton get_btn();
void ecrobot_shutdown_NXT();
