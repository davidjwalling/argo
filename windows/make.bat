@echo off
if "%1" == "clean" (
	if exist Win32 rmdir /s /q Win32
	if exist x64 rmdir /s /q x64
	if exist test.* del /f test.*
	exit /b
)
Setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild -m argo.sln /p:Configuration=Debug /p:Platform=x64
msbuild -m argo.sln /p:Configuration=Release /p:Platform=x64
msbuild -m argo.sln /p:Configuration=Debug /p:Platform=x86
msbuild -m argo.sln /p:Configuration=Release /p:Platform=x86
Endlocal
if "%1" == "install" (
	copy x64\Release\libargo.dll C:\Windows\System32
	copy x64\Release\argo.exe C:\Windows\System32
	copy x64\Release\testargo.exe C:\Windows\System32
	copy ..\argo.cfg C:\Windows\System32
)
if "%1" == "uninstall" (
	del C:\Windows\System32\libargo.dll
	del C:\Windows\System32\argo.exe
	del C:\Windows\System32\testargo.exe
)
exit /b
