;--------------------------------

!define BASE_NAME "OgreBullet"
!define PKG_NAME "${BASE_NAME}_Demo"
!define PKG_EXE "${PKG_NAME}_Setup.exe"
!define PKG_SUBMENU "Ogre SDK\${PKG_NAME}" 
!define PKG_FOLDER "${PKG_NAME}"
 
!define VERSION "V.1.0"

;--------------------------------
SetCompress off
SetCompressor /SOLID lzma
CRCCheck on

XPStyle on
SetDateSave on
SilentInstall normal

;--------------------------------
Var OGRE_HOME 
Var PKG_BINDIR

;-------------------------------
;Include Modern UI
!include "MUI.nsh"
  

;--------------------------------
Name "${PKG_NAME}"
OutFile "${PKG_EXE}"
InstallDirRegKey HKLM "Software\Ogre\${PKG_NAME}" "Install_Dir"


  
;----------------
;Modern UI Configuration

  !define MUI_FINISHPAGE_RUN "$PKG_BINDIR\${BASE_NAME}Demos.exe"
  ;!define MUI_FINISHPAGE_SHOWREADME "$PKG_BINDIR\${BASE_NAME}ReadMe.txt"
  !define MUI_FINISHPAGE_NOREBOOTSUPPORT 1

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY "Software\Ogre\${PKG_NAME}"
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP ".\${BASE_NAME}_header.bmp"
  
  !define MUI_WELCOMEFINISHPAGE_BITMAP ".\${BASE_NAME}_left.bmp"
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER "Ogre\${PKG_NAME}"
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ogre\${PKG_NAME}"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !define MUI_STARTMENUPAGE_VARIABLE "$R9"


  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;!insertmacro MUI_PAGE_STARTMENU  
  !insertmacro MUI_PAGE_LICENSE "license.txt"
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES  
  !insertmacro MUI_UNPAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_DIRECTORY
  !insertmacro MUI_UNPAGE_COMPONENTS
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "French"
 ; !insertmacro MUI_LANGUAGE "German"
 ; !insertmacro MUI_LANGUAGE "Spanish"
 ; !insertmacro MUI_LANGUAGE "SimpChinese"
 ; !insertmacro MUI_LANGUAGE "TradChinese"
  ;!insertmacro MUI_LANGUAGE "Japanese"
  ;;!insertmacro MUI_LANGUAGE "Korean"
  ;!insertmacro MUI_LANGUAGE "Italian"
  ;!insertmacro MUI_LANGUAGE "Dutch"
  ;!insertmacro MUI_LANGUAGE "Danish"
 ; !insertmacro MUI_LANGUAGE "Swedish"
 ; !insertmacro MUI_LANGUAGE "Greek"
 ; !insertmacro MUI_LANGUAGE "Russian"
 ; !insertmacro MUI_LANGUAGE "Portuguese"
 ; !insertmacro MUI_LANGUAGE "PortugueseBR"
 ; !insertmacro MUI_LANGUAGE "Polish"
 ; !insertmacro MUI_LANGUAGE "Ukrainian"
 ; !insertmacro MUI_LANGUAGE "Czech"
 ; !insertmacro MUI_LANGUAGE "Slovak"
 ; !insertmacro MUI_LANGUAGE "Croatian"
 ; !insertmacro MUI_LANGUAGE "Bulgarian"
 ; !insertmacro MUI_LANGUAGE "Hungarian"
 ; !insertmacro MUI_LANGUAGE "Thai"
  ;!insertmacro MUI_LANGUAGE "Romanian"
  ;!insertmacro MUI_LANGUAGE "Macedonian"
 ; !insertmacro MUI_LANGUAGE "Estonian"
 ; !insertmacro MUI_LANGUAGE "Turkish"
 ; !insertmacro MUI_LANGUAGE "Lithuanian"
 ; !insertmacro MUI_LANGUAGE "Catalan"
 ; !insertmacro MUI_LANGUAGE "Serbian"

;--------------------------------
;Reserve Files

  ;Things that need to be extracted on first (keep these lines before any File command!)
  ;Only for BZIP2 compression
  !insertmacro MUI_RESERVEFILE_LANGDLL
;--------------------------------
;Language Strings

  ;Description
    
  ;Header
  LangString TEXT_IO_SUBTITLE ${LANG_ENGLISH} "Please review the license terms before installing ${MUI_PRODUCT}"
  LangString TEXT_IO_SUBTITLE ${LANG_FRENCH} "Prière de lire la license avant l'installation ${MUI_PRODUCT}"

;--------------------------------
;Data
  
  
  LicenseData "license.txt"
  
