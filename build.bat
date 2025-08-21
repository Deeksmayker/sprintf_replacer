@echo off

SET options= -Zi -EHa -DDEBUG_BUILD=1
if "%~1"=="release" (SET options=-O2 -EHsc -DRELEASE_BUILD=1)

cl %options% main.cpp /Fe:sprintf_s_replacer.exe /std:c++20

