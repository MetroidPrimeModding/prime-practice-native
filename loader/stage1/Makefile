PREFIX=powerpc-eabi-
CC=${PREFIX}gcc
CXX=${PREFIX}g++
OBJCOPY=${PREFIX}objcopy
OBJDUMP=${PREFIX}objdump
AS=${PREFIX}as
ld=${PREFIX}ld
DEBUG=-g
CCFLAGS=-ffreestanding -nostdlib -O2 -I$(DEVKITPRO)/libogc/include -L$(DEVKITPRO)/libogc/lib/cube -DGEKKO -mogc -mcpu=750 -meabi -mhard-float -logc --std=gnu99 ${DEBUG}
LIBS=-lgcc
BIN=bin
SRC=src

SRC_S_FILES=$(wildcard *.s)
SRC_C_FILES=$(wildcard *.c)
SRC_O_FILES=$(patsubst %.s,%.o,$(SRC_S_FILES)) $(patsubst %.c,%.o,$(SRC_C_FILES))
BIN_O_FILES=$(patsubst %,bin/%,$(SRC_O_FILES))

all: ${BIN} ${BIN}/stage1.img decompile

decompile: ${BIN}/stage1.elf
	${OBJDUMP} -d bin/stage1.elf > bin/stage1.elf.s

${BIN}:
	mkdir $@

${BIN}/%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $<

${BIN}/%.o: %.s
	${CC} ${CCFLAGS} -c -o $@ $<

${BIN}/stage1.elf: ${BIN_O_FILES}
	${CC} ${CCFLAGS} ${LIBS} -T stage1.ld -Xlinker -Map=${BIN}/stage1.map -o $@ ${BIN_O_FILES}

${BIN}/stage1.bin: ${BIN}/stage1.elf
	${OBJCOPY} ${BIN}/stage1.elf -O binary ${BIN}/stage1.bin

${BIN}/stage1.img: ${BIN}/stage1.bin
	cp -v $< $@

.PHONY: clean

clean:
	rm -rf ${BIN}