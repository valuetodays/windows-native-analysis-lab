@echo off
setlocal

cd /d "%~dp0"

echo === Compiler ===
g++ --version
echo.

g++ main.cpp ^
    -std=c++20 ^
    -O0 ^
    -s ^
    -Wall ^
    -Wextra ^
    -Wpedantic ^
    -o REG-001-registry-read-write-release.exe

if errorlevel 1 (
    echo.
    echo Build failed.
    exit /b 1
)

echo.
echo Build successful.
exit /b 0