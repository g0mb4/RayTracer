#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>

void si_getUsedMemory(char *buf, unsigned int buf_size);

#endif