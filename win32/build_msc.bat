@echo off
cd ..
nmake -fMakefile_msc %1
cd win32
nmake -fMakefile_msc %1
