!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"

!AddPluginDir "res\plugins"

!define APP_NAME      "Screenpic"
!define APP_NAME_LOW  "screenpic"
!define APP_DOMAIN    "rup.io"
!define APP_REGKEY    "Software\${APP_NAME}"
!define APP_VERSION   "0.13.0"
!define APP_REVISION  "0"
!define APP_COPYRIGHT "Copyright © 2013-2014 Alexander Sedov"
!define APP_SITE      "http://${APP_DOMAIN}"
!define APP_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

Var update

Name "${APP_NAME}"
BrandingText "${APP_COPYRIGHT}"
OutFile "bin\${APP_NAME_LOW}-${APP_VERSION}.exe"
InstallDir "$LOCALAPPDATA\${APP_NAME}"
InstallDirRegKey HKCU "Software\${APP_NAME}" ""
RequestExecutionLevel user

VIProductVersion "${APP_VERSION}.${APP_REVISION}"
VIAddVersionKey  "CompanyName"      "${APP_NAME}"
VIAddVersionKey  "FileDescription"  "${APP_NAME} Installer"
VIAddVersionKey  "FileVersion"      "${APP_VERSION}"
VIAddVersionKey  "LegalCopyright"   "${APP_COPYRIGHT}"
VIAddVersionKey  "OriginalFilename" "${APP_NAME_LOW}-${APP_VERSION}.exe"
VIAddVersionKey  "ProductName"      "${APP_NAME}"
VIAddVersionKey  "ProductVersion"   "${APP_VERSION}"

SetCompressor /SOLID lzma
SetCompressorDictSize 8

ReserveFile "res\plugins\FindProcDLL.dll"

!define MUI_ABORTWARNING
!define MUI_ICON                       "res\app.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP   "res\wizard.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "res\wizard.bmp"
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_LINK          "${APP_SITE}"
!define MUI_FINISHPAGE_LINK_LOCATION "${APP_SITE}"
!define MUI_FINISHPAGE_RUN           "$INSTDIR\${APP_NAME_LOW}.exe"

!define MUI_PAGE_CUSTOMFUNCTION_PRE updatePre
!insertmacro MUI_PAGE_WELCOME
!define MUI_PAGE_CUSTOMFUNCTION_PRE updatePre
!insertmacro MUI_PAGE_COMPONENTS
!define MUI_PAGE_CUSTOMFUNCTION_PRE updatePre
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_PAGE_CUSTOMFUNCTION_PRE updatePre
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
!define MUI_LANGDLL_REGISTRY_KEY "${APP_REGKEY}" 
!define MUI_LANGDLL_REGISTRY_VALUENAME "language"

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

!insertmacro MUI_RESERVEFILE_LANGDLL


LangString SEC_CORE     ${LANG_ENGLISH} "Core Components"
LangString DESC_CORE    ${LANG_ENGLISH} "Core Components of ${APP_NAME}"
LangString SEC_DESKTOP  ${LANG_ENGLISH} "Desktop Shortcut"
LangString DESC_DESKTOP ${LANG_ENGLISH} "Create desktop shortcut"
LangString SEC_STARTUP  ${LANG_ENGLISH} "Add to Startup"
LangString DESC_STARTUP ${LANG_ENGLISH} "Add ${APP_NAME} to Startup"

LangString SEC_CORE     ${LANG_RUSSIAN} "Основные компоненты"
LangString DESC_CORE    ${LANG_RUSSIAN} "Основные компоненты ${APP_NAME}"
LangString SEC_DESKTOP  ${LANG_RUSSIAN} "Ярлык на рабочем столе"
LangString DESC_DESKTOP ${LANG_RUSSIAN} "Создать ярлык на рабочем столе"
LangString SEC_STARTUP  ${LANG_RUSSIAN} "Добавить в автозагрузку"
LangString DESC_STARTUP ${LANG_RUSSIAN} "Добавить ${APP_NAME} в автозагрузку"


!macro KILL_ALL _NAME
  FindProcDLL::FindProc "${_NAME}"
  Pop $R0

  ${If} $R0 == 1
    ExecWait '"${_NAME}" -exit'

    ${While} $R0 == 1
      FindProcDLL::FindProc "${_NAME}"
      Pop $R0
      Sleep 100
    ${EndWhile}
  ${EndIf}
