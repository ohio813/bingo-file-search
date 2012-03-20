@echo off
rem Bingo project config tool.
rem Author: Xu Cheng
rem Copyright (C) 2011-2012, this tool is under the license of GPLv3.
rem More detial see ./license/license.txt

rem For first use, please modify the code below.
set QT_HOME=[your QT Home path, for example, C:\QtSDK\QtSources\4.7.4]

title Bingo Project Config Tool[Power by Xu Cheng]
if Not Exist %QT_HOME%/bin/qmake.exe (
cls
echo. Wrong QT_HOME path, please open this scrpit by notepad, 
echo. or any editor you like. And modify the code below.
echo.
echo.   set QT_HOME=
echo.
echo. It should be your QT Home path, for example, C:\QtSDK\QtSources\4.7.4
echo. You must modify it, if you run this script for first time.
echo.
pause
exit
)

set PATH=%QT_HOME%/bin;%PATH%

:license
cls
echo.
echo License Agreement:
echo.
FOR /F "tokens=* delims= " %%i IN (%~dp0/license/license.txt) DO echo. %%i
echo.
echo If you accept, type 'a'.
echo To see GPLv3 license, type 'g'; to see LGPLv3 type 'l'.
SET /P choice= You choice:
echo.
if /I "%choice%"=="a" goto accept
if /I "%choice%"=="g" goto gpl-license
if /I "%choice%"=="l" goto lgpl-license
goto license

:gpl-license
FOR /F "tokens=* delims= " %%i IN (%~dp0/license/gpl-license.txt) DO echo. %%i
pause
goto license

:lgpl-license
FOR /F "tokens=* delims= " %%i IN (%~dp0/license/lgpl-license.txt) DO echo. %%i
pause
goto license

:accept
echo Choose the options below:
echo. 1. Generate Visual Studio solution.
echo. 2. Generate MAKE file and build all.[You need run this script under Visual Studio Command Prompt]
echo. 3. Do both 1 and 2.
SET /P choice= You choice:
echo.
if /I "%choice%"=="1" goto vs-sln
if /I "%choice%"=="2" goto make-file
if /I "%choice%"=="3" goto both
goto accept

:vs-sln
%~dp0/tools/vs-sln.bat
exit

:make-file
%~dp0/tools/make-file.bat
exit

:both
%~dp0/tools/vs-sln.bat
%~dp0/tools/make-file.bat
exit
