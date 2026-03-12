# Toolchain
CC      := avr-gcc
OBJCOPY := avr-objcopy
OBJISP  := avrdude
AVRSIZE := avr-size
PORT    := \\\\.\\COM4        # Change to your Arduino port on Windows, e.g., COM3

MCU := atmega328p

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Build flags
CFLAGS  := -Wall -Wextra -Wundef -pedantic \
           -Os -std=gnu99 -DF_CPU=16000000UL -mmcu=$(MCU) -DBAUD=9600
LDFLAGS := -mmcu=$(MCU)
SOURCES = main.c servo.c uart.c lcd.c
# Files
BIN := program.hex
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean fresh isp

# Default target
all: $(BIN_DIR)/$(BIN)

# Compile C files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -MD -o $@ -c $<

# Link ELF
$(BIN_DIR)/%.elf: $(OBJ)
	if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CC) $(LDFLAGS) -Wl,-Map=$(OBJ_DIR)/$*.map -o $@ $^
	$(AVRSIZE) $@

# Convert ELF to HEX
$(BIN_DIR)/%.hex: $(BIN_DIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

# Flash to Arduino
isp: $(BIN_DIR)/$(BIN)
	$(OBJISP) -F -V -c arduino -p $(MCU) -P $(PORT) -U flash:w:$<

# Clean object files
clean:
	if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)

# Clean everything
fresh: clean
	if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)