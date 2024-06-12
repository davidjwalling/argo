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
Use the "where ava" command to confirm the location of the Ava executable.  
```
C:\repos\argo> where argo
C:\Windows\System32\argo.exe
```
### Run Argo as a Windows Service
#### Install the Service
#### Start the Service
#### Stop the Service
#### Uninstall the Service
### Run the Test Program
