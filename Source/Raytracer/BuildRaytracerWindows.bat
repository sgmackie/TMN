@echo off

REM Build enviroment
set ProjectName="Raytracer"
set PlatformName="Windows"
set WorkingDirectory=%~dp0

REM Input
REM TODO: Recursive add to a variable instead of this file hack
if exist %WorkingDirectory%\SourceFiles.txt del %WorkingDirectory%\SourceFiles.txt

REM 3rd Party
set SourceFiles=%WorkingDirectory%..\3rdParty\tlsf\tlsf.c

REM 3rd Party - MiMalloc
set SourceMiMallocDirectory=%WorkingDirectory%..\3rdParty\mimalloc\src
for /f %%i in ('FORFILES /P %SourceMiMallocDirectory% /S /M static.c /C "cmd /c echo @PATH"') do (
	echo %%~i >> %WorkingDirectory%\SourceFiles.txt
)

REM Core
set SourceCoreDirectory=%WorkingDirectory%..\Core
for /f %%i in ('FORFILES /P %SourceCoreDirectory% /S /M *.cpp /C "cmd /c echo @PATH"') do (
	echo %%~i >> %WorkingDirectory%\SourceFiles.txt
)

setlocal EnableExtensions DisableDelayedExpansion
set "content="
for /F "usebackq delims=" %%x in ("%WorkingDirectory%\SourceFiles.txt") do (
   set "item=%%x"
   setlocal EnableDelayedExpansion
   for /F "delims=" %%a in ("content=!content!!item!") do (
       endlocal
       set "%%a"
   )
)
setlocal EnableDelayedExpansion
set SourceFiles=%SourceFiles% !content!

REM Project files
set SourceFiles=%SourceFiles% %WorkingDirectory%RaytracerMain.cpp

REM Include files
set IncludePaths=/I%SourceCoreDirectory% /I%WorkingDirectory%..\3rdParty\mimalloc\include
set SuperluminalPath=/I"%SUPERLUMINAL_API_DIR%"\include
set IncludePaths=%IncludePaths% %SuperluminalPath%

REM Lib files
set BuildType="%1"
if %BuildType%=="Debug" set LibFiles="%SUPERLUMINAL_API_DIR%"\lib\x64\PerformanceAPI_MTd.lib
if %BuildType%=="Release" set LibFiles="%SUPERLUMINAL_API_DIR%"\lib\x64\PerformanceAPI_MT.lib

REM Compiler settings
set CompilerFlags=/nologo /WX /WL /FC /std:c++14
set CompilerDefines=/DCORE_PLATFORM_WINDOWS=1 /DCORE_PROFILER_SUPERLUMINAL=1

REM Default to Debug builds
set CompilerOptimisation=/Od /MTd /Zi
if %BuildType%=="Release" set CompilerOptimisation=/O2 /MT /Zi

REM Set output path
set OutputDirectory=%WorkingDirectory%..\..\Bin\%ProjectName%\%PlatformName%\%BuildType%

REM Cleanup last build
rmdir /s /q %OutputDirectory%
if not exist %OutputDirectory% mkdir %OutputDirectory%
pushd %OutputDirectory%

REM Build and run unit tests
REM TODO: Call unit test build script, exit this script on fail

REM Run compiler
cl %CompilerFlags% %CompilerDefines% %CompilerOptimisation% /Fe"%OutputDirectory%\%ProjectName%" %IncludePaths% %SourceFiles% %LibFiles% /link /SUBSYSTEM:windows

popd

endlocal
