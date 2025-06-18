; Installer for Full Build Folder
Name "Tic Tac Toe Game"
OutFile "TicTacToeSetup.exe"
InstallDir "$PROGRAMFILES\TicTacToeGame"
RequestExecutionLevel admin
Unicode True

Icon "C:\Users\miroh\Documents\QtProjects\Advanced_TicTacToe\tictactoe.ico"

!include "MUI2.nsh"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_TITLE "Installation Complete"
!define MUI_FINISHPAGE_TEXT "Tic Tac Toe Game has been successfully installed."
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchApplication"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Tic Tac Toe Game"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "Install Game"

  SetOutPath "$INSTDIR"
  
  ; Copy full build folder recursively
  File /r "C:\Users\miroh\Documents\QtProjects\Advanced_TicTacToe\build\Desktop_Qt_6_9_0_MinGW_64_bit-Debug\*"

  ; Uninstaller Info
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TicTacToeGame" \
                   "DisplayName" "Tic Tac Toe Game"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TicTacToeGame" \
                   "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

Function LaunchApplication
  ExecShell "open" "$INSTDIR\debug\Advanced_TicTacToe.exe"
FunctionEnd

Section "Uninstall"

  RMDir /r "$INSTDIR"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TicTacToeGame"

SectionEnd
