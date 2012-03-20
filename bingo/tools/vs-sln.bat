@echo off
rem Bingo project config tool.
rem Author: Xu Cheng
rem Copyright (C) 2011-2012, this tool is under the license of GPLv3.
rem More detial see ./license/license.txt
cls
echo Generate Visual Studio Solution...
echo.
echo Generate lib.vcxproj
cd src\lib
qmake "CONFIG-=debug_and_release CONFIG-=build_all" lib.pro -tp vc -o lib.vcxproj
cd %~dp0\..
echo Generate libdemo.vcxproj
cd src\libdemo
qmake "CONFIG-=release" libdemo.pro -tp vc -o libdemo.vcxproj
cd %~dp0\..
echo Generate server.vcxproj
cd src\server
qmake "CONFIG-=release" server.pro -tp vc -o server.vcxproj
cd %~dp0\..
echo Generate app.vcxproj
cd src\app
qmake "CONFIG-=release" app.pro -tp vc -o app.vcxproj
cd %~dp0\..
echo Generate bingo.sln
del /f /q bingo.sln >nul 2>nul
"%~dp0\slncreator.exe" -o "%~dp0\../bingo.sln" -proj "%~dp0\../src/lib/lib.vcxproj" "%~dp0\../src/libdemo/libdemo.vcxproj" "%~dp0\../src/server/server.vcxproj" "%~dp0\../src/app/app.vcxproj"
echo Visual Studio Solution Generate Finish.
pause
echo.
