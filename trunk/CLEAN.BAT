@echo off

setlocal ENABLEEXTENSIONS

if %PROCESSOR_ARCHITECTURE%==x86 (

	FOR /F "usebackq tokens=3*" %%A IN (`REG QUERY "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\12.0" /v "InstallDir" 2^>nul`) DO ( set appdir=%%A %%B )
	if defined appdir (
		goto CLEAN
	)
	
) else (

	FOR /F "usebackq tokens=3*" %%A IN (`REG QUERY "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\12.0" /v "InstallDir" 2^>nul`) DO ( set appdir=%%A %%B )
	if defined appdir (
		goto CLEAN
	)

)

goto ERROR

:CLEAN
ECHO Clean solution...
start /b /w "" "%appdir%\devenv.exe" /clean Debug OnePercent.sln

goto END


:ERROR

ECHO Visual Studio 12.0 installation not found

:END