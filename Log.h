/*
 ログ用関数のプロトタイプ宣言
*/
#ifndef LOG_H
#define LOG_H

//ログ総数
#define LOGNUM FOOTER-HEADER-2
//数値ログ位置
#define LOGINT LOGNUM+1
void ClearLog();
void LogString(char*);
void LogInt(int);

#endif
