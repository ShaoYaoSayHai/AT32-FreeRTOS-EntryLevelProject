#ifndef __VERSION_INFO_H__
#define __VERSION_INFO_H__


#define HARDWARE_VERSION               "V1.0.0"

extern char VERSION_INFO[];

extern const char g_compile_date[];
extern const char g_compile_time[];

void get_compile_time(char *compile_time);
#endif // __VERSION_INFO_H__