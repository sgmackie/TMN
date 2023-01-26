@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 

REM Raytracer
call "Source/Raytracer/BuildRaytracerWindows.bat" %1