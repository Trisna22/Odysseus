@ECHO OFF

@REM clang src/main.cpp -o bin/win32hash.exe
cl.exe  /nologo /Tcsrc/main.cpp /link /OUT:bin/win32hash.exe /SUBSYSTEM:CONSOLE /MACHINE:x64

