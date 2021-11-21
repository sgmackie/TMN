@echo off

REM Working directories 
set ProjectName=TMNE
set ProjectPath=%CD%
set CodePath=%ProjectPath%\code
set OutputPath=%ProjectPath%\build

REM Commom options
set Compiler=cl
set CompilerFlags=/nologo /WX /WL /FC /Wv:18 /std:c++14
set CompilerFlags=/DCORE_PLATFORM_WINDOWS=1 %CompilerFlags%

REM Release builds
if "%1"=="release" set ReleaseBuild = true 
if "%1"=="Release" set ReleaseBuild = true 
if "%ReleaseBuild%"=="true" (
    echo %ProjectName% Release Build
    set CompilerFlags=/O2 /MD /Zo %CompilerFlags%
    set CompilerLibFlags=/O2 /MT /Zo %CompilerFlags%
) else (
    echo %ProjectName%: Debug Build
    set CompilerFlags=/Od /MDd /Zi %CompilerFlags%
    set CompilerLibFlags=/O2 /MTd /Zo %CompilerFlags%
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

REM List headers
set HeaderFiles=/I%CodePath%
set HeaderFiles=/I%CodePath%\Core %HeaderFiles%

REM List sources
set SourceFiles=%CodePath%\main.cpp
set SourceFiles=%CodePath%\Core\Source\Unicode.cpp %SourceFiles%
set SourceFiles=%CodePath%\Core\Source\PlatformWindows.cpp %SourceFiles%

%Compiler% %CompilerFlags% %SourceFiles%
if not %ERRORLEVEL% == 0 (
    goto end
)

:end
    REM Delete intermediate files
    del *.obj
    popd