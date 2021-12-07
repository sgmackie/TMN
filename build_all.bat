@echo off
cmake --build ./build/Ninja --config Debug
cmake --build ./build/Ninja --config Release
cmake --build ./build/VS2019 --config Debug
cmake --build ./build/VS2019 --config Release