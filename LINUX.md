# Argo on Linux
## Update Prerequisites
### Debian, Ubuntu, Mint
```
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install build-essential
$ sudo apt autoremove
$ sudo snap install code
```
## Configure Tools
### ~/.vimrc
Create or edit the ~/.vimrc file so that tabs are expanded to four spaces when editing source files.  
Use "vi -u NONE Makeflie" when editting Makefile to preserve its tabs.
```
set expandtab
set ts=4
```
### .editorconfig
Argo includes .editorconfig to preserve Makefile tabs when using Visual Studio.  
An extension might need to be installed to enable .editorconfig when using Visual Code.
```
root = true

[Makefile*]
indent_style = tab
indent_size = 8

[*.{cpp,h}]
indent_style = space
indent_size = 4
```
### git config
Configure your user name and email for using git.
```
$ git config --global user.name <user>
$ git config --global user.email <email>
```
### Visual Studio Code Configuration
Launch Visual Studio Code.  
Install the Microsoft C/C++, C/C++ Extension Pack, and C/C++ Themes.  
I also install the Native Debug extension.
## Download
Clone a local copy of the Argo repository.  
Here, we create the argo folder in a new "repos" parent folder.
```
$ cd ~
$ mkdir repos && cd repos
$ git clone https://github.com/davidjwalling/argo.git
```
## Build
### Build Using CMake
```
$ cd ~/repos/argo
$ mkdir build && cd build
$ cmake -DCMAKE_TYPE_TYPE=Debug ..
$ sudo make install
$ sudo ldconfig
```
