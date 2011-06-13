@echo off

if exist bin (
rmdir /s /q bin
)
mkdir bin
cd bin

call "%VS90COMNTOOLS%vsvars32.bat"
cmake -G "Visual Studio 9 2008" -DCMAKE_BUILD_TYPE=Release ..
vcbuild.exe /time Ogitor.sln "Release|Win32"
vcbuild.exe PACKAGE.vcproj "Release|Win32"

copy /y *win32.exe ..
cd ..

if not defined NOPAUSE pause
