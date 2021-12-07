@echo off
set ProjectPath=%CD%
set BuildPath=%ProjectPath%\build
set NinjaPath=%BuildPath%\Ninja
set VS2019Path=%BuildPath%\VS2019

REM Ninja - Clang
if not exist %NinjaPath% mkdir %NinjaPath%
pushd %NinjaPath%

set CC=clang.exe
set CXX=clang.exe
cmake %ProjectPath% -G "Ninja Multi-Config"
popd

REM VS2019 - MSVC
if not exist %VS2019Path% mkdir %VS2019Path%
pushd %VS2019Path%

set CC=cl.exe
set CXX=cl.exe
cmake %ProjectPath% -G "Visual Studio 16 2019"
popd