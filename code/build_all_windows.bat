@echo off

REM TODO Get the path of this bat file and not the calling path 
call %CD%\code\engine\build_engine_windows.bat
call %CD%\code\raytracer\build_raytracer_windows.bat