# Makefile
SHELL:=/bin/bash


EXE=d2q9-bgk
CC=icc
CFLAGS= -std=c99 -xHOST -openmp
LIBS = -lm

MODULE_1 =module load languages/intel-compiler-15
MODULE_2 =module load languages/python-2.7.6

FINAL_STATE_FILE=./final_state.dat
AV_VELS_FILE=./av_vels.dat
REF_FINAL_STATE_FILE=check/256x256.final_state.dat
REF_AV_VELS_FILE=check/256x256.av_vels.dat

all: $(EXE)

$(EXE): 
	$(MODULE_1) && \
	$(CC) $(CFLAGS) $(EXE).c $(LIBS) -o $@

check:
	$(MODULE_2) && \
	python check/check.py --ref-av-vels-file=$(REF_AV_VELS_FILE) --ref-final-state-file=$(REF_FINAL_STATE_FILE) --av-vels-file=$(AV_VELS_FILE) --final-state-file=$(FINAL_STATE_FILE)

.PHONY: all check clean

clean:
	rm -f $(EXE)

