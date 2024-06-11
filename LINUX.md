# Argo on Linux
## Update Prerequisites
### Debian, Ubuntu, Mint
```
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install build-essential code
$ sudo apt autoremove
```
### Fedora
```
$ sudo dnf update
$ sudo dnf install @development-tools
$ sudo dnf install code
```
### CentOS, Oracle
```
$ sudo yum update
$ sudo yum upgrade
$ sudo yum groupinstall "Development Tools"
$ sudo yum install code
```
## Configure Tools
### ~/.vimrc
When using vim, expand each tab to four spaces.  
Exclude this when editing Makefile by using "vi -u NONE Makefile".
```
set expandtab
set ts=4
```
### .editorconfig
Apply appropriate spacing when using VS Code.
```
root = true

[Makefile*]
indent_style = tab
indent_size = 8

[*.{cpp,h}]
indent_style = space
indent_size = 4
```
