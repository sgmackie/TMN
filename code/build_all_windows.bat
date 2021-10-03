@echo off

REM Build engine and tests
call %~dp0\engine\build_engine_windows.bat

REM Run test suite
if %ERRORLEVEL% == 0 (
    %dp0\..\build\unit_tester.exe
)

REM Build project
if %ERRORLEVEL% == 1 (
    call %~dp0\raytracer\build_raytracer_windows.bat
) else (
    echo Error:
    echo Unit Tests failed!
)
