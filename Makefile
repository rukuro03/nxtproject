#
# TOPPERS/JSPを用いたライントレーサー用のMakefile
#
#	単にmakeとするとプログラムをアップロード
#	make music　で音楽の検査用プログラムmusic.exeを作成
#	make clean　で余計なファイルを削除
#		music.exeだけは残ってしまうので注意
#	make firmware　でNXTにファームウェアをアップロード

#	11/18 適当に本来jougaだったファイル名を全てmonoatumeにしたが、
#	その影響でmakeできなくなるかもしれない。多分大丈夫だけど！
#	その場合元のMakefileを回収してきて、monoatumeをjougaに直せば問題ないはず。

#	12/5 適当にファイル分割しました
#	その影響でコンパイルエラーが出るかもしれない…頑張ったけど

# Target specific macros
TARGET = monoatume
TARGET_SOURCES = \
	monoatume.c \
	music.c button.c display.c graphics.c \
	Task.c Log.c Move.c Arm.c
TOPPERS_JSP_CFG_SOURCE = ./monoatume.cfg
TARGET_HEADERS = \
	monoatume.h \
	monoatume_cfg.h music.h button.h display.h graphics.h \
	Log.h Move.h Arm.h 
FIRMWARE = c:/cygwin/nexttool/lms_arm_nbcnxc_128.rfw

# ここから先は、内容を理解してから変更してください
# Don't modify below part unless you understand them

TOPPERS_KERNEL = NXT_JSP
NXTOSEK_ROOT = /nxtOSEK
BUILD_MODE = RXE_ONLY

.PHONY: first_target
first_target	: all upload

.PHONY: upload
upload	:
	sh ./rxeflash.sh

.PHONY: firmware
firmware	:
	/nexttool/NeXTTool.exe /COM=usb -firmware=$(FIRMWARE)

O_PATH ?= build
include $(NXTOSEK_ROOT)/ecrobot/ecrobot.mak

music	: music.c
	gcc -DMUSIC_DEBUG -o music music.c -lwinmm

#### 依存関係の指定
#### Dependencies
monoatume.o	: $(TARGET_HEADERS)
button.o	: button.h
music.o		: 
kernel_cfg.c	: monoatume_cfg.h monoatume.h
display.o	: display.h
graphics.o	: graphics.h display.h
Log.o		: Log.h
Arm.o		: Arm.h
Move.o		: Move.h
Task.o		:
# 上記適当に変えました　悪い意味の適当です。
