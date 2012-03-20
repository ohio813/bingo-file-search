@echo off
rem Bingo project config tool.
rem Author: Xu Cheng
rem Copyright (C) 2011-2012, this tool is under the license of GPLv3.
rem More detial see ./license/license.txt
cls
echo Generate MAKE file...
echo.
cd %~dp0\..
qmake %~dp0\../bingo.pro
echo. Build All...
nmake
nmake clean
echo Generate MAKE file and build all Finish.
pause
echo.