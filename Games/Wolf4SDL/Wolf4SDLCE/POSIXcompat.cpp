/* IO functions reimplementation (WinCE are not POSIX-compatible) */
#include "../Adopt.h"
#include <Windows.h>

/* TODO: This is POSIX-hack to quickly get working prototype on WinCE. Port later to stdlib */
int read(FILE* f, void* buf, int len)
{
	return fread(buf, len, 1, f);	
}

int write(FILE* f, void* buf, int len)
{
	return fwrite(buf, len, 1, f);
}

long lseek(FILE* f, long pos, int origin)
{
	fseek(f, pos, origin);

	return ftell(f);
}

int mkdir(char* dirName)
{
	wchar_t buf[256];

	mbstowcs((wchar_t*)&buf, dirName, 256);
	return CreateDirectoryW((LPCWSTR)&buf, 0) == TRUE;
}

void unlink(char* file)
{
	wchar_t buf[256];

	mbstowcs((wchar_t*)&buf, file, 256);
	DeleteFileW((LPCWSTR)&buf);
}

int exists(char* fileName)
{
	int ret = 0;
	FILE* f = fopen(fileName, "r");
	ret = f != 0;

	if(ret)
		fclose(f);

	return ret;
}