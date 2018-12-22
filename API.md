## MEMO -API解説-
### By ご飯システム班の一人
新しく作ったAPIの解説を書きます。ECrobotやTOPPERSのAPIについてはカバーしません。

### 注意
このファイルはマークダウン言語で書かれています。txtファイルとしても読めると
思いますが、マークダウンならちょっと読みやすいです。
ブラウザ上で使えるマークダウンビューアとかを使うといいと思います。
github上なら自動で綺麗に表示されるはずです。

## Move.h
```c
int GetPower()
int GetTurn()
int GetLength()
int GetPgain()
int GetIgain()
int GetDgain()
int GetShaftLength()
int GetWheelRadius()
```
### 引数
無し
### 動作
対応するグローバル変数の値を返す。オブジェクト指向言語で言うprivateフィールド
に対するゲッターと同様の意味合いで用いる

---
```c
void SetPgain(int num);
void SetIgain(int num);
void SetDgain(int num);
void SetWheelRadius(int num);
void SetShaftLength(int num);
```
### 引数
**num**:変数に代入する値
### 動作
対応するグローバル変数の値を変更する。オブジェクト指向言語で言うprivateフィールド
に対するセッターと同様の意味合いで用いる。特にSetMenu関数内で使われる

---
```c
void MoveSetPower(int pow)
```
### 引数
**pow**:モータに設定するパワー
### 動作
モータの速度を指定したパワーに変更する

---
```c
void MoveSetSteer(int turn)
```
### 引数
**turn**:設定したい左右のモータの回転比
### 動作
左右のモータの回転比を設定する。この回転比の仕様は仕様書に記載してある

---
```c
void MoveActivate()
```
### 引数
無し
### 動作
MoveTskを起動する。

---
```c
void MoveTerminate()
```
### 引数
無し
### 動作
MoveTskとそれに付随して呼ばれるタスクを終了し、モータを止める。

---
```c
FLGPTN WaitForOR(FLGPTN flg)
```
### 引数
**flg**:終了条件となるフラグ
### 動作
センサー用イベントフラグがflgで指定したビット列と1ビットでも合えば、
待ち状態から抜ける。戻り値は待ち状態終了時の実際のフラグの状態

---
```c
FLGPTN WaitForAND(FLGPTN flg)
```
### 引数
**flg**:終了条件となるフラグ
### 動作
センサー用イベントフラグがflgで指定したビット列と完全に一致した際、
待ち状態から抜ける。戻り値は待ち状態終了時の実際のフラグの状態

---
```c
void CheckLength(int length)
```
### 引数
**length**:指定する移動距離(mm)
### 動作
移動距離を指定して、CheckTskを起動する

---
```c
FLGPTN MoveLength(int pow,int turn,int length)
```
### 引数
**pow**:モータに設定するパワー

**turn**:設定したい左右のモータの回転比

**length**:移動させたい距離(mm)
### 動作
MoveSetPower、MoveSetSteer、CheckLength、MoveActivate、WaitForOR
をこの順番で呼ぶ。これらの関数に対する引数は与えられた引数から適切なものを選択する。
WaitForORには移動完了、時間切れ、いずれかのタッチセンサが押される、を意味する
フラグが与えられる。WaitForORの戻り値を返す

---
```c
FLGPTN MLIgnoreTouch(int pow,int turn,int length)
```
### 引数
**pow**:モータに設定するパワー

**turn**:設定したい左右のモータの回転比

**length**:移動させたい距離(mm)
### 動作
MoveLengthとほぼ同一の関数だが、タッチセンサの状態を無視する。

## Arm.h
---
```c
void ArmDown(int pow)
```
### 引数
**pow**:アーム用モータに設定するパワー
### 動作
アームを下げる

---
```c
void ArmUp(int pow)
```
### 引数
**pow**:アーム用モータに設定するパワー
### 動作
アームを上げる

## Log.h
```c
void ClearLog()
```
### 引数
無し
### 動作
ログデータの消去、画面のクリア

---
```c
void LogString(char* str)
```
### 引数
**str**:ログデータに追加する文字列リテラルへのポインタ
### 動作
文字列をログに追加し、表示する

---
```c
void LogInt(int dat)
```
### 引数
**dat**:表示したい整数値
### 動作
整数値を画面に表示する

## Menu.h
```c
void NormalMenu(NameFunc* MenuList,int cnt)
```
### 引数
**MenuList**:メニュー項目を表すNameFunc構造体**配列**へのポインタ

**cnt**:メニュー項目数
### 動作
メニューを起動する

---
```c
void SetMenu(SetFunc* MenuList,int cnt);
```
### 引数
**MenuList**:メニュー項目を表すSetFunc構造体**配列**へのポインタ

**cnt**:メニュー項目数
### 動作
変数変更用メニューを起動する

## monoatume_cfg.h
---
```c
void InitTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
初期化・MainTskの起動を行う

---
```c
void MainTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
メインメニューの起動を行う

---
```c
void FuncTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
メニューで指定された関数を起動する

---
```c
void QuitTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
キャンセルボタンが押されたらFuncTskを強制終了し、
移動を止め、アームを下げ、ログを削除し、メインメニューを起動する

---
```c
void TimerTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
残り時間タイマーを更新する

---
```c
void CheckTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
移動距離を計測し、CheckLengthで指定された距離だけ移動したら
イベントフラグに移動終了フラグを立てる

---
```c
void TimeoutTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
起動された時間からの経過時間を計測し、SetTimeoutで指定された時間だけ
経過したらイベントフラグに時間切れフラグを立てる

---
```c
void DispTsk(VP_INT exinf)
```
### 引数
**exinf**:タスクに渡す拡張情報。μITRON仕様で定義されている
### 動作
ディスプレイのヘッダー・フッターを描画し、display_updateを行う。
他の部分でディスプレイ制御を行いたい際はdisplay_updateを呼ばなくとも、
このタスクが勝手に呼んでくれるが、明示的に呼ぶ方がこのタスクの仕様変更時に
困らずにすむと思う

## Task.h
---
```c
void SetTimeOut(int time)
```
### 引数
**time**:指定する残り時間
### 動作
残り時間を指定してTimeoutTskを起動する。