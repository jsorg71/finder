@echo off
rem this is for 32 bit
set PATH=D:\WinDDK\7600.16385.1\bin\x86;D:\WinDDK\7600.16385.1\bin\x86\x86;c:\windows;c:\windows\system32
set FINDER_LDFLAGS=/libpath:D:\WinDDK\7600.16385.1\lib\Crt\i386 /libpath:D:\WinDDK\7600.16385.1\lib\win7\i386

rem this is for 64 bit
rem set PATH=D:\WinDDK\7600.16385.1\bin\x86;D:\WinDDK\7600.16385.1\bin\x86\amd64;c:\windows;c:\windows\system32
rem set FINDER_LDFLAGS=/libpath:D:\WinDDK\7600.16385.1\lib\Crt\amd64 /libpath:D:\WinDDK\7600.16385.1\lib\win7\amd64

set FINDER_CFLAGS=/ID:\WinDDK\7600.16385.1\inc\crt /ID:\WinDDK\7600.16385.1\inc\api
set FINDER_LIBS=user32.lib advapi32.lib shell32.lib ole32.lib gdi32.lib comctl32.lib

cmd