!macroend


Section "$(SEC_CORE)" SecCore
  SectionIn RO

  !insertmacro KILL_ALL "$INSTDIR\${APP_NAME_LOW}.exe"

  SetOutPath "$INSTDIR"
  File "bin\libeay32.dll"
  File "bin\ssleay32.dll"

  File "bin\${APP_NAME_LOW}.exe"

  ${If} $update == false
    WriteRegStr HKCU "${APP_REGKEY}" "" $INSTDIR
    WriteRegStr HKCU "${APP_REGKEY}" "language" $LANGUAGE
  
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut  "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_NAME_LOW}.exe" "" "" "" "" "" "${APP_NAME} ${APP_VERSION}"

    WriteUninstaller "$INSTDIR\uninstall.exe"
    WriteRegStr HKCU "${APP_UNINST_KEY}" "DisplayName"     "${APP_NAME} ${APP_VERSION}"
    WriteRegStr HKCU "${APP_UNINST_KEY}" "UnInstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKCU "${APP_UNINST_KEY}" "DisplayIcon"     "$INSTDIR\${APP_NAME_LOW}.exe"
    WriteRegStr HKCU "${APP_UNINST_KEY}" "Publisher"       "${APP_DOMAIN}"
    WriteRegStr HKCU "${APP_UNINST_KEY}" "URLInfoAbout"    "${APP_SITE}"
    WriteRegStr HKCU "${APP_UNINST_KEY}" "DisplayVersion"  "${APP_VERSION}"
    WriteRegStr HKCU "${APP_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  ${EndIf}

  ${If} $update == true
    Banner::destroy
    Exec '"$INSTDIR\${APP_NAME_LOW}.exe"'
  ${EndIf}
SectionEnd


Section "$(SEC_DESKTOP)" SecDesktop
  ${If} $update == false
    CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_NAME_LOW}.exe" "" "" "" "" "" "${APP_NAME} ${APP_VERSION}"
  ${EndIf}
SectionEnd

Section "$(SEC_STARTUP)" SecStartup
  ${If} $update == false
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${APP_NAME}" "$INSTDIR\${APP_NAME_LOW}.exe"
  ${EndIf}
SectionEnd


!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(DESC_CORE)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} $(DESC_DESKTOP)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartup} $(DESC_STARTUP)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function .onInit
  ${GetParameters} $R0
  ${GetOptionsS} $R0 "-update" $R0
  ${Unless} ${Errors}
    ${GetParent} "$EXEDIR" $R0
    StrCpy $INSTDIR $R0
    StrCpy $update true
    SetSilent silent
  ${Else}
    StrCpy $update false
  ${EndUnless}

  !insertmacro MUI_LANGDLL_DISPLAY

  ${If} $update == true
    ${If} $LANGUAGE == 1049
      Banner::show /NOUNLOAD /set 76 "Идёт обновление ${APP_NAME}." "Пожалуйста подождите."
    ${Else}
      Banner::show /NOUNLOAD /set 76 "Updating ${APP_NAME}. Please wait." ""
    ${EndIf}
  ${EndIf}
FunctionEnd


Section "Uninstall"

  DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${APP_NAME}"
  
  !insertmacro KILL_ALL "$INSTDIR\${APP_NAME_LOW}.exe"

  RMDir "$SMPROGRAMS\${APP_NAME}"
  
  Delete "$DESKTOP\${APP_NAME}.lnk"

  Delete "$INSTDIR\libeay32.dll"
  Delete "$INSTDIR\ssleay32.dll"
  Delete "$INSTDIR\${APP_NAME_LOW}.exe"
  Delete "$INSTDIR\uninstall.exe"
  
  RMDir "$INSTDIR"

  DeleteRegKey HKCU "${APP_UNINST_KEY}"
  DeleteRegKey /ifempty HKCU "${APP_REGKEY}"
SectionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE 
FunctionEnd

Function updatePre
  ${If} $update == true
    Abort
  ${EndIf}
FunctionEnd
