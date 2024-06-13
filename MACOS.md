## Argo on macOS
### Download
```
me@macbook ~ % cd repos
me@macbook repos % git clone https://github.com/davidjwalling/argo.git
me@macbook repos % cd argo
```
### Build Using Make
make installs to /usr/local/lib and /usr/local/bin
```
me@macbook argo % make clean
me@macbook argo % make
me@macbook argo & sudo make install
```
### Build Using CMake
Make sure CMake is installed on your system.
CMake installs to /usr/local/lib, /usr/local/include and /usr/local/bin
```
me@macbook argo % rm -rf build
me@macbook argo % mkdir build && cd build
me@macbook build % cmake -DCMAKE_BUILD_TYPE=Debug ..
me@macbook build % make
me@macbook build % sudo make install
me@macbook build % cd ..
```
### Run Argo in the Foreground
Use the "which argo" command to confirm the location of the Argo executable.
```
me@macbook argo % which argo
/usr/local/bin/argo
```
### Run Argo in the Background
```
```
### Run Argo as a Service
```
```
### Install the Service
```
```
### Start the Service
```
```
### Stop theService
```
```
### Unintall the Service
```
```
### Run the Test Program
```
```
