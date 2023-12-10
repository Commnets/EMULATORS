; License Generator Installer
; Copyright 2013 by Community Networks S.L.
; Ignacio Cea Forniés

#define MyAppName "VIC20Emulator"
#define MyAppExeName "VIC20Emulator.exe"
#define MyProductName "Community Networks VIC20Emulator"
#define MyProductInfo "Community Networks VIC20Emulator"
#include "..\Commons\Commons.iss"

[Setup]
AppId={{2B5ED34E-1866-4519-89C6-86711509E171}
SetupIconFile=VIC20Logo.ico
UninstallDisplayIcon={app}\VIC20Logo.ico

[Files]
Source: "VIC20EmulatorC.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "VIC20Emulator.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "RVIC20Console.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "6500Compiler.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "Test6500.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "basic.901486-01.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "characters.901460-03-ENG.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "kernal.901486-07-ENG.bin"; DestDir: "{app}"; Flags: ignoreversion
Source: "VIC20.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "Commodore.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "LocalConsole.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "Standard.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "console.def"; DestDir: "{app}"; Flags: ignoreversion
Source: "VIC20.fmt"; DestDir: "{app}"; Flags: ignoreversion
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
