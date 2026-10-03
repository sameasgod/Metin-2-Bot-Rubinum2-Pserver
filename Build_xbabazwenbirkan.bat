@echo off
title Building xbabazwenbirkan.exe - Per-Client Isolated Multi-Launcher
color 0A

echo ===================================================================
echo   Building xbabazwenbirkan Standalone Executable ^& Spoofer Engine
echo ===================================================================
echo.

set CSC="C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe"

if not exist %CSC% (
    set CSC="C:\Windows\Microsoft.NET\Framework\v4.0.30319\csc.exe"
)

echo [*] Compiling xbabazwenbirkan_app.cs using %CSC%...
%CSC% /target:winexe /win32icon:xbabazwenbirkan.ico /out:xbabazwenbirkan.exe xbabazwenbirkan_app.cs

if exist "xbabazwenbirkan.exe" (
    echo.
    echo [+] SUCCESS: xbabazwenbirkan.exe generated successfully!
) else (
    echo.
    echo [!] BUILD FAILED. Please inspect compiler logs above.
)

pause
