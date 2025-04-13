#include <TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamilyRegisters.hpp>

// ---
TEXASINSTRUMENTS::TMS99xxFamilyRegisters::TMS99xxFamilyRegisters (size_t vS)
	: MCHEmul::ChipRegisters 
		(TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_ID, 
		 new MCHEmul::PhysicalStorage 
			(TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_ID, MCHEmul::PhysicalStorage::Type::_RAM, 2),
		 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 2),
	  _videoMemory (vS, MCHEmul::UByte::_0),
	  _controlRegisters (8, MCHEmul::UByte::_0),
	  _internalPhysicalMemory (nullptr),
	  _lastValueRead (MCHEmul::UByte::_0)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	// The memory cannot have less than 16k...
	assert (_videoMemory.size () >= 0x4000);

	_internalPhysicalMemory = physicalStorage ();

	initializeInternalValues ();
}

// ---
TEXASINSTRUMENTS::TMS99xxFamilyRegisters::TMS99xxFamilyRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s, size_t vS)
	: MCHEmul::ChipRegisters (TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_ID, ps, pp, a, s),
	  _videoMemory (vS, MCHEmul::UByte::_0),
	  _controlRegisters (8, MCHEmul::UByte::_0),
	  _internalPhysicalMemory (nullptr)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	// The memory cannot have less than 16k...
	assert (_videoMemory.size () >= 0x4000);

	initializeInternalValues ();
}

// ---
TEXASINSTRUMENTS::TMS99xxFamilyRegisters::~TMS99xxFamilyRegisters ()
{
	// It could be nullptr, if it wouldn't have been created internally...
	delete (_internalPhysicalMemory);
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

	result.add ("MemSize",				std::to_string (_videoMemory.size ()));
	result.add ("GraphicMode",			_graphicMode);
	result.add ("ExternalVideo",		_externalVideo);
	result.add ("16kVideo",				_16k);
	result.add ("BlankScreen",			_blankScreen);
	result.add ("Sprites16pixels",		_sprites16pixels);
	result.add ("SpritesEnlarged",		_spritesEnlarged);
	result.add ("LaunchInterrupt",		_launchScreenUpdateInterrupt);
	result.add ("NameAddress",			_nameAddress);
	result.add ("ColorAddress",			_colorAddress);
	result.add ("PatternAddress",		_patternAddress);
	result.add ("SpriteAddress",		_spriteAddress);
	result.add ("SpriteAttrsAddress",	_spriteAttrsAddress);
	result.add ("SpriteGenAddress",		_spriteGenAddress);
	result.add ("TextColor",			_textColor);
	result.add ("BackdropColor",		_backdropColor);
	result.add ("BYTES",				_videoMemory);

	return (result);
}

// ---
MCHEmul::UBytes TEXASINSTRUMENTS::TMS99xxFamilyRegisters::screenMemorySnapShot () const
{
	// TODO

	return (MCHEmul::UBytes ());
}

// ---
MCHEmul::UBytes TEXASINSTRUMENTS::TMS99xxFamilyRegisters::colorMemorySnapShot () const
{
	// TODO

	return (MCHEmul::UBytes ());
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamilyRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 2;

	MCHEmul::PhysicalStorageSubset::setValue (pp, v);

	switch (pp)
	{
		case 0x00:
			{
				// The value written is also stored in the buffer ahead....
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

		default:
			break;
	}

}

// ---
const MCHEmul::UByte& TEXASINSTRUMENTS::TMS99xxFamilyRegisters::readValue (size_t p) const
{
	size_t pp = p % 2;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
	{
		case 0x00:
			{
				// Return the value in the buffer...
				result = _readAheadBuffer;
				// ...read a new value (the position eill have been incremented when st for reading...
				_readAheadBuffer = videoData (_readWriteAddress);
				// ...and then increment looking for a bew position...
				incrementReadWriteAddress ();
			}

			break;

		case 0x01:
			{
				// Reading the statis several insternal status will be actualized...
				result = readStatus ();
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
	size_t pp = p % 2;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
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
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamilyRegisters::setControlRegister (unsigned char rId, const MCHEmul::UByte& v)
{
	_controlRegisters [rId] = v;

	switch (rId)
	{
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

		case 0x02:
			{
				// Bits 0, 1, 2 & 3: Where the table of codes (patterns' name) is (blocks of 1k)...
				_nameAddress = MCHEmul::Address (2, (v.value () & 0x0f) << 10);

				// The rest of the bits are not used
			}

			break;

		case 0x03:
			{
				// Special meaning when bit 1 register 0 (M2) is on...
				_colorAddress = MCHEmul::Address (2, v.value () << 6);
			}

			break;

		case 0x04:
			{
				// Bits 0, 1 & 2: Where the patters are (blocks of 2k)...
				_patternAddress = MCHEmul::Address (2, (v.value () & 0x07) << 11);
			}

			break;

		case 0x05:
			{
				// Bits 0 to 6: Where the sprites' attribute table is (blocks of 128 bytes)...
				_spriteAttrsAddress = MCHEmul::Address (2, (v.value () & 0x7f) << 7);
			}

			break;

		case 0x06:
			{
				// Bits 0, 1 & 3: Where the definition of the sprites is (blocks of 2k)...
				_spriteGenAddress = MCHEmul::Address (2, (v.value () & 0x07) << 11);
			}

			break;

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
	// TODO
}
