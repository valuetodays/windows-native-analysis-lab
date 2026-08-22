@echo off
setlocal

cd /d "%~dp0"

echo === Compiler ===
g++ --version
echo.

echo === Build LIFE-002 Debug ===

g++ main.cpp  -std=c++20  -municode  -mwindows -g3  -O0  -fno-omit-frame-pointer  -Wall -Wextra -Wpedantic -o LIFE-002-windows-subsystem-debug.exe

if errorlevel 1 (
  echo.
  echo Build failed.
  exit /b 1
)

echo.
echo Build successful:
echo LIFE-002-windows-subsystem-debug.exe

exit /b 0
