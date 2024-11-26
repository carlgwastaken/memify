#include "mem/memify.h"

int main()
{
	// might want to make sure the game is open first.
	memify mem("cs2.exe");
	uintptr_t base = mem.GetBase("client.dll");

	while (true)
	{
		// check if our window is in the foreground before reading any memory, useful for ESPs
		if (!mem.InForeground("Counter-Strike 2"))
			continue;

		// read our first offset
		uintptr_t offset1 = mem.Read<uintptr_t>(base + 0x6969);

		// check if the new address if valid, if not then we just continue.
		if (!offset1)
			continue;

		// next, we read a ptr to a string address.
		uintptr_t straddr = mem.Read<uintptr_t>(base + 0x69420);
		
		// create the buffer the output will go to.
		char buf[256];

		// if our string address is valid, we read the raw memory. Since this is a string we don't want it to be some random shit.
		if (straddr) {
			mem.ReadRaw(straddr, buf, sizeof(buf));
		}

		// and we write a value to yet another random address.
		mem.Write<uintptr_t>(base + 0x1337, 5);
	}
	// deconstructor called automatically upon exit, no cleanup needed.
}