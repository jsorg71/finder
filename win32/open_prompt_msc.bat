@echo off

rem winddk 32 bit
set DDKPATH=C:\WinDDK\7600.16385.1
set PATH=%DDKPATH%\bin\x86\x86;%DDKPATH%\bin\x86;c:\windows;c:\windows\system32
set FINDER_LDFLAGS=/libpath:%DDKPATH%\lib\Crt\i386 /libpath:%DDKPATH%\lib\win7\i386
set FINDER_CFLAGS=/I%DDKPATH%\inc\crt /I%DDKPATH%\inc\api
set FINDER_LIBS=user32.lib advapi32.lib shell32.lib ole32.lib gdi32.lib comctl32.lib

rem winddk 64 bit
rem set DDKPATH=C:\WinDDK\7600.16385.1
rem set PATH=%DDKPATH%\bin\x86\amd64;%DDKPATH%\bin\x86;c:\windows;c:\windows\system32
rem set FINDER_LDFLAGS=/libpath:%DDKPATH%\lib\Crt\amd64 /libpath:%DDKPATH%\lib\win7\amd64
rem set FINDER_CFLAGS=/I%DDKPATH%\inc\crt /I%DDKPATH%\inc\api
rem set FINDER_LIBS=user32.lib advapi32.lib shell32.lib ole32.lib gdi32.lib comctl32.lib

rem vc2010 32 bit
rem set VCPATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0
rem set SDKPATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A
rem set PATH=%VCPATH%\VC\bin;%VCPATH%\Common7\IDE;c:\windows;c:\windows\system32
rem set FINDER_LDFLAGS=/libpath:"%VCPATH%\VC\lib" /libpath:"%SDKPATH%\Lib"
rem set FINDER_CFLAGS=/I"%VCPATH%\VC\include" /I"%SDKPATH%\Include"
rem set FINDER_LIBS=user32.lib advapi32.lib shell32.lib ole32.lib gdi32.lib comctl32.lib

rem vc2010 64 bit
rem set VCPATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0
rem set SDKPATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A
rem set PATH=%VCPATH%\VC\bin\x86_amd64;%VCPATH%\VC\bin;%VCPATH%\Common7\IDE;c:\windows;c:\windows\system32
rem set FINDER_LDFLAGS=/libpath:"%VCPATH%\VC\lib\amd64" /libpath:"%SDKPATH%\Lib\x64"
rem set FINDER_CFLAGS=/I"%VCPATH%\VC\include" /I"%SDKPATH%\Include"
rem set FINDER_LIBS=user32.lib advapi32.lib shell32.lib ole32.lib gdi32.lib comctl32.lib

cmd
