## Argo on Windows
### Download
```
C:> cd repos
C:\repos> git clone https://github.com/davidjwalling/argo.git
C:\repos> cd argo
```
### Build Using Make
Open the "x64 Native Tools Command Prompt" as an Administrator for your version and edition of Visual Studio.
make.bat installs to C:\Program Files\System32.
Update the system PATH environment variable as needed.
```
C:\repos\argo> cd windows
C:\repos\argo\windows> make clean
C:\repos\argo\windows> make install
C:\repos\argo\windows> cd ..
```
### Build Using CMake
Make sure CMake is installed on your system.  
Open the "x64 Native Tools Command Prompt" as an Administrator for your version and edition of Visual Studio.  
CMake installs to C:\Program Files (x86)\argo\bin.
Update the system PATH environment variable as needed.
```
C:\repos\argo> rmdir /s /q build
C:\repos\argo> mkdir build && cd build
C:\repos\argo\build> cmake ..
C:\repos\argo\build> cmake --build . --config Debug --target install
C:\repos\argo\build> cd ..
```
### Run Argo in the Foreground
Use the "where argo" command to confirm the location of the Argo executable.  
```
C:\repos\argo> where argo
C:\Windows\System32\argo.exe
C:\repos\argo>argo
I1001 Argo 0.X Experimental
I1002 Copyright 2010 David J. Walling. MIT License.
I1055 Configuration completed
exit
I1056 Service stopping
I1057 Exiting main loop
I1058 Finalization completed
```
### Install Argo as a Windows Service
```
C:\repos\argo> argo install
```
### Start the Argo Windows Service
```
C:\repos\argo> sc start ava
C:\repos\argo> sc query ava
C:\repos\argo> netstat -an | findstr 1143
```
### Stop the Argo Windows Service
```
C:\repos\argo> sc stop argo
C:\repos\argo> sc query argo
```
### Uninstall the Argo Windows Service
```
C:\repos\argo> argo uninstall
C:\repos\argo> sc query argo
```
### Run the Test Program
```
C:\repos\argo> testargo all
Argo Test Program [0.X]
Copyright 2010 David J. Walling. MIT License.

sizeof(short)      2 bytes
sizeof(int)        4 bytes
sizeof(long)       4 bytes
sizeof(long long)  8 bytes
...
Tests Completed
```
