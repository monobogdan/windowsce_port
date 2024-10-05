#include <stdlib.h>
#include <stdio.h>

#include <Windows.h>
#include "gximpl.h"

#include "Waves'NES.h"

HWND hwnd = 0;
HDC dc = 0;

int WinMain(HINSTANCE inst, HINSTANCE hPrevInst, LPWSTR cmdLine, int showCmd)
{
	BITMAPINFO info;

	RUN_WaveNES();

	hwnd = CreateWindowW(L"static", L"Emulator", WS_VISIBLE | WS_SYSMENU, 0, 0, 240, 340, 0, 0, 0, 0);
	dc = GetDC(hwnd);
	
	memset(&info, 0, sizeof(info));
	info.bmiHeader.biBitCount = 16;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biHeight = -320;
	info.bmiHeader.biWidth = 240;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSize = sizeof(info);

	while(IsWindow(hwnd))
	{
		NesTimer(2);
		SetDIBitsToDevice(dc, 0, 0, info.bmiHeader.biWidth, -info.bmiHeader.biHeight, 0, 0, 0, -info.bmiHeader.biHeight, LCD, &info, DIB_RGB_COLORS);

		//Sleep(1000 / 60);
	}

	
	return 0;
}