@echo off

REM Working directories 
set ProjectPath=%CD%
set CodePath=%ProjectPath%\code
set OutputPath=%ProjectPath%\build

REM Commom options
set Compiler=cl
set CompilerFlags=/nologo /WX /WL /FC /Wv:18
set CompilerFlags=/DPLATFORM_WINDOWS=1 %CompilerFlags%

REM Release builds
if "%1"=="release" set ReleaseBuild = true 
if "%1"=="Release" set ReleaseBuild = true 
if "%ReleaseBuild%"=="true" (
    echo Release Build
    set CompilerFlags=/O2 /MD /Zo %CompilerFlags%
) else (
    echo Debug Build
    set CompilerFlags=/Od /MDd /Zi %CompilerFlags%
)

REM Compiler tools
if "%2"=="sanitize" set SanitizeBuild="true" 
if "%2"=="Sanitize" set SanitizeBuild="true" 
if "%SanitizeBuild%"=="true" (
    echo Running Address Sanitizer
    set CompilerFlags=/fsanitize=address %CompilerFlags%
)

REM Start build
if not exist %OutputPath% mkdir %OutputPath%
pushd %OutputPath%

REM Code generator 
set CodeGeneratorPath=%CodePath%\tools\code_generation
set CodeGeneratorSource=%CodeGeneratorPath%\code_generator.c
%Compiler% %CompilerFlags% /I%CodePath%\engine %CodeGeneratorSource%
if not %ERRORLEVEL% == 0 (
    goto end
)

REM Run code generator
code_generator.exe %CodePath%


REM Core module
set CorePath=%CodePath%\engine\core
set CoreSource=%CorePath%\memory.c
%Compiler% %CompilerFlags% /I%CodePath%\engine %CoreSource% /LD /Fecore
if not %ERRORLEVEL% == 0 (
    goto end
)

REM Unit tests
set UnitTesterPath=%CodePath%\tools\unit_testing
set UnitTesterSource=%UnitTesterPath%\unit_tester.c
%Compiler% %CompilerFlags% /I%CodePath%\engine %UnitTesterSource%
if not %ERRORLEVEL% == 0 (
    goto end
)

:end
    popd
