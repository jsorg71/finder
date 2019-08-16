@echo off

rem normally you should use your visual stdio Tools Command Prompt
rem or you can use this and build with winddk

rem winddk 32 bit
rem set DDKPATH=C:\WinDDK\7600.16385.1
rem set PATH=%DDKPATH%\bin\x86\x86;%DDKPATH%\bin\x86;c:\windows;c:\windows\system32
rem set INCLUDE=/I%DDKPATH%\inc\crt;%DDKPATH%\inc\api
rem set LIB=%DDKPATH%\lib\Crt\i386;%DDKPATH%\lib\win7\i386

rem winddk 64 bit
set DDKPATH=C:\WinDDK\7600.16385.1
set PATH=%DDKPATH%\bin\x86\amd64;%DDKPATH%\bin\x86;c:\windows;c:\windows\system32
set INCLUDE=%DDKPATH%\inc\crt;%DDKPATH%\inc\api
set LIB=%DDKPATH%\lib\Crt\amd64;%DDKPATH%\lib\win7\amd64

cmd
