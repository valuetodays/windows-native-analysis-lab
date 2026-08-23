@echo off
setlocal

cd /d "%~dp0"

echo === Compiler ===
g++ --version
echo.

echo === Build FILE-001 Release ===

g++ main.cpp ^
    -std=c++20 ^
    -O0 ^
    -s ^
    -Wall ^
    -Wextra ^
    -Wpedantic ^
    -o FILE-001-file-read-write-release.exe

if errorlevel 1 (
    echo.
    echo Build failed.
    exit /b 1
)

echo.
echo Build successful:
echo FILE-001-file-read-write-release.exe

exit /b 0