# Memify | Memory Manipulation

Simple (Shitty) Memory Class ✔

Fully Pastable 🖨

Comes with some utility 🛠

## Requirements
C++20
Multibyte support

## Usage
Simply include memify/memify.h into your process.
```cpp
#include "memify/memify.h"
```

and pass in the process name

```cpp
memify mem("process.exe");
```

or multiple names!

```cpp
std::string processes[] = {"process1.exe", "process2.exe", "process3.exe"};
memify mem(processes);
```

and now you have an open handle to the game!
