# Memify | Memory Manipulation

Simple (Shitty) Memory Class ✔

Fully Pastable 🖨

Incorporates Handle Hijacking 🦺

Comes with some utility 🛠

## Requirements
Uses Multibyte & C++20

## Usage
```cpp

#include "mem/memory.h"

int main()
{
	memify mem("cs2.exe");

	while (!mem.ProcessIsOpen("cs2.exe")) {
		std::cout << "Couldn't find CS2, waiting..." << std::endl;
		Sleep(2000);
	}

	uintptr_t base = mem.GetBase("client.dll");

	while (true)
	{
		if (mem.InForeground())
			continue;

		uintptr_t offset1 = mem.Read<uintptr_t>(base + 0x6969);

		if (!offset1)
			continue;

		std::string str;
		uintptr_t straddr = mem.Read<uintptr_t>(base + 0x69420);

		if (straddr) // check since we don't want our string being random shit
		{
			char buf[256];
			mem.ReadRaw(straddr, buf, sizeof(buf));
			str = buf;
		}

		mem.Write<uintptr_t>(base + 0x1337, 5); // address, then the value u want to change it to
		// keep in mind it patches the value / changes it, doesn't add on to it.
	}
	//exit
}
