#pragma once
#include <Windows.h> // RPM + WPM
#include <TlHelp32.h>
#include <Psapi.h>

#include "handle_hijack.h"

// the ReadProcessMemory is basically just a pointer to this function + some error handling, so we save some performance.
// 5% according to the post below:
// unknowncheats.me/forum/general-programming-and-reversing/230813-readprocessmemory-vs-ntreadvirtualmemory-performance-benchmark-comparison.html
typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(HANDLE handle, PVOID base, PVOID buf, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);

class memify
{
	HANDLE handle = 0;
	DWORD processID;


};