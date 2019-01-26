#include "sysinfo.h"

void si_getUsedMemory(char *buf, unsigned int buf_size) {
	PROCESS_MEMORY_COUNTERS pmc;
	memset(&pmc, 0, sizeof(pmc));

	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	unsigned int mem = pmc.PeakWorkingSetSize;

	double tb = 1099511627776;
	double gb = 1073741824;
	double mb = 1048576;
	double kb = 1024;

	if (mem >= tb)
		sprintf_s(buf, buf_size, "MEM: %.2f TB", (double)mem / tb);
	else if (mem >= gb && mem < tb)
		sprintf_s(buf, buf_size, "MEM: %.2f GB", (double)mem / gb);
	else if (mem >= mb && mem < gb)
		sprintf_s(buf, buf_size, "MEM: %.2f MB", (double)mem / mb);
	else if (mem >= kb && mem < mb)
		sprintf_s(buf, buf_size, "MEM: %.2f kB", (double)mem / kb);
	else if (mem < kb)
		sprintf_s(buf, buf_size, "MEM: %.2f B", (double)mem);
	else
		sprintf_s(buf, buf_size, "MEM: %.2f B", (double)mem);
}