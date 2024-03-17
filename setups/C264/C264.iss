; License Generator Installer
; Copyright 2013 by Community Networks S.L.
; Ignacio Cea Forniés

#define MyAppName "C264Emulator"
#define MyAppExeName "C264Emulator.exe"
#define MyProductName "Community Networks VIC20Emulator"
#define MyProductInfo "Community Networks VIC20Emulator"
#include "..\Commons\Commons.iss"

[Setup]
AppId={{BB21CEFB-9F1C-49BE-8C3A-CF2474F2590C}
SetupIconFile=C264Logo.ico
UninstallDisplayIcon={app}\C264Logo.ico

[Files]
Source: "C264EmulatorC.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C264Emulator.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "R264Console.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "6500Compiler.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "Test6500.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "basic.318006-01.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "kernal.318004-05-ENG.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "kernal.318005-05-FRA.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "kernal.318030-02-HUN.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "kernel.325155-03-SWE.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\CP4\3-plus-1.317053-01.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\CP4\3-plus-1.317054-01.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "C264.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "Commodore.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "LocalConsole.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "Standard.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "console.def"; DestDir: "{app}"; Flags: ignoreversion
Source: "C264.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "Commodore.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "LocalConsole.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "Standard.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "Default.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "blkFormatter.fmt"; DestDir: "{app}"; Flags: ignoreversion

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Run]

[Dirs]

[CustomMessages]

[Icons]

[Code]
{ Begin of code }

{ To install the special pages used by this install program.
  Those pages are to ask for information the installation needs }
Procedure createCustomWizardPages;
Begin
  
End;

{ The Initialize Wizard is an event launched
  when the setup program starts. This situation
  is here used to add special wizards to ask for the
  information that the server installation needs. }
Procedure InitializeWizard;
Begin

  CreateAboutButtonAndURLLabel (WizardForm, WizardForm.CancelButton);
  
  { 
    addLicensePage;
    This page defined in Commons.iss, 
    but it's been disabled in this installation.
  }
  
  {
    addDataDirPage;
    This page defined in Commons.iss, 
    but it's been disabled in this installation.
  }

  {
    addScalePage;
    This page defined in Commons.iss, 
    but it's been disabled in this installation.
  }

  createCustomWizardPages;

End;

{ To validate the local data introduced.
  This method is called from the common.iss page, in the NexButtonClick Event.
  Depending on the page you are, the data are validate.
  Once all data is valid, it is completed if needed. }
Function NextButtonClick (curPageId: Integer) : Boolean;
Var
  _message : String;

Begin

  _message := '';

  { This versión doesn't consider asking for a data page
    So this part of the code has been commented 
    Else
    If (curPageId = _dataDirPage.ID) Then
      _message := validateDataDirAtNextButtonClick ();
   }

  { This versión doesn't consider asking for a scale page
    So this part of the code has been commented 
    Else
    If (curPageId = _scalePage.ID) Then
      _message := validateScaleAtNextButtonClick ();
   }
  
  { This versión doesn't consider asking for a license file
    So this part of the code has been commented 
    Else
    If (curPageId = _licensePage.ID) Then
      _message := validateLicenseFileAtNextButtonClick ();
  }

  If (_message <> '') Then
  Begin
    MsgBox (_message, mbError, MB_OK);
    Result := False;
  End
  Else
    Result := True;

End;

{ Event shoot when the instalation of Conf.xml finishes. }
Procedure configureConf;
Begin

  { Configure the the data dir }
  changeTextInFile (ExpandConstant('{app}')+'\Conf.xml', '@@DATA_DIR@@', 
    _dataDirPage.Values [0]);

  { Configure the scale of the graphics system }
  changeTextInFile (ExpandConstant('{app}')+'\Conf.xml', '@@SCALE@@', 
    _scalePage.Values [0]);

End;

{ End of code }
