@echo off

REM Working directory 
set ProjectPath=%CD%
set CodePath=%ProjectPath%\code

REM MSVC version
if "%1"=="" (
REM 2019
@REM call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
set Compiler="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29910\bin\Hostx64\x64\cl.exe"
set CompilerFlags=/std:c11
) else if "%1"=="2012" (
REM 2012 
@REM call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
set Compiler="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\cl.exe"
set CompilerFlags=/?
) else if "%1"=="2008" (
REM 2008 
@REM call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\amd64\vcvarsamd64.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\amd64\cl.exe"
set CompilerFlags=/?
)

REM Commom options
set CompilerFlags=/nologo /WX /WL /FC /Wv:18 %CompilerFlags%
set CompilerFlags=/DPLATFORM_WINDOWS=1 %CompilerFlags%

REM Debug builds
set CompilerFlags=/Od /MDd /Zi %CompilerFlags%
REM set CompilerFlags=/O2 /MD /Zo %CompilerFlags%

REM Compiler tools
REM set CompilerFlags=/fsanitize=address %CompilerFlags%

REM Build all
if not exist .\build mkdir .\build
pushd .\build

REM Code generator 
set CodeGeneratorPath=%ProjectPath%\tools\code_generation
set CodeGeneratorSource=%CodeGeneratorPath%\code_generator.c
%Compiler% %CompilerFlags% /I%CodePath% %CodeGeneratorSource%
code_generator.exe %CodePath%


REM Core module
set CorePath=%ProjectPath%\code\core
set CoreSource=%CorePath%\memory.c
%Compiler% %CompilerFlags% /I%CodePath% %CoreSource% /LD /Fecore


REM Unit tests
set UnitTesterPath=%ProjectPath%\tools\unit_testing
set UnitTesterSource=%UnitTesterPath%\unit_tester.c
%Compiler% %CompilerFlags% /I%CodePath% %UnitTesterSource%

popd
