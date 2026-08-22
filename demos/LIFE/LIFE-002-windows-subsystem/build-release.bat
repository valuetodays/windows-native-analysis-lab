@echo off
setlocal

cd /d "%~dp0"

echo === Compiler ===
g++ --version
echo.

echo === Build LIFE-002 Release ===

g++ main.cpp ^
    -std=c++20 ^
    -municode ^
    -mwindows ^
    -O0 ^
    -s ^
    -Wall ^
    -Wextra ^
    -Wpedantic ^
    -o LIFE-002-windows-subsystem-release.exe

if errorlevel 1 (
    echo.
    echo Build failed.
    exit /b 1
)

echo.
echo Build successful:
echo LIFE-002-windows-subsystem-release.exe

exit /b 0