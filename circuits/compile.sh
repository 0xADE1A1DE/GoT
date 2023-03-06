#!/bin/bash

gcc and.c branchless_fan.c correct.c dmux.c fan.c full_adder.c \
half_adder.c main.c majority.c mm.c mux.c nand.c NBitAdder.c nor.c \
not_2_plus_from_8.c not_majority_5.c not_majority.c not.c or_from_nand.c \
or_from_nor_not.c or.c util.c xor.c \
gol_gate_fixed.c \
gol_game.c \
alu.c \
gol_glider.c \
_32BitAdder.c \
sha1.c \
pht_nuke.S \
-o out -w -g -O1 -fno-ipa-icf  -fno-delayed-branch -fno-guess-branch-probability -falign-loops -falign-functions=65536 -falign-labels -fcaller-saves -fexpensive-optimizations -fno-if-conversion2 -fno-if-conversion -fno-ipa-profile -fno-merge-constants  -fno-cse-follow-jumps  -fno-cse-skip-blocks  -fno-dse -fno-dce -fno-tree-dse -fno-ipa-sra -fno-profile-arcs -fno-gcse \
-fno-hoist-adjacent-loads \
-fomit-frame-pointer -fno-cprop-registers -fno-rename-registers \
-fno-schedule-insns -fno-schedule-insns2 \
-fno-combine-stack-adjustments \
-fno-sched-spec \
# -DGATES_STATS
#-fsanitize=address

