;--------------------------------
;Include Modern UI

!include "MUI2.nsh"

;--------------------------------
;General

!define MUI_PRODUCT "HiveGame"
!define MUI_FILE "hivegame"
;Get version from RCS
!system 'git describe --tags > version.tmp'
!define /file MUI_VERSION version.tmp
!delfile version.tmp

;Name and file
Name "HiveGame"
OutFile "${MUI_FILE}-${MUI_VERSION}-setup.exe"

;Default installation folder
InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"

;Request application privileges for Windows Vista/7
RequestExecutionLevel user

;Check CRC
CRCCheck On

;Best compression
SetCompressor /FINAL /SOLID bzip2

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY 
!insertmacro MUI_PAGE_INSTFILES 
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "Russian"

;--------------------------------
;Installer Sections

Section ""

SetOutPath "$INSTDIR"

;Files
File /a "Release\${MUI_FILE}.exe"
File /a "Release\_pyhivegame.pyd"
File /a "Release\pyhivegame.py"
File /a "vfs.cfg"
File /a "${MUI_FILE}.cfg"
File /a "autoexec.py"
File /a "$%VS80COMNTOOLS%\..\..\VC\redist\x86\Microsoft.VC80.CRT\*"
; TODO : вся эта каша не нужна, оставить только нужное
File /a "C:\Projects\CrystalSpace\crystalspace-src-1.4.0\out\release8\bin\*.dll"
File /a "C:\Projects\CrystalSpace\cel-src-1.4.0\out\release8\bin\*.dll"
File /a "C:\Projects\CrystalSpace\CrystalSpaceLibs\dlls\*-cs.dll"
File /a "C:\Projects\CrystalSpace\CrystalSpaceLibs\dlls\cg*.dll"
File /a "C:\Projects\CrystalSpace\CrystalSpaceLibs\dlls\vc\*-csvc8.dll"
File /a "C:\Projects\Python-2.7.1\PC\VS8.0\python27.dll"
SetOutPath "$INSTDIR\lib"
File /a /r "C:\Projects\Python-2.7.1\Lib\*"
SetOutPath "$INSTDIR\lib\cspace"
File /a "C:\Projects\CrystalSpace\crystalspace-src-1.4.0\scripts\python\frozen\cspace\*.py"
File /a "C:\Projects\CrystalSpace\crystalspace-src-1.4.0\scripts\python\cshelper.py"
File /a "C:\Projects\CrystalSpace\crystalspace-src-1.4.0\scripts\python\csutils.py"
File /a "C:\Projects\CrystalSpace\crystalspace-src-1.4.0\out\release8\bin\*.pyd"
File /a "$%VS80COMNTOOLS%\..\..\VC\redist\x86\Microsoft.VC80.CRT\*"


;create desktop shortcut
CreateShortCut "$DESKTOP\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe"

;create start-menu items
CreateDirectory "$SMPROGRAMS\${MUI_PRODUCT}"
CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" "" "$INSTDIR\${MUI_FILE}.exe" 0

;Create uninstaller
WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

;Delete Files 
RMDir /r "$INSTDIR\*.*"    
Delete "$INSTDIR\Uninstall.exe"

;Remove the installation directory
RMDir "$INSTDIR"
 
;Delete Start Menu Shortcuts
Delete "$DESKTOP\${MUI_PRODUCT}.lnk"
Delete "$SMPROGRAMS\${MUI_PRODUCT}\*.*"
RMDir  "$SMPROGRAMS\${MUI_PRODUCT}"

SectionEnd
