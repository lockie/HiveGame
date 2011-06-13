@echo off
set NOPAUSE=1

cd Ogitor
call buildall.vc9.bat
move *win32.exe ..

cd ../HiveGame
call buildall.vc9.bat
move *win32.exe ..
cd ..
pause
