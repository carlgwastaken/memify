#pragma once
// literally all of these are in handle_hijack.h, so feel free to skip out on including them here and just handle_hijack.h for includes
// but for simplicity i will include them here too. *except for psapi.h*
#include <Windows.h> // RPM + WPM
#include <TlHelp32.h> 
#include <string> // couldn't get processName.compare to work with char, probably some other method but :shrug:
#include <Psapi.h> // enumprocessmodules

#include "handle_hijack.h"

/*
Created By https://github.com/carlgwastaken/
Please Support Open Source and leave this message here if you're using in your own source
Besides that, enjoy!
*/

// the ReadProcessMemory is basically just a pointer to this function + some error handling, so we save some performance.
// 5% according to the post below:
// unknowncheats.me/forum/general-programming-and-reversing/230813-readprocessmemory-vs-ntreadvirtualmemory-performance-benchmark-comparison.html
typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
typedef NTSTATUS(WINAPI* pNtWriteVirtualMemory)(HANDLE Processhandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);

class memify
{
private:
	// initalize at 0 so we can check later
	HANDLE handle = 0;
	DWORD processID = 0;

	pNtReadVirtualMemory VRead; // define Virtual Read + Virtual Write
	pNtWriteVirtualMemory VWrite;

	uintptr_t GetProcessId(std::string_view processName)
	{
		DWORD ids[1024];
		DWORD neededId;

		if (EnumProcesses(ids, sizeof(ids), &neededId)) 
		{
			int processCount = neededId / sizeof(DWORD);

			for (int i = 0; i < processCount; ++i)
			{
				if (handle != 0)
				{
					char moduleName[MAX_PATH];
					if (GetModuleBaseNameA(handle, nullptr, moduleName, sizeof(moduleName)))
					{
						if (!processName.compare(moduleName)) {
							return ids[i];
						}
					}
				}
			}
		}

		return 0;
	}

	// make BaseModule private since i'd rather shorthen name in public, and just return this function but thats your choice
	// move it to public if you want to decrease lines
	uintptr_t GetBaseModule(std::string_view moduleName)
	{
		HMODULE modules[1024];
		DWORD neededmodule; 

		if (EnumProcessModules(handle, modules, sizeof(modules), &neededmodule))
		{
			int moduleCount = neededmodule / sizeof(HMODULE);

			for (int i = 0; i < moduleCount; ++i)
			{
				char buffer[MAX_PATH];

				if (GetModuleBaseNameA(handle, modules[i], buffer, sizeof(buffer)))
				{
					if (!moduleName.compare(buffer)) {
						return reinterpret_cast<uintptr_t>(modules[i]);
					}
				}
			}
		}

		return 0;
	}
public:

	// constructor opens handle and you save one line!!!! (will make your spaghetti code 10x better)
	memify(std::string_view processName)
	{
		VRead = (pNtReadVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
		VWrite = (pNtWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");

		processID = GetProcessId(processName);

		if (processID != 0)
		{
			handle = hj::HijackExistingHandle(processID);

			if (!hj::IsHandleValid(handle))
			{
				std::cout << "Failed Handle Hijacking." << std::endl;
				// open handle with OpenProcess
			}
		}
	}

	// deconstructor, you can just use a void Exit(), which is less to type but whatever
	~memify()
	{
		if (handle)
			CloseHandle(handle);

		if (hj::HijackedHandle)
			CloseHandle(hj::HijackedHandle);
	}

	// shorten name here
	uintptr_t GetBase(std::string_view moduleName)
	{
		return GetBaseModule(moduleName);
	}

	// read
	template <typename T> // use types which are defined later on, so it's compatible with alot of shit.
	T Read(uintptr_t address)
	{
		T buffer{ };
		VRead(handle, (void*)address, &buffer, sizeof(T), 0);
		return buffer;
	}

	template <typename T>
	T Write(uintptr_t address, T value)
	{
		VWrite(handle, (void*)address, &value, sizeof(T), NULL);
		return value;
	}

	// for reading structs and strings and shit
	bool ReadRaw(uintptr_t address, void* buffer, size_t size)
	{
		SIZE_T bytesRead;
		if (VRead(handle, (void*)address, &buffer, static_cast<ULONG>(size), (PULONG)&bytesRead))
			return bytesRead == size;

		return false;
	}

	// utilities, shit that isn't required but nice to have

	bool ProcessIsOpen(const std::string_view processName)
	{
		return GetProcessId(processName) != 0;
	}

	bool InForeground()
	{
		// just takes Counter-Strike 2 but change to whatever u want, or implement an input you can do that too
		// maybe get the foreground window and then compare it to your own window, use processID, anything u want
		HWND current = GetForegroundWindow();

		char title[256];
		GetWindowText(current, title, sizeof(title));

		if (strstr(title, "Counter-Strike 2") != nullptr)
			return true;

		return false;
	}
};
