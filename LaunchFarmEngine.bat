@echo off
title Metin2 Svside Rapid Monetization Engine Launcher
color 0A

echo ===================================================================
echo    Metin2 Svside Rapid Monetization & Mass Farm Launcher v3.0
echo ===================================================================
echo.
echo [*] Step 1: Connecting to Ring 0 Kernel Driver...
echo [*] Step 2: Unlinking Process from EPROCESS ActiveProcessLinks (DKOM)...
echo [*] Step 3: Loading Svside Profile (profiles/svside_profile.json)...
echo [*] Step 4: Initializing Multi-Client Parallel Farm Orchestrator...
echo.

if exist "GlobalMain.exe" (
    start GlobalMain.exe profiles/svside_profile.json
    echo [+] Farm Engine launched successfully!
) else (
    echo [!] GlobalMain.exe executable not found. Please compile the solution in Visual Studio.
)

echo.
pause
