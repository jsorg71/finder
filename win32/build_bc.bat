@echo off
cd ..
make -fMakefile_bc %1
cd win32
make -fMakefile_bc %1
