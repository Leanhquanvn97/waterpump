# MPLAB IDE generated this makefile for use with GNU make.
# Project: code.mcp
# Date: Tue May 26 06:27:22 2020

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

code.cof : code.o
	$(LD) /l"C:\MCC18\lib" "..\..\..\..\..\MCC18\lkr\18f4520.lkr" "code.o" /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /o"code.cof" /M"code.map" /W

code.o : code.c ../../../../../MCC18/h/delays.h ../../../../../MCC18/h/math.h ../../../../../MCC18/h/usart.h ../../../../../MCC18/h/stdio.h ../../../../../MCC18/h/i2c.h code.c ../../../../../MCC18/h/P18f4520.h ../../../../../MCC18/h/p18cxxx.h ../../../../../MCC18/h/p18f4520.h ../../../../../MCC18/h/stdarg.h ../../../../../MCC18/h/stddef.h
	$(CC) -p=18F4520 "code.c" -fo="code.o" -D__DEBUG -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "code.o" "code.cof" "code.hex"

