
!include "MUI2.nsh"

Name "MineralApp"
OutFile "mineralapp-install.exe"
Unicode True
InstallDir "$PROGRAMFILES\MineralApp"

!define MUI_ICON "..\icon\mineralapp.ico"
!define MUI_UNICON "..\icon\mineralapp.ico"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section
    SetOutPath $INSTDIR
    File Release\mineralapp.exe
    File Release\expat.dll
    File Release\jpeg62.dll
    File Release\libpng16.dll
    File Release\lzma.dll
    File Release\sqlite3.dll
    File Release\tiff.dll
    File Release\wxbase313u_vc_custom.dll
    File Release\wxbase313u_xml_vc_custom.dll
    File Release\wxmsw313u_core_vc_custom.dll
    File Release\wxmsw313u_html_vc_custom.dll
    File Release\wxmsw313u_richtext_vc_custom.dll
    File Release\zlib1.dll
    WriteUninstaller $INSTDIR\uninstall.exe
    CreateShortcut "$SMPROGRAMS\MineralApp.lnk" "$INSTDIR\mineralapp.exe"
    CreateShortcut "$DESKTOP\MineralApp.lnk" "$INSTDIR\mineralapp.exe"
SectionEnd
 
Section "Uninstall"
    Delete $INSTDIR\uninstall.exe
    Delete $INSTDIR\mineralapp.exe
    Delete $INSTDIR\expat.dll
    Delete $INSTDIR\jpeg62.dll
    Delete $INSTDIR\libpng16.dll
    Delete $INSTDIR\lzma.dll
    Delete $INSTDIR\sqlite3.dll
    Delete $INSTDIR\tiff.dll
    Delete $INSTDIR\wxbase313u_vc_custom.dll
    Delete $INSTDIR\wxbase313u_xml_vc_custom.dll
    Delete $INSTDIR\wxmsw313u_core_vc_custom.dll
    Delete $INSTDIR\wxmsw313u_html_vc_custom.dll
    Delete $INSTDIR\wxmsw313u_richtext_vc_custom.dll
    Delete $INSTDIR\zlib1.dll
    Delete "$SMPROGRAMS\MineralApp.lnk"
    Delete "$DESKTOP\MineralApp.lnk"
    RMDir $INSTDIR
SectionEnd

