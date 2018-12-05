## MEMO -用語・API逆引き時点-
### By ご飯システム班の一人
今回作成するシステムは、'Hello,World!'のようなプログラムとは全く異なる
環境でプログラミングしなければなりません。そのため、ドキュメンテーションを
読んでいても意味のわからない単語が多々存在します。ここでは私が、この単語や
関数の意味がわかっていたりすると開発が容易になると感じたものをまとめます。

### 注意
このファイルはマークダウン言語で書かれています。txtファイルとしても読めると
思いますが、マークダウンならちょっと読みやすいです。
ブラウザ上で使えるマークダウンビューアとかを使うといいと思います。
github上なら自動で綺麗に表示されるはずです。

# 開発環境編
+ **RTOS**:リアルタイムOSのこと。即時的な処理、応答が求められるシステム。
+ **API**:アプリケーションプログラミングインターフェイス。一言で言えば関数の
宣言。関数のプロトタイプ宣言とその関数が何をするかという抽象的な仕様のみを定めたもの。
+ **ITRON**:RTOSを実現するための処理をまとめたAPI。
+ **μITRON**:8-bitか16-bitのCPUを対象とするITRON(ソース未確認)。
+ **スタンダードプロファイル**:μITRONの仕様のうち、ある一定の範囲のみ実装したもの。
多少機能制限がくわわる。
+ **TOPPERS/JSP**:μITRONスタンダードプロファイルの実装。一言で言えば関数の定義。
+ **nxt**:ロボットの上にのってる機械。
+ **nxtOSEK**:TOPPERS/JSPを用いてnxtを動かすためのAPI。すでに実装もなされている。
+ **ECrobot**:これはどこをさがしても出てこないが、恐らくnxtOSEKが定義する
API自体の名前？

# μITRON編
+ **静的API**:タスクやセマフォ、イベントフラグ等の定義をするファイルを出力するための
API。これはC言語ではなく、コンパイラではないプログラム(コンフィギュレータ？)で解読される。
+ **タスク**:並列実行の単位。C++やJavaのスレッドに近い感覚。
1つの関数が1つのタスクとなる。
+ **ハンドラ**:タスク以外の、OS外からの処理。
+ **非タスクコンテキスト**:タスク以外の場所。ハンドラ等。

## μITRON・動的API編
大体これらだけで十分だと思います。
+ **act_tsk**:タスクの起動。
+ **ter_tsk**:タスクの強制終了。
+ **dly_tsk**:タスクを待ち状態にする。
+ **wai_sem**:セマフォを取得できるまで待ち、セマフォの値から1減算。
+ **sig_sem**:セマフォの値に1加算。
+ **wai_flg**:イベントフラグを待つ。ANDかORのどちらでイベント達成か選べる。
+ **set_flg**:イベントフラグをセット(引数とフラグのOR)。
+ **clr_flg**:イベントフラグをクリア(引数とフラグのAND)。

# nxtOSEK・ECrobotAPI編
+ **nxt_motor_set_speed**:モーターに与えるパワーを指定する。
回転方向が逆転しやすいよう、motor_set_speedというマクロが宣言されている。
+ **nxt_motor_set_count**:モーター回転角度をセット。普通0にする。
+ **nxt_motor_get_count**:モーター回転角度を取得。**注意**　モータを逆回転させると、
案の定回転角度は減っていきます。マイナスにもなります。
+ **display系**:結構ストレートな名前なのでわかりやすいはず。

# 自作API
+ **get_master_slave**:外側と内側のモーターのポートを取得。
+ **MoveSetPower**:モーターパワーの設定。
+ **MoveSetSteer**:モータ回転比の設定＋設定されたパワーからパワーの調整。
+ **MoveActivate**:移動用タスクの起動。
+ **MoveTerminate**:移動用タスクの終了+モータの停止。
+ **WaitForOR**:wai_flgのラッパーORバージョン。
+ **WaitForAND**:wai_flgのラッパーANDバージョン。
+ **MoveLength**:MoveSetPower→MoveSetSteer→MoveActivate→WaitForOR→
MoveTerminateを行うラッパー。
+ **MoveTurn**:機体の角度を旋回させる。MoveLengthのラッパー。

### 参考文献：
#### μITRON ver4.2.0　仕様書
#### https://osdn.net/projects/toppersjsp4bf/docs/user.pdf/ja/3/user.pdf.pdf
TOPPERS/JSPのマニュアル