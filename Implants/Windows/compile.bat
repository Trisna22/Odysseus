@REM Project: 	Windows implant
@REM Description:	Implant for windows hosts as part of the Odysseus framework

cl.exe  /nologo /Tcsrc/main.cpp /link /OUT:bin/implant.exe /SUBSYSTEM:CONSOLE /MACHINE:x64
