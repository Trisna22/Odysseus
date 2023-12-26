@ECHO OFF

@REM clang src/main.cpp -o bin/antidebug.exe
cl.exe  /nologo /Tcsrc/main.cpp /link /OUT:bin/antidebug.exe /SUBSYSTEM:CONSOLE /MACHINE:x64



