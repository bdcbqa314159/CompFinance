# Some Notes

- cmake integration to allow cross platform deployment.
- msvc: similar results, the xll is produced returns good results with xlsx spreadsheets. The sln file si produced as well and is independent from the repository source code.
- gcc: on it...

- under windows is a bit tricky to work straight forward with cmake - the best is execute commands from the developper terminal of vs.

Some useful commands, assuming we are in the root of the project:

1. mkdir build
2. cd build && cmake -G "Visual Studio 17 2022" -A x64
3. cmake --build . --config Release
4. cmake --build . --config Debug
5. cmake --install . -> this needs a pre setup if you are not admin, you could hit some errors.
