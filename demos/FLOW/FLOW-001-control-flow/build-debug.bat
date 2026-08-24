@echo off
setlocal

cd /d "%~dp0"

echo === Compiler ===
g++ --version
echo.

g++ main.cpp -O0 -s -o FLOW-001-control-flow-release.exe

if errorlevel 1 (
    echo.
    echo Build failed.
    exit /b 1
)

echo.
echo Build successful.

exit /b 0
