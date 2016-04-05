#--------------------- Basic Settings -----------------------------------------#
PROGRAM_NAME  := supersim
BINARY_BASE   := bin
BUILD_BASE    := bld
SOURCE_BASE   := src
MAIN_FILE     := src/main.cc

#--------------------- External Libraries -------------------------------------#
HEADER_DIRS   := \
	../libprim/inc \
	../libstrop/inc \
	../libjson/inc
STATIC_LIBS   := \
	../libprim/bld/libprim.a \
	../libstrop/bld/libstrop.a \
	../libjson/bld/libjson.a

#--------------------- Cpp Lint -----------------------------------------------#
LINT          := $(HOME)/.makeccpp/cpplint/cpplint.py
LINT_FLAGS    :=

#--------------------- Unit Tests ---------------------------------------------#
TEST_SUFFIX   := _TEST
GTEST_BASE    := $(HOME)/.makeccpp/gtest

#--------------------- Compilation and Linking --------------------------------#
CXX           := g++
SRC_EXTS      := .cc
HDR_EXTS      := .h .tcc
CXX_FLAGS     := -Wall -Wextra -pedantic -Wfatal-errors -std=c++11
CXX_FLAGS     += -Wno-unused-parameter -Wno-variadic-macros
CXX_FLAGS     += -march=native -g -O3 -flto
LINK_FLAGS    := -lz

#--------------------- Auto Makefile ------------------------------------------#
include $(HOME)/.makeccpp/auto_bin.mk
