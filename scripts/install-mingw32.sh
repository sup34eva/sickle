#!/bin/bash
sudo apt-get install qt54tools p7zip binutils-mingw-w64-i686 gcc-mingw-w64-i686 g++-mingw-w64-i686 wine -qq > /dev/null
wget http://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt5_54/qt.54.win32_mingw491/5.4.0-1qt5_essentials.7z
7zr x 5.4.0-1qt5_essentials.7z -y -oQt > /dev/null
