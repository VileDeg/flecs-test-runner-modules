# Flecs Test Runner Modules

This is sample testable modules for **Flecs Test Runner (FTR)** applications.

TODO

## Build (C++)

`CMake` must be installed in order to build.

When cmake is called, it will **fetch the depencies** from remote Git repositories, using `FetchContent`. 

```ps
cmake -S . -B build/
```

Output of the build is a **static library**.