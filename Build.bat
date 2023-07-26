@echo off

REM Build enviroment
set ProjectName=Raytracer
set PlatformName=Windows
set WorkingDirectory=%~dp0

REM Set output path
set OutputDirectory=%WorkingDirectory%Bin\%ProjectName%

REM Cleanup last build
if exist %OutputDirectory% rmdir /s /q %OutputDirectory%
if not exist %OutputDirectory% mkdir %OutputDirectory%

REM Project files
set SourceFiles=%WorkingDirectory%Source\Main.c

REM Include files
set IncludePaths=/I%WorkingDirectory%Source\3rdParty\SDL\include
set SuperluminalPath=/I"%SUPERLUMINAL_API_DIR%"\include
set IncludePaths=%IncludePaths% %SuperluminalPath%

REM Lib files
set BuildType="%1"
if %BuildType%=="Debug" set LibFiles="%SUPERLUMINAL_API_DIR%"\lib\x64\PerformanceAPI_MTd.lib
if %BuildType%=="Release" set LibFiles="%SUPERLUMINAL_API_DIR%"\lib\x64\PerformanceAPI_MT.lib

REM Compiler settings
set CompilerFlags=/nologo /WX /WL /FC

REM Default to Debug builds
set CompilerOptimisation=/Od /MTd /Zi
if %BuildType%=="Release" set CompilerOptimisation=/O2 /MT /Zi

pushd %OutputDirectory%

REM Build and link SDL
MSBuild %WorkingDirectory%Source\3rdParty\SDL\VisualC\SDL\SDL.vcxproj /p:Configuration=Debug /p:OutDir=%OutputDirectory%\\
set LibFiles=%LibFiles% %OutputDirectory%\SDL3.lib

REM Build project
cl %CompilerFlags% %CompilerOptimisation% /Fe"%OutputDirectory%\%ProjectName%_%PlatformName%" %IncludePaths% %SourceFiles% %LibFiles%

popd