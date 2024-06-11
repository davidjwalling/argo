## Argo on Linux
Commands on this page were tested on Linux Mint Debian Edition (LMDE).
### Update Installed Packages
```
$ sudo apt update
$ sudo apt upgrade
$ sudo apt autoremove
```
### Install Build Tools
```
$ sudo apt install build-essential git cmake net-tools
$ sudo snap install --classic code
```
### Configure Build Tools
#### ~/.vimrc
Create or edit the ~/.vimrc file so that tabs are expanded to four spaces when editing source files.  
Use "vi -u NONE Makeflie" when editting Makefile to preserve its tabs.
```
set expandtab
set ts=4
```
#### .editorconfig
An .editorconfig is included to preserve Makefile tabs when using Visual Studio.  
Install the EditorConfig extension for Visual Studio Code if needed.
#### git
Configure your user name and email for using git.
```
$ git config --global user.name <user>
$ git config --global user.email <email>
```
#### Visual Studio Code
Install Microsoft C/C++, C/C++ Extension Pack, C/C++ Themes.  
I also use the Native Debug extension from WebFreak.
### Download Argo
```
$ cd ~
$ mkdir repos && cd repos
$ git clone https://github.com/davidjwalling/argo.git
```
### Build
#### Build Using Make
A Makefile is included as an alternative to using CMake.
```
$ cd ~/repos/argo
$ make
```
#### Build Using CMake
Running CMake from within a build folder help to keep CMake artifacts separate from the repository sources.
```
$ cd ~/repos/argo
$ mkdir build && cd build
$ cmake -DCMAKE_TYPE_TYPE=Debug ..
$ sudo make install
$ sudo ldconfig
```