;--------------------------------
Function .onInit
    ReadEnvStr $OGRE_HOME OGRE_HOME

    StrCpy $INSTDIR "$OGRE_HOME\..\${PKG_FOLDER}"
    StrCpy $PKG_BINDIR "$INSTDIR"
    
    # the plugins dir is automatically deleted when the installer exits
    InitPluginsDir
    #optional
    #File /oname=$PLUGINSDIR\splash.wav "C:\myprog\sound.wav"
    File /oname=$PLUGINSDIR\splash.bmp "${BASE_NAME}_splash.bmp"
    advsplash::show 1000 600 400 0x00005B $PLUGINSDIR\splash
    Pop $0

    Delete $PLUGINSDIR\splash.bmp
    !insertmacro MUI_LANGDLL_DISPLAY
    
FunctionEnd


Section "${PKG_NAME} (required)"

  SectionIn RO
  
   SetOutPath $INSTDIR  
  
  
              
    File ..\..\OgreSDK\bin\Release\OgreBulletDemos.exe      
               
    File /oname=Plugins.cfg Plugins_OgreBullet_binary.cfg       
    File /oname=resources.cfg resources_OgreBullet_binary.cfg     

    File ..\..\OgreSDK\bin\Release\OgreMain.dll                   
    File ..\..\OgreSDK\bin\Release\OIS.dll                 
    File ..\..\OgreSDK\bin\Release\Plugin_CgProgramManager.dll      
    File ..\..\OgreSDK\bin\Release\Plugin_ParticleFX.dll    
    
    File ..\..\OgreSDK\bin\Release\Plugin_BSPSceneManager.dll       
    File ..\..\OgreSDK\bin\Release\Plugin_OctreeSceneManager.dll         

    File ..\..\OgreSDK\bin\Release\RenderSystem_GL.dll              
    File ..\..\OgreSDK\bin\Release\RenderSystem_Direct3D9.dll       

    File ..\..\OgreSDK\bin\Release\cg.dll                         

    ;File "D:\Program Files (x86)\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest"    
    ;File "D:\Program Files (x86)\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\msvcm80.dll"    
    ;File "D:\Program Files (x86)\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\msvcp80.dll"    
    ;File "D:\Program Files (x86)\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\msvcr80.dll"    

    
   SetOutPath $INSTDIR\Media\OgreBullet
     File ..\Demos\Media\materials\*.
     File ..\Demos\Media\models\*.
     File ..\Demos\Media\overlays\*.
     File ..\Demos\Media\textures\*. 
     File ..\Demos\Media\gui\*. 


   SetOutPath $INSTDIR\Media\Ogre\packs
    File ..\..\OgreSDK\media\packs\OgreCore.zip
   
   SetOutPath $INSTDIR\Media\Ogre\fonts
   File ..\..\OgreSDK\media\fonts\*.*  
   
   SetOutPath $INSTDIR\Media\Ogre\materials\programs
   File ..\..\OgreSDK\media\materials\programs\*.*  
   
   SetOutPath $INSTDIR\Media\Ogre\materials\scripts
   File ..\..\OgreSDK\media\materials\scripts\*.* 
   
   SetOutPath $INSTDIR\Media\Ogre\materials\textures
   File ..\..\OgreSDK\media\materials\textures\*.* 
   
   SetOutPath $INSTDIR\Media\Ogre\models
   File ..\..\OgreSDK\media\models\*.* 
   
   SetOutPath $INSTDIR\Media\Ogre\overlays
   File ..\..\OgreSDK\media\overlays\*.* 
   
   SetOutPath $INSTDIR\Media\Ogre\particle
   File ..\..\OgreSDK\media\particle\*.*
   
   SetOutPath $INSTDIR\Media\Ogre\gui
   File ..\..\OgreSDK\media\gui\*.* 

   SetOutPath $INSTDIR  
   
  WriteRegStr HKLM "SOFTWARE\${PKG_SUBMENU}" "Install_Dir" "$INSTDIR"
  
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PKG_NAME}" "DisplayName" ${PKG_NAME}
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PKG_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PKG_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PKG_NAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
  Exec '"$INSTDIR\someprogram.exe"'

SectionEnd

Section "Start Menu Shortcuts"
  
  CreateDirectory "$SMPROGRAMS\${PKG_SUBMENU}"
  
  SetOutPath $PKG_BINDIR

  CreateShortCut  "$SMPROGRAMS\${PKG_SUBMENU}\${BASE_NAME}Demos.lnk" "$PKG_BINDIR\OgreBulletDemos.exe" "" "$PKG_BINDIR\OgreBulletDemos.exe" 0

  SetOutPath $OUTDIR

  CreateShortCut  "$SMPROGRAMS\${PKG_SUBMENU}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

Section "Uninstall"
    
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PKG_NAME}"
  DeleteRegKey HKLM SOFTWARE\Ogre\${BASE_NAME}

  RMDir /r "$SMPROGRAMS\${PKG_SUBMENU}\"  
  RMDir /r "$INSTDIR"

SectionEnd