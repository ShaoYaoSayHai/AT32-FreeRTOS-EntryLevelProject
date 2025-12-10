#include "version_info.h"
#include "stdio.h"

const char g_compile_date[] = __DATE__;
const char g_compile_time[] = __TIME__;

char VERSION_INFO[] = "V106SP03";

void get_compile_time(char *compile_time) {
    sprintf(compile_time, "%s %s", g_compile_date, g_compile_time);
}