
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void printi(int);
void printd(double);
#ifndef _STDIO_H_
void printf(float);
#endif
void printc(char);
void prints(const char *);
void printptr(const void *);

#ifdef __cplusplus
}
#endif
