# Memify | Memory 

Shitty Memory Class ✔

Fully Pastable 🖨

Incorporates Handle Hijacking 🦺

Comes with some utility 🛠

## Requirements
Uses Multibyte + C++20

## Usage
```cpp

#include "mem/memory.h"

int main()
{
	memify mem("cs2.exe");
	uintptr_t base = mem.GetBase();

	while (true)
	{
		if (mem.ProcessIsOpen())
			continue;

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

		mem.Write<uintptr_t>(base + 0x1337);
	}
	//exit
	mem.~memify();
}
