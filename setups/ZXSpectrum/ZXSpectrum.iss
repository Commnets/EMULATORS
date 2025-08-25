; License Generator Installer
; Copyright 2013 by Community Networks S.L.
; Ignacio Cea Forniés

#define MyAppName "ZXSpectrumEmulator"
#define MyAppExeName "ZXSpectrumEmulator.exe"
#define MyProductName "Community Networks ZXSpectrumEmulator"
#define MyProductInfo "Community Networks ZXSpectrumEmulator"
#include "..\Commons\Commons.iss"

[Setup]
AppId={{259C2F33-B626-461F-BD63-DF727A6FDFF6}
SetupIconFile=ZXSpectrumLogo.ico
UninstallDisplayIcon={app}\ZXSpectrumLogo.ico

[Files]
Source: "ZXSpectrumEmulatorC.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "ZXSpectrumEmulator.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "RZXSpectrumConsole.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "Z80Compiler.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "TestZ80.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "ZXSpectrum.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "SINCLAIR.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "LocalConsole.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "Standard.hlp"; DestDir: "{app}"; Flags: ignoreversion
Source: "console.def"; DestDir: "{app}"; Flags: ignoreversion
Source: "ZXSpectrum.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "SINCLAIR.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "LocalConsole.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "Standard.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "Default.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "blkFormatter.fmt"; DestDir: "{app}"; Flags: ignoreversion
Source: "bios/48Standard-ENG.rom"; DestDir: "{app}/bios"; Flags: ignoreversion
Source: "bios/48Standard-ESP.rom"; DestDir: "{app}/bios"; Flags: ignoreversion
Source: "bios/48Standard-NOR.rom"; DestDir: "{app}/bios"; Flags: ignoreversion
Source: "bios/48Standard-ARA.rom"; DestDir: "{app}/bios"; Flags: ignoreversion
Source: "ZXSpectrumSysVars.txt"; DestDir: "{app}"; Flags: ignoreversion

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
