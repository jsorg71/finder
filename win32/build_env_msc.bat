
rem set PATH=d:\borland\bcc55\bin;c:\windows;c:\windows\system32

rem set INCLUDE=D:\WinDDK\7600.16385.1\inc
rem set LIB=D:\WinDDK\7600.16385.1\lib
rem set PATH=D:\WinDDK\7600.16385.1\bin\x86;D:\WinDDK\7600.16385.1\bin\x86\x86;c:\windows;c:\windows\system32

set FINDER_CFLAGS=/O2 /W3 /I.. /ID:\WinDDK\7600.16385.1\inc\crt /ID:\WinDDK\7600.16385.1\inc\api
set FINDER_LDFLAGS=/libpath:D:\WinDDK\7600.16385.1\lib\Crt\amd64 /libpath:D:\WinDDK\7600.16385.1\lib\win7\amd64
set FINDER_LIBS=user32.lib advapi32.lib shell32.lib ole32.lib gdi32.lib comctl32.lib
set PATH=D:\WinDDK\7600.16385.1\bin\x86;D:\WinDDK\7600.16385.1\bin\x86\amd64;c:\windows;c:\windows\system32

cmd
