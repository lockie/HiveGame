@echo off

if exist bin (
rmdir /s /q bin
)
mkdir bin
cd bin

call "%VS90COMNTOOLS%vsvars32.bat"
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
nmake
nmake package

copy /y *win32.exe ..
cd ..

pause
