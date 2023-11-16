#include "mem/memory.h"

int main()
{
	memify mem("cs2.exe");
	uintptr_t base = mem.GetBase("client.dll");

	while (true)
	{
		if (mem.ProcessIsOpen("cs2.exe"))
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

		mem.Write<uintptr_t>(base + 0x1337, 5); // address, then the value u want to change it to
		// keep in mind it patches the value / changes it, doesn't add on to it.
	}
	//exit
	mem.~memify();
}
