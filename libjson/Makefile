#--------------------- Basic Settings -----------------------------------------#
PROGRAM_NAME  := json
BINARY_BASE   := bin
BUILD_BASE    := bld
INCLUDE_BASE  := inc
SOURCE_BASE   := src

#--------------------- External Libraries -------------------------------------#
HEADER_DIRS   :=
STATIC_LIBS   :=

#--------------------- Cpp Lint -----------------------------------------------#
LINT          :=
LINT_FLAGS    :=

#--------------------- Unit Tests ---------------------------------------------#
TEST_SUFFIX   := _TEST
GTEST_BASE    :=

#--------------------- Compilation and Linking --------------------------------#
CXX           := g++
AR            := gcc-ar
SRC_EXTS      := .cc
HDR_EXTS      := .h .tcc
CXX_FLAGS     := -Wall -Wextra -pedantic -Wfatal-errors -std=c++11
CXX_FLAGS     += -march=native -g -O3 -flto
LINK_FLAGS    :=

#--------------------- Auto Makefile ------------------------------------------#
include $(HOME)/.makeccpp/auto_lib.mk
