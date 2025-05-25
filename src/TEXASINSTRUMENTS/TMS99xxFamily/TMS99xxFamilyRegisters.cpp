#include <TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamilyRegisters.hpp>

// ---
MCHEmul::Strings TEXASINSTRUMENTS::TMS99xxFamilyRegisters::SpriteDefinition::spriteDrawSnapShot () const
{
	MCHEmul::Strings result = { };

	auto toString = [](const MCHEmul::UByte& b) -> std::string
		{
			std::string result = "";
			for (int i = 7; i > 0; i--) result += b.bit (i) ? "*" : ".";
			return (result);
		};

	if (_16pixels)
		for (size_t i = 0; i < 16; i++) 
			result.emplace_back (toString (_data [i]) + toString (_data [i + 16]));
	else
		for (size_t i = 0; i < 8; i++)
			result.emplace_back (toString (_data [i]));

	return (result);
}

// ---
MCHEmul::InfoStructure TEXASINSTRUMENTS::TMS99xxFamilyRegisters::SpriteDefinition::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ID",			((int) _id) + 1); // From 1 to 32, not from 0 to 31.
	result.add ("POSX",			(int) _posX);
	result.add ("POSY",			(int) _posY);
	result.add ("PATTERN",		(int) _pattern);
	result.add ("COLOR",		_color);
	result.add ("16PIXELS",		_16pixels);
	result.add ("ENLARGED",		_enlarged);
	result.add ("EARLYCLOCK",	_earlyClock);
	result.add ("BYTES",		_data);

	return (result);
}

// ---
TEXASINSTRUMENTS::TMS99xxFamilyRegisters::TMS99xxFamilyRegisters (size_t vS)
	: MCHEmul::ChipRegisters (TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_ID,
		2 /** bytes per address. */, 2 /** registers. */),
	  _videoMemory (vS, MCHEmul::UByte::_0),
	  _controlRegisters (8, MCHEmul::UByte::_0),
	  _lastValueRead (MCHEmul::UByte::_0)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	// The memory cannot have less than 16k...
	assert (_videoMemory.size () >= 0x4000);

	initializeInternalValues ();
}

// ---
TEXASINSTRUMENTS::TMS99xxFamilyRegisters::TMS99xxFamilyRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s, size_t vS)
	: MCHEmul::ChipRegisters (TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_ID, ps, pp, a, s),
	  _videoMemory (vS, MCHEmul::UByte::_0),
	  _controlRegisters (8, MCHEmul::UByte::_0)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	// The memory cannot have less than 16k...
	assert (_videoMemory.size () >= 0x4000);

	initializeInternalValues ();
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamilyRegisters::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure TEXASINSTRUMENTS::TMS99xxFamilyRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("MEMSIZE",				std::to_string (_videoMemory.size ()));
	result.add ("GRAPHICMODE",			_graphicMode);
	result.add ("EXTERNALVIDEO",		_externalVideo);
	result.add ("16KVIDEO",				_16k);
	result.add ("BLANKSCREEN",			_blankScreen);
	result.add ("SPRITES16PIXELS",		_sprites16pixels);
	result.add ("SPRITESENLARGED",		_spritesEnlarged);
	result.add ("LAUNCHINTERRUPT",		_launchScreenUpdateInterrupt);
	result.add ("NAMEADDRESS",			_nameAddress);
	result.add ("COLORADDRESS",			_colorAddress);
	result.add ("PATTERNADDRESS",		_patternAddress);
	result.add ("SPRITEATTRSADDRESS",	_spriteAttrsAddress);
	result.add ("SPRITEGENADDRESS",		_spriteGenAddress);
	result.add ("TEXTCOLOR",			_textColor);
	result.add ("BACKDROPCOLOR",		_backdropColor);
	result.add ("COLLISION",			_spriteCollisionDetected);
	result.add ("FIFTHSPRITE",			_fifthSpriteDetected);
	result.add ("FIFTHSPRITENOTDRAWN",	_fifthSpriteNotDrawn);
	result.add ("VDPSpritesInfo",		std::move (getSpritesInfoStructure ()));
	// The video RAM is 16k long. But it is allowed to print it out...
	result.add ("MEMSIZE",				(unsigned int) _videoMemory.size ());
	result.add ("BYTES",				_videoMemory);

	return (result);
}

