@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 
call %~dp0/Tools/TMNEBuild/Bin/TMNEBuild.exe %1 %2 %3