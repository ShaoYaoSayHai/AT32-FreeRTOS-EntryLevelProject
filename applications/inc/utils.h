#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdint.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#include "rtc.h"

char* trim(char *str);
char* trimQuotes(char *str);
void ByteToHexStr(const uint8_t* source, char* dest, int sourceLen);
void HexStrToByte(const char* source, uint8_t* dest, int sourceLen);
uint8_t GetCrcNumWithInit(uint8_t Init, uint8_t *point, uint32_t len);
int8_t parse_utc_time(const char* time_str, calendar_type* rtc_time);
uint16_t Gb698Fcs16(uint16_t fcs, uint8_t *cp, int len);
char *strnchr(const char *s, char c, uint32_t n);
uint8_t calculate_simple_xor(const uint8_t *data, uint32_t data_len);
#endif