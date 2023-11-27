#include "Mouse.h"

MouseGHUB* mouseGHUB = new MouseGHUB();
BOOL MouseGHUB::callmouse(MOUSE_IO* buffer){
	IO_STATUS_BLOCK block;
	return NtDeviceIoControlFile(g_input, 0, 0, 0, &block, 0x2a2010, buffer, sizeof(MOUSE_IO), 0, 0) == 0L;
}

NTSTATUS MouseGHUB::device_initialize(PCWSTR device_name){
	UNICODE_STRING name;
	OBJECT_ATTRIBUTES attr;

	RtlInitUnicodeString(&name, device_name);
	InitializeObjectAttributes(&attr, &name, 0, NULL, NULL);

	NTSTATUS status = NtCreateFile(&g_input, GENERIC_WRITE | SYNCHRONIZE, &attr, &g_io, 0,
		FILE_ATTRIBUTE_NORMAL, 0, 3, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);

	return status;
}

BOOL MouseGHUB::mouse_open(void){
	NTSTATUS status = 0;


	if (g_input == 0) {

		wchar_t buffer0[] = L"\\??\\ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015}";

		status = device_initialize(buffer0);
		if (NT_SUCCESS(status))
			g_found_mouse = 1;
		else {
			wchar_t buffer1[] = L"\\??\\ROOT#SYSTEM#0001#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
			status = device_initialize(buffer1);
			if (NT_SUCCESS(status))
				g_found_mouse = 1;
		}
	}
	return status == 0;
}

void MouseGHUB::mouse_close(void){
	if (g_input != 0) {
		CloseHandle(g_input);
		g_input = 0;
	}
}

void MouseGHUB::mouse_move(char button, char x, char y, char wheel){
	MOUSE_IO io;
	io.unk1 = 0;
	io.button = button;
	io.x = x;
	io.y = y;
	io.wheel = wheel;

	if (!callmouse(&io)) {
		mouse_close();
		mouse_open();
	}
}