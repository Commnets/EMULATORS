<b>Framework for Classic EMULATORS</b><br>
Develop By <b>Ignacio Cea Forniés</b>. Copyright (C) 2023.<br>
Used allowed under GPL license.<br>
The framework has been developed fully in C++ (version 20) using <a href="https://visualstudio.microsoft.com/es/vs/">Visual Studio Community 2022 (64bits)</a>.<br>
<br>
For better understanding of the framework unzip <b>html.zip</b>, included in the package.<br>
The package includes classes to develop you own emulator, including also assembler, compiler and consoles, remote and local ones.
<br>
The implementation of the framework for Commodore 64 has been included.<br>
In that case, the behaviour of the VICII has been emulated following the fantastic article by Christian Bauer that can be found at
<a href="http://www.zimmers.net/cbmpics/cbm/c64/vic-ii.txt">Zimmers.net</a><br>
<br>
To compile the framework the <a href="https://github.com/libsdl-org/SDL/releases/tag/release-2.26.2">SDL Core</a>, <a href="https://www.libsdl.org/projects/SDL_image/release/">SDL image</a>, <a href="https://github.com/libsdl-org/SDL_mixer">SDL Mixer</a> and <a href="https://github.com/facebookarchive/RakNet">Raknet</a> libraries are necessary.
Download them, install them and set the environmental variables: <b>SDL_HOME</b>, <b>SDL_IMAGEHOME</b>, <b>SDL_MIXERHOME</b>, <b>RAKNET_HOME</b>
<br><br>
<img src="./docs/C64Data/Picture1.png"/><br><br>
The important C64 keys in a Win32 standard keyboard are:<br>
LEFT CONTROL         = CTRL<br>
RIGHT CONTROL        = CTRL<br>
LEFT ALT             = STOP<br>
SHIFT + LEFT ALT     = RUN<br>
PAUSE                = COMMODORE KEY<br>
LEFT BRACKET         = +<br>
RIGHT BRACKET        = *<br>
APOSTROPHE           = :<br>
BACKSLASH            = @<br>
ESCAPE               = ARROW BACK<br>
GRAVE                = POUND<br>
