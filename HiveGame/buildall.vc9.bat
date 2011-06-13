@echo off

set STARTTIME=%TIME%

if exist bin (
rmdir /s /q bin
)
mkdir bin
cd bin

call "%VS90COMNTOOLS%vsvars32.bat"
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
nmake package

copy /y *win32.exe ..
cd ..

set ENDTIME=%TIME%
set STARTTIME=%STARTTIME: =0%
set /A STARTTIME=(1%STARTTIME:~0,2%-100)*360000 + (1%STARTTIME:~3,2%-100)*6000 + (1%STARTTIME:~6,2%-100)*100 + (1%STARTTIME:~9,2%-100)
set ENDTIME=%ENDTIME: =0%
set /A ENDTIME=(1%ENDTIME:~0,2%-100)*360000 + (1%ENDTIME:~3,2%-100)*6000 + (1%ENDTIME:~6,2%-100)*100 + (1%ENDTIME:~9,2%-100)
set /A DURATION=(%ENDTIME%-%STARTTIME%) / 100
echo Build took %DURATION% s

if not defined NOPAUSE pause
