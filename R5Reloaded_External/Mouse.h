#pragma once

#include <Windows.h>
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")
#define MOUSE_PRESS 1
#define MOUSE_RELEASE 2
#define MOUSE_MOVE 3
#define MOUSE_CLICK 4

class MouseGHUB {
public:
	typedef struct {
		char button;
		char x;
		char y;
		char wheel;
		char unk1;
	} MOUSE_IO;

	HANDLE g_input;
	IO_STATUS_BLOCK g_io;
	BOOL g_found_mouse;
	BOOL callmouse(MOUSE_IO* buffer);
	NTSTATUS device_initialize(PCWSTR device_name);
	BOOL mouse_open(void);
	void mouse_close(void);
	void mouse_move(char button, char x, char y, char wheel);
};
extern MouseGHUB* mouseGHUB;