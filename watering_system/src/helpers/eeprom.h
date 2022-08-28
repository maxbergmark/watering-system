#pragma once

#include <FS.h>
#include <EEPROM.h>
#include <user_interface.h>

#define EEPROM_MIN_ADDR 0
#define EEPROM_MAX_ADDR 4096

boolean f(int addr);
boolean eeprom_is_addr_ok(int addr);
boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes);
boolean eeprom_write_string(int addr, const char* string);
boolean eeprom_read_bytes(int startAddr, byte array[], int numBytes);
boolean eeprom_read_string(int addr, char* buffer, int bufSize);
void eeprom_erase_all();