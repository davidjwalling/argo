@echo off
if "%1" == "clean" (
	if exist Win32 rmdir /s/q Win32
	if exist x64 rmdir /s/q x64
	exit /b
)
Setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild -m argo.sln /p:Configuration=Debug /p:Platform=x64
msbuild -m argo.sln /p:Configuration=Release /p:Platform=x64
msbuild -m argo.sln /p:Configuration=Debug /p:Platform=x86
msbuild -m argo.sln /p:Configuration=Release /p:Platform=x86
Endlocal
exit /b
