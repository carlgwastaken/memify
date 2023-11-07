#pragma once
// literally all of these are in handle_hijack.h, so feel free to skip out on including them here and just handle_hijack.h for includes
// but for simplicity i will include them here too.
#include <Windows.h> // RPM + WPM
#include <TlHelp32.h>
#include <string> // couldn't get processName.compare to work with char, probably some other method but :shrug:

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

	uintptr_t GetProcessId(std::string processName = "cs2.exe")
	{
		PROCESSENTRY32 pe; // Processentry holds processID.
		
		auto ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processID);

		uintptr_t result = 0;

		if (ss != INVALID_HANDLE_VALUE)
		{
			while (Process32Next(ss, &pe))
			{
				if (!processName.compare(pe.szExeFile))
				{
					result = pe.th32ProcessID;
					break;
				}
			}
		}

		// we have processID.
		if (ss) // snapshot handle
			CloseHandle(ss); // close it since we have no use for it anymore.

		if (result != 0) // checks if we have a valid result
			return result; // if we do, return processID
		else
			return false; // add some more debug here if you want
	}

	// make BaseModule private since i'd rather shorthen name in public, and just return this function but thats your choice
	// move it to public if you want to decrease lines
	uintptr_t GetBaseModule(std::string moduleName) // default is defined later on
	{
		MODULEENTRY32 pe;

		auto ss = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID); // holds modules in all process

		uintptr_t result = 0;

		if (ss != INVALID_HANDLE_VALUE)
		{
			while (Module32Next(ss, &pe))
			{
				if (!moduleName.compare(pe.szModule))
				{
					// modbaseaddr is a BYTE, so use reinterpret_cast
					// i tried using BYTE *, but i'm slow and forgot to change the name of the shortened version in public
					// so it should work using BYTE *, but don't quote me on that 
					result = reinterpret_cast<uintptr_t>(pe.modBaseAddr);
					break;
				}
			}
		}
			
		if (ss)
			CloseHandle(ss);

		if (result != 0)
			return result;
		else
			return false;
	}

	pNtReadVirtualMemory VRead; // define
	pNtWriteVirtualMemory VWrite;

public:
	
	// constructor opens handle and you save one line!!!! (will make your spaghetti code 10x better)
	memify(std::string processName)
	{
		VRead = (pNtReadVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
		VWrite = (pNtWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");

		processID = GetProcessId("cs2.exe"); // default is "cs2.exe", so we can leave this empty if we want

		if (processID)
		{
			PROCESSENTRY32 pe;

			auto ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processID);

			if (ss != INVALID_HANDLE_VALUE)
			{
				while (Process32Next(ss, &pe))
				{
					if (!processName.compare(pe.szExeFile))
					{
						handle = hj::HijackExistingHandle(processID);
						break;

						// i don't recommend falling back to OpenProcess, since that is flagged(?)
						//if (!hj::IsHandleValid(handle))
						//{
						//	OpenProcess(PROCESS_ALL_ACCESS, false, processID);
						//}
					}
				}
			}

			if (ss)
				CloseHandle(ss);
		}
	}

	// deconstructor, you can just use a void Exit(), which is less to type but whatever
	// an actual exit function is only useful if you have a really long name.
	~memify()
	{
		if (handle)
			CloseHandle(handle);

		if (hj::HijackedHandle)
			CloseHandle(hj::HijackedHandle);
	}

	// shorten name here
	uintptr_t GetBase(std::string moduleName = "client.dll")
	{
		return GetBaseModule(moduleName);
	}

	// read
	template <typename T> // use types which are defined later on, so it's compatible with alot of shit.
	T Read(T address)
	{
		T buffer { };
		VRead(handle, (void*)address, &buffer, sizeof(T), 0);
		return buffer;
	}

	template <typename T>
	T Write(T address)
	{
		T buffer { };
		VWrite(handle, (void*)address, &buffer, sizeof(T), 0;
	}

	// for reading structs and strings and shit
	bool ReadRaw(uintptr_t address, void* buffer, size_t size)
	{
		SIZE_T bytesRead;
		if (VRead(handle, (void*)address, &buffer, static_cast<ULONG>(size), (PULONG)&bytesRead))
			return bytesRead == size;
		else
			return false;
	}

	// utilities now, nothing requred
	bool ProcessIsOpen(std::string processName = "cs2.exe")
	{
		if (GetProcessId(processName) != 0)
			return true;
		else
			return false;
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
		else
			return false;
	}

};