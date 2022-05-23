include clear-config.mk
COMPILE_TARGET:=lib
CFLAGS:=-O0
SRCS:=fb_common.c
DEP_INCS+=fb/include
include add-config.mk
