#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#include <Windows.h>
#include <Psapi.h>

#include <stdio.h>

void si_getUsedMemory(char * buf, unsigned int buf_size);

#endif