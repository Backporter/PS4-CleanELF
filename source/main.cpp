#include <stdint.h>
#include <stdarg.h>

#include "../include/types.h"
#include "../include/imports.h"
#include "../include/syscall.h"
#include "../include/detour.h"
#include "../include/utility.h"
#include "../include/Notify.h"

void (*_Start)();

bool exists(const char* FileToFind) {
	int file = open(FileToFind, 0, 0);
	if (file >= 0) {
		close(file);
		return true;
	}
	else {
		return false;
	}
}

void Start() {
	// iterate trhough and get all PRX's or just use one... does not really matter
	if (exists("/app0/prx.prx"))
	{
		Notify("[ELF] PRX Found!... Loading PRX!");
	    SceKernelModule PRXHandle = sceKernelLoadStartModule("/app0/prx.prx", 0, 0, 0, 0, 0);
		if (!PRXHandle) {
			Notify("Failed To Load PRX!");
			return;
		}
		sys_dynlib_dlsym(PRXHandle, "_Start", &_Start);
		_Start();
	}
}

extern "C" void _main(void) {
	initImports();
	Start();
	return;
}
