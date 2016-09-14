#--------------------- Basic Settings -----------------------------------------#
PROGRAM_NAME  := hyperxsearch
BINARY_BASE   := bin
BUILD_BASE    := bld
SOURCE_BASE   := src
MAIN_FILE     := src/search.cc

#--------------------- External Libraries -------------------------------------#
HEADER_DIRS   := \
	../libprim/inc \
	../libtopos/inc \
	../libstrop/inc \
	../libgrid/inc

STATIC_LIBS   := \
	../libprim/bld/libprim.a \
	../libtopos/bld/libtopos.a \
	../libstrop/bld/libstrop.a \
	../libgrid/bld/libgrid.a

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
CXX_FLAGS     += -march=native -g -O3 -flto
LINK_FLAGS    :=

#--------------------- Auto Makefile ------------------------------------------#
include $(HOME)/.makeccpp/auto_bin.mk