// ---
MCHEmul::InfoStructure TEXASINSTRUMENTS::TMS99xxFamilyRegisters::getSpritesInfoStructure
	(const std::vector <size_t>& nS) const
{
	MCHEmul::InfoStructure result;
	std::vector <size_t> rnS = nS;
	if (nS.empty ()) 
		for (size_t i = 1; i <= 32; rnS.push_back (i++)); // The 32 if nothing...
	
	size_t ct = 0;
	MCHEmul::InfoStructure sInfo;
	for (const auto& i : rnS)
		if (i > 0 && i <= 32) // Between 1 and 32...
			sInfo.add (MCHEmul::fixLenStr (std::to_string (ct++), 3 /** Enough. */, true, MCHEmul::_CEROS),
				std::move (readSpriteDefinition ((unsigned char) (i - 1) /** Here is from 0 to 31. */).getInfoStructure ()));
	result.add ("VDPSpritesInfo", std::move (sInfo));

	return (result);
}

// ---
MCHEmul::Strings TEXASINSTRUMENTS::TMS99xxFamilyRegisters::spritesDrawSnapShot (const std::vector <size_t>& nS) const
{
	MCHEmul::Strings result;

	std::vector <size_t> rnS = nS;
	if (nS.empty ()) for (unsigned char i = 1; i <= 32; rnS.push_back (i++)); // The 32 if nothing...
	for (const auto& i : rnS)
	{
		result.emplace_back ("-Sprite:" + std::to_string ((int) i) + "-----");
		MCHEmul::Strings sS = std::move (spriteDrawSnapShot (i));
		result.insert (result.end (), sS.begin (), sS.end ());
	}

	return (result);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamilyRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in TMS99xxFamily");

		return;
	}

	// Keeps the value in the internal physical storage...
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	switch (p)
	{
		case 0x00:
			{
				// The value written is also stored in the buffer ahead....
				// The time to set / read a data to / from the VRAM will depend on whether 
				// the Chip is or not in a "CPU window", that is a moment where it is not reading info from the VRAM
				// The size of that window will depend on two things: where the raster line is and the type of graphics more.
				// That time can vary between 2us and 8us (in the visual screen and in the graphics mode I or II and with the sprites on)
				// 8us are 14 clock cycles in a MSX1, what is a Z80 instruction later...
				// Note that this delay is not emulated!
				setVideoData (_readWriteAddress, _readAheadBuffer = v);
				// ...and then the position is incremented...
				incrementReadWriteAddress ();
			}

			break;

		case 0x01:
			{
				// Has the port 0x99 already been accessed once before?
				if (_99setOnce)
				{
					_readWriteAddress = MCHEmul::Address (2, 
						((unsigned int) (v.value () & 0x3f) << 8) | 
						 (unsigned int) _99firstAccessValue.value ());

					// Accessing to a register...
					if (v.bit (7))
						setControlRegister 
							(v.value () & 0x07 /** From 0 to 7: Only matters bits 0, 1 & 2. */, _99firstAccessValue);
					// or accessing to the video memory RAM...
					else
					{
						// Winting to set or to read the Video RRAM?
						if (!v.bit (6)) // ...reading
						{
							// The info is put a internal buffer that is read through the register 0...
							_readAheadBuffer = videoData (_readWriteAddress);
							// ...and just after reading the position is incremented...
							incrementReadWriteAddress ();
						}
					}

					_99setOnce = false; // Starts back...
				}
				// os is it the first time?
				else
				{
					_99setOnce = true;

					// ...keep the value because the real instruction...
					// ...will be mandated in the second access...
					_99firstAccessValue = v;
				}
			}

			break;

		// It shouldn't be here...
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& TEXASINSTRUMENTS::TMS99xxFamilyRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in TMS99xxFamily");

		return (_lastValueRead = result);
	}

	switch (p)
	{
		case 0x00:
			{
				// Return the value in the buffer...
				result = _readAheadBuffer;
				// ...read a new value (the position eill have been incremented when st for reading...
				// Read the value from the VRAM has delay too...
				// Read the detailed information written for the setValue method!
				_readAheadBuffer = videoData (_readWriteAddress);
				// ...and then increment looking for a bew position...
				incrementReadWriteAddress ();
			}

			break;

		case 0x01:
			{
				// Reading the statis several insternal status will be actualized...
				result = readStatus ();

				// Reading the status, the info is again ready to be read!
				_99setOnce = false; 
			}

			break;

		// It shouldn't be here...
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
const MCHEmul::UByte& TEXASINSTRUMENTS::TMS99xxFamilyRegisters::peekValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in TMS99xxFamily");

		return (_lastValueRead = result);
	}

	switch (p)
	{
		case 0x00:
			{
				// Return the value in the buffer...
				result = _readAheadBuffer;
			}

			break;

		case 0x01:
			{
				// Reading the statis several insternal status will be actualized...
				result = peekStatus ();
			}

			break;

		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamilyRegisters::initializeInternalValues ()
{
	// Controlling attributes...
	_graphicMode							= 0;
	_externalVideo							= false;
	_16k									= true;
	_blankScreen							= false;
	_sprites16pixels						= true;
	_spritesEnlarged						= false;
	_launchScreenUpdateInterrupt			= false;
	MCHEmul::Address _nameAddress			= { };
	MCHEmul::Address _colorAddress			= { };
	MCHEmul::Address _patternAddress		= { };
	MCHEmul::Address _spriteAddress			= { };
	MCHEmul::Address _spriteAttrsAddress	= { };
	MCHEmul::Address _spriteGenAddress		= { };
	unsigned char _textColor				= 0x00;
	unsigned char _backdropColor			= 0x00;
	for (size_t i = 0; i < 8; 
		_controlRegisters [i++] = MCHEmul::UByte::_0);

	// Running attributes modified from the chip...
	// That are part of the status...
	_spriteCollisionDetected				= false;
	_fifthSpriteDetected					= false;
	_fifthSpriteNotDrawn					= 0x00;
	_screenUpdateHappen						= false;

	// Variables set when the different registers are accesed...
	_99setOnce								= false;
	_99firstAccessValue						= MCHEmul::UByte::_0;
	_readWriteAddress						= { };
	_readAheadBuffer						= MCHEmul::UByte::_0;

	// Iniatialize the video memory...
	for (size_t i = 0; i < _videoMemory.size (); 
		_videoMemory [i++] = MCHEmul::UByte::_0);

	// The temnporal info used when running...
	_dataRead = false;
	_lastPatternNameTablePositionRead = { };
	_lastPatterNameValueRead = MCHEmul::UByte::_0;
	_lastPatterGeneratorTablePositionRead = { };
	_lastPatternGeneratorValueRead = MCHEmul::UByte::_0;
	_lastColorTablePositionRead = { };
	_lastColorValueRead = MCHEmul::UByte::_0;
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamilyRegisters::setControlRegister (unsigned char rId, const MCHEmul::UByte& v)
{
	_controlRegisters [rId] = v;

	switch (rId)
	{
		// Control Register 1
		// Graphic mode
		case 0x00:
			{
				// Bit 0: External signal?
				_externalVideo = v.bit (0);

				// Bit 1: Changing graphic mode.
				MCHEmul::UByte gM = MCHEmul::UByte::_0;
				gM.setBit (0, _controlRegisters [1].bit (4));
				gM.setBit (1, _controlRegisters [0].bit (1));
				gM.setBit (2, _controlRegisters [1].bit (3));
				setGraphicMode (gM.value ());

				// The rest of the bits are not used...
			}

			break;

		// Control Register 2
		// Graphic mode, Sprites, Interrupt...
		case 0x01:
			{
				// Bit 0: Sprites enlarged in both axis?
				_spritesEnlarged = v.bit (0);

				// Bit 1: Sprites 16 x 16 or 8 x 8 (before being enlarged)
				_sprites16pixels = v.bit (1);

				// Bit 2 is not used.

				// Bits 3 & 4: Changing graphic mode (bits 3 y 4 are used here. Notice the sequence)
				MCHEmul::UByte gM = MCHEmul::UByte::_0;
				gM.setBit (0, _controlRegisters [1].bit (4));
				gM.setBit (1, _controlRegisters [0].bit (1));
				gM.setBit (2, _controlRegisters [1].bit (3));
				setGraphicMode (gM.value ());

				// Bit 5: Generates interrupt when screen is over?
				_launchScreenUpdateInterrupt = v.bit (5);

				// Bit 6: Blank screen?
				_blankScreen = !v.bit (6); // True if set!

				// Bit 7: 16K in the screen memory (o just 4)?
				// In TMS99xxFamily TEXASINSTRUMENTS has no effect because it has always 16k...
				_16k = v.bit (7);
			}

			break;

		// Pattern Name Table Address
		case 0x02:
			{
				// Bits 0, 1, 2 & 3: Where the table of codes (patterns' name) is (blocks of 1k)...
				_nameAddress = MCHEmul::Address (2, (v.value () & 0x0f) << 10);

				// The rest of the bits are not used
			}

			break;

		// Color Name Table Address
		case 0x03:
			{
				// Special meaning when bit 1 register 0 (M2) is on...
				_colorAddress = MCHEmul::Address (2, v.value () << 6);
			}

			break;

		// Pattern Generation Table Address
		case 0x04:
			{
				// Bits 0, 1 & 2: Where the patters are (blocks of 2k)...
				_patternAddress = MCHEmul::Address (2, (v.value () & 0x07) << 11);
			}

			break;

		// Sprite Attributes Table Address
		case 0x05:
			{
				// Bits 0 to 6: Where the sprites' attribute table is (blocks of 128 bytes)...
				_spriteAttrsAddress = MCHEmul::Address (2, (v.value () & 0x7f) << 7);
			}

			break;

		// Sprite Generation Table Address
		case 0x06:
			{
				// Bits 0, 1 & 3: Where the definition of the sprites is (blocks of 2k)...
				_spriteGenAddress = MCHEmul::Address (2, (v.value () & 0x07) << 11);
			}

			break;

		// Colors
		case 0x07:
			{
				// Bits 0, 1, 2 & 3: Back drop (background color)
				_backdropColor = v.value () & 0x0f;

				// Bits 4, 5, 6 & 7: Text color
				_textColor = (v.value () & 0xf0) >> 4;
			}

			break;

		default:
			break;
	}
}

// ---
MCHEmul::UByte TEXASINSTRUMENTS::TMS99xxFamilyRegisters::readStatus () const
{
	MCHEmul::UByte result = peekStatus ();

	// After reading, the folowwing bit are moved back to false...
	_spriteCollisionDetected = false;
	_screenUpdateHappen = false;

	return (result);
}

// ---
MCHEmul::UByte TEXASINSTRUMENTS::TMS99xxFamilyRegisters::peekStatus () const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// The first 5 bits make only sense when the situation happen...
	result |= _fifthSpriteDetected ? (_fifthSpriteNotDrawn & 0x1f) : 0x00;
	result.setBit (5, _spriteCollisionDetected);
	result.setBit (6, _fifthSpriteDetected);
	result.setBit (7, _screenUpdateHappen);

	return (result);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamilyRegisters::setGraphicMode (unsigned char gM)
{
	_graphicMode = gM;

	_dataRead = false;

	_lastPatternNameTablePositionRead = { };
	_lastPatterNameValueRead = MCHEmul::UByte::_0;
	_lastPatterGeneratorTablePositionRead = { };
	_lastPatternGeneratorValueRead = MCHEmul::UByte::_0;
	_lastColorTablePositionRead = { };
	_lastColorValueRead = MCHEmul::UByte::_0;
}

// ---
std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte> 
	TEXASINSTRUMENTS::TMS99xxFamilyRegisters::readGraphicInfo (unsigned short x, unsigned short y)
{
	switch (_graphicMode)
	{
		/** Graphics I mode. \n
			32 * 24 + Sprites. \n
			Every character is 8 pixels width. */
		case _GRAPHICIMODE:
			{
				MCHEmul::Address pT = 
					_nameAddress + 
					(size_t) (((y >> 3 /** 8 bytes high every pattern. */) << 5 /** 32 patterns per line. */) + 
					(size_t) (x >> 3 /** 8 bits used per line of pattern. */)); // 768 positions max...

				// If the position to read is "new" or nothing was read before, read the data...
				if (pT != _lastPatternNameTablePositionRead || !_dataRead)
				{
					_lastPatterNameValueRead = 
						videoData (_lastPatternNameTablePositionRead = pT); // a pattern bwteen 0 and 255...
					_lastPatternGeneratorValueRead =
						videoData (_lastPatterGeneratorTablePositionRead = 
							// The base...
							_patternAddress +
							// ...8 bytes per char in the pattern table...
							(size_t) (_lastPatterNameValueRead.value () << 3) + 
							// ...and the right byte within that 8, will depend on the line where the raster is now...
							(size_t) (y % 8)); // Total 2048 bytes...
					_lastColorValueRead =
						videoData (_lastColorTablePositionRead =
							// The base...
							_colorAddress +	
							// 32 possible colors depending on the char name...
							(size_t) (_lastPatterNameValueRead.value () >> 3));
				}

				_dataRead = true;
			}

			break;

		/** Text mode. \n
			40 * 24 - Sprites. \n
			Every character is 6 pixels width. */
		case _TEXTMODE:
			{
				// In this mode the first and the last 8 pixels per line are blank...
				// ...and they have to be drawn in the same color than the background...
				if (x < 8 || x >= 248 /** (40 * 6) + first 8 pixels = 248. */)
				{
					_lastPatternNameTablePositionRead =
					_lastPatterGeneratorTablePositionRead =
					_lastColorTablePositionRead = { };
					_lastPatterNameValueRead = 
					_lastPatternGeneratorValueRead =
					_lastColorValueRead = MCHEmul::UByte::_0;
				}
				else
				{
					MCHEmul::Address pT = 
						_nameAddress + 
						(size_t) ((y >> 3 /** 8 bytes high every pattern. */) * 40 /** 32 patterns per line. */) + 
						(size_t) ((x - 8 /** From 1st position onwards. */) / 6 /** 6 bits used per line of pattern. */); // 960 in total...

					// If the position to read is "new" or nothing was read before, read the data...
					if (pT != _lastPatternNameTablePositionRead || !_dataRead)
					{
						_lastPatterNameValueRead = 
							videoData (_lastPatternNameTablePositionRead = pT);
						_lastPatternGeneratorValueRead =
							videoData (_lastPatterGeneratorTablePositionRead = 
								// The base...
								_patternAddress + 
								// 8 bytes per pattern (name selects the pattern)...
								(size_t) (_lastPatterNameValueRead.value () << 3) + 
								// The right byte of the pattern depends on the screen position...
								(size_t) (y % 8)); // Total 2048 bytes...
						// Only the first 6 bits are used...
						// ...and the color table in this case is not used...
						_lastColorTablePositionRead = { }; // Not important...
						_lastColorValueRead = MCHEmul::UByte::_0;
					}

					_dataRead = true;
				}
			}

			break;

		/** Multicolor mode. \n
			64 * 48 (blocks of 4 * 4 pixels) + Sprites. 
			Just to draw graphics of colors. */
		case _MULTICOLORMODE:
			{
				MCHEmul::Address pT = 
					_nameAddress + 
					(size_t) (((y >> 3 /** 8 bytes high every pattern. */) << 5 /** 32 patterns per line. */) + 
					(size_t) (x >> 3 /** 8 bits used per line of pattern. */)); // 768 positions max...

				// If the position to read is "new" or nothing was read before, read the data...
				if (pT != _lastPatternNameTablePositionRead || !_dataRead)
				{
					unsigned char row = (unsigned char) (y >> 3); // From 0 to 23...
					_lastPatterNameValueRead = 
						videoData (_lastPatternNameTablePositionRead = pT);
					// The pattern name is used to get the color pattern that is stored in the pattern generation table.
					// Every color pattern is made up of 8 bytes and there is 2 colors defined in each byte
					_lastPatternGeneratorValueRead =
						videoData (_lastPatterGeneratorTablePositionRead =
							// The base...
							_patternAddress +
							// 8 bytes per pattern (name selects the color pattern)...
							(size_t) (_lastPatterNameValueRead.value () << 3)); // Total 2048 bytes...
					// But the specific byte to select will depend on, the row where the pattern name was initially read (768 positions)
					// So if the row were 0, the byte selected would be 0 & 1, row 1 -> bytes 2 & 3, row 2 -> bytes 4 & 5, row 3 -> bytes 6 & 7...
					// So the byte to select is (row AND 3) << 1 & ((row AND 3) << 1) + 1
					// The 1 added at the end will depend of the raster row: 0 - 3 = +0, 4 - 7 = +1, 8 - 11 = +0, 12 - 15 = +1,...
					// So (rasterRow >> 2) % 2 is what is has to be added...
					_lastColorTablePositionRead = { }; // Is wasn't used...
					_lastColorValueRead = 
						_lastPatternGeneratorValueRead [((row & 0x03) << 1) + ((y >> 2) % 2) /** Even = +0, Odd = +1. */];
				}

				_dataRead = true;
			}

			break;

		/** Graphics II mode. \n
			Similar to Graphics mode 1, 
			but with a much wider set of patterns and colors to select. \n
			32 * 24 + Sprites.
			Every character is 8 pixels width. */
		case _GRAPHICIIMODE:
			{
				size_t sP = 
					(size_t) ((y >> 3 /** 8 bytes high every pattern. */) << 5 /** 32 patterns per line. */) + 
					(size_t) (x >> 3 /** 8 bits used per line of pattern. */); // 768 different positions max...
				MCHEmul::Address pT = _nameAddress + sP;

				// If the position to read is "new" or nothing was read before, read the data...
				if (pT != _lastPatternNameTablePositionRead || !_dataRead)
				{
					// 0, 1 or 2. clock of 256 bytes in the screen...
					unsigned char bP = (unsigned char) (sP >> 8); 
					_lastPatterNameValueRead = 
						videoData (_lastPatternNameTablePositionRead = pT); // a pattern bwteen 0 and 255...
					_lastPatternGeneratorValueRead =
						videoData (_lastPatterGeneratorTablePositionRead = 
							// The base...
							_patternAddress +
							// ...a different block of 2048 bytes depending on the section in the screen (<< 11 = *0x0800 = 2048)
							(size_t) (bP << 11) +
							// ...8 bytes per char in the pattern table...
							(size_t) (_lastPatterNameValueRead.value () << 3) +
							// ...and the right byte within that 8, will depend on the line where the raster is now...
							(size_t) (y % 8)); // Total 2048 * 3 = 6144 bytes
					_lastColorValueRead =
						videoData (_lastColorTablePositionRead =
							// The base...
							_colorAddress +
							// ...a different block of 2048 bytes depending on the section in the screen (<< 11 = *0x0800 = 2048)
							(size_t) (bP << 11) +
							// ...8 bytes per char in the color table...
							(size_t) (_lastPatterNameValueRead.value () << 3) +
							// ...and the right byte within that 8, will depend on the line where the raster is now...
							(size_t) (y % 8)); // Total 2048 * 4 = 6144 bytes
				}

				_dataRead = true;
			}

			break;

		// This mode is not allowed...
		default:
			{
				_LOG ("Graphic mode:" + std::to_string (_graphicMode) + " not implemented");

				assert (false);
			}

			break;
	}

	// Returns the info read if it was actualized...
	return (std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte> 
		{ _lastPatterNameValueRead, _lastPatternGeneratorValueRead, _lastColorValueRead });
}
