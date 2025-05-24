/** \ingroup TEXASINSTRUMENTS */
/*@{*/

/**	
 *	@file	
 *	File: TMS99xxFamilyRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 28/03/2024 \n
 *	Description: The registers of the TMS99xxFamily.
 *	Versions: 1.0 Initial
 *  Based on the standard (toshiba): https://www.cs.columbia.edu/~sedwards/papers/TMS9918.pdf
 */

#ifndef __TEXASINSTRUMENTS_TMS99XXFAMILYREGISTERS__
#define __TEXASINSTRUMENTS_TMS99XXFAMILYREGISTERS__

#include <CORE/incs.hpp>

namespace TEXASINSTRUMENTS
{
	/** The registers of the TMS99xxFamily chip. \n
		The registers include the video memory. \n
		That memory is not accesed ever from outside but only through the registers. \n
		The chip includes a register (16bits) that point out to a physical position in that memory. \n
		That position is automatically incremented when reading or writting values to the video RAM. */
	class TMS99xxFamilyRegisters : public MCHEmul::ChipRegisters
	{
		friend class TMS99xxFamily;

		public:
		/** To identify the attributes of a sprite. */
		struct SpriteDefinition
		{
			SpriteDefinition ()
				: _posX (0), _posY (0), 
				  _pattern (0), 
				  _color (0),
				  _16pixels (false), _enlarged (false),
				  _earlyClock (false),
				  _data ({ })
							{ }

			/** Return true if the sprite is visible at raster line rL. \n
				The raster line 0 happens at the beginning of the visible zone. \n
				Takes into account whether the sprite is expanded and it is 16 pixels wide or only 8. \n
				Returns also the line visible within the definition. If it is not visible that value makes no sense. \n
				The method receives also the size of the upper border (that will depend on the projection system). */
			inline bool visibleAtVisibleLine 
				(unsigned short vL, unsigned short uB, size_t& dF) const;
			/** Return true if the sprite is visible in the position x. \n
				The position y has the same reference than the position of the sprite. \n
				It take sinto account whether the early clock is set up or not. \n
				Returns also which is "bit" visible. It is not visible that value makes no sense. \n
				The methos receives also the size of the left border (that will depend on the projection system). */
			inline bool visibleAtVisiblePosition 
				(unsigned short vP, unsigned short lB, size_t& dP) const;

			/** To get the info of the sprite as a set of strings. */
			MCHEmul::Strings spriteDrawSnapShot () const;

			unsigned char _posX, _posY; // The position in the memory...
			unsigned char _pattern; // Block of info where to find the definition of the sprite
			unsigned int _color; // From the palette...
			bool _16pixels; // Whether it is 16 pixels wide definition or not...
			bool _enlarged; // Whether it is expanded of not...
			bool _earlyClock; // To show the sprite 32 pixels in advance inthe X axis...
			std::vector <MCHEmul::UByte> _data;
		};

		static const unsigned int _ID = 1200;

		/** The different graphic modes. */
		static const unsigned char _GRAPHICIMODE = 0;
		static const unsigned char _TEXTMODE = 1;
		static const unsigned char _MULTICOLORMODE = 2;
		static const unsigned char _GRAPHICIIMODE = 4;
		// The rest (up to 8) can be used but they have an non offical behaviour...

		// Constructors...
		/** This constructor is used when the registers will be used directly from the Chip
			and this one from the port manager. \n
			That is when the chip is working under a processor which devices are managed by ports. \n
			A temporal internal phisical memory is created and it is deleted when detroyed. \n
			@see attribute _internalPhysicalMemory */
		TMS99xxFamilyRegisters (size_t vS = 0x4000); // 16k of RAM...the minimum
		/** This constructor is used when the registers are used as an extension of the memory
			and they are accessed from outside. \n
			The physical memory is not owned by this class but the computer (memory class). */
		TMS99xxFamilyRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s, size_t vS = 0x4000); 

		/** This chip has only 2 registers valid: 0 & 1 */
		virtual size_t numberRegisters () const override
							{ return (2); }

		// Accesing to the important registers...
		/** Whether the system is or not active. */
		bool blankScreen () const
							{ return (_blankScreen); }
		/** The text color. */
		unsigned char textColor () const
							{ return (_textColor); }
		/** The background color. */
		unsigned char backDropColor () const
							{ return (_backdropColor); }

		// Manage the internal video memory of the chip...
		// Take care because no boundaries checks are done!
		size_t videoMemorySize () const
							{ return (_videoMemory.size ()); }
		const std::vector <MCHEmul::UByte>& videoMemory () const
							{ return (_videoMemory); }
		const MCHEmul::UByte& videoData (const MCHEmul::Address& pos) const
							{ return (_videoMemory [(size_t) pos.value ()]); }
		std::vector <MCHEmul::UByte> videoData (const MCHEmul::Address& pos, size_t nB) const
							{ return (std::vector <MCHEmul::UByte> 
								(_videoMemory.begin () + pos.value (), 
								 _videoMemory.begin () + pos.value () + nB /** The last is no included. */)); }
		void setVideoData (const MCHEmul::Address& pos, const MCHEmul::UByte& v)
							{ _videoMemory [(size_t) pos.value ()] = v; }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  *	MemSize				= Attribute: The size in bytes of the memry RAM
		  *	GraphicMode			= Attribute: The graphic mode in which the graphic is working
		  *	ExternalVideo		= Attribute: Is there external video to attend?
		  *	16kVideo			= Attribute: Is the 16k bytes long or just 4k?
		  *	BlankScreen			= Attribute: Blank screen active?
		  *	Sprites16pixels		= Attribute: Sprites 16 pixels width? (or just 8?)
		  *	SpritesEnlarged		= Attribute: Sprites enlarged?
		  *	LaunchInterrupt		= Attribute: Launch an interrupt when the screen is refreshed?
		  *	NameAddress			= Attribute: Memory address with the characteres to draw (used in some modes)
		  *	ColorAddress		= Attribute: Memory address where the color of the screen positions is defined.
		  *	PatternAddress		= Attribute: Memory address where the patterns to draw is located.
		  *	SpriteAddress		= Attribute: Memory address where the situation of the sprites is defined.
		  *	SpriteAttrsAddress	= Attribute: Memory address where the attributes of the sprites is located.
		  *	SpriteGenAddress	= Attribute: Memory address where the info to generate the sprites is located.
		  *	TextColor			= Attribute: Color of the text.
		  *	BackdropColor		= Attribute: Color of the background.
		  *	BYTES				= Attribute: Video RAM.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		// To get info about the VRAM...
		// No boundaries checks are done...
		/** To get the content of the pattern name table. \n
			The information finally retrieved will depend on the active graphical mode. */
		std::vector <MCHEmul::UByte> patternNameTableSnapShot () const
							{ return (videoData (_nameAddress, 
								(size_t) (_graphicMode == _TEXTMODE) ? (40 * 24) : (32 * 24))); }
		/** To get the information about the pattern generation table. \n
			The information finally collected will depend on the active graphical mode . */
		std::vector <MCHEmul::UByte> patternGenerationTableSnapShot () const
							{ return (videoData (_patternAddress, 
								(size_t) (_graphicMode == _GRAPHICIIMODE) ? (2048 * 3) : 2048)); }
		/** To get the information about the color name table. \n
			The information finally collected will depend on the active graphical mode . \n
			In the TextMode the color are not used, and in the multicolor mode the table 
			defining the colort is the pattern generation one... */
		std::vector <MCHEmul::UByte> colorNameTableSnapShot () const
							{ return (
								(_graphicMode == _GRAPHICIMODE || _graphicMode == _GRAPHICIIMODE) 
									? videoData (_colorAddress, (_graphicMode == _GRAPHICIMODE) ? 32 : (2048 * 3))
									: std::vector <MCHEmul::UByte> ()); } 
		/** To get an snapshot of the sprite definition. 
			No boundaries limits are done. \n
			The number of the sprite is from 1 to 32. */
		MCHEmul::Strings spriteDrawSnapShot (size_t nS) const
							{ return (readSpriteDefinition ((unsigned char) (nS - 1)).spriteDrawSnapShot ()); }
		/** To get a snapshot of a set of sprites.
			When parameter = empty means all sprites. */
		MCHEmul::Strings spritesDrawSnapShot (const std::vector <size_t>& nS = { }) const;

		protected:
		// This methods are directly invoked when the registers are accesible from a memory position
		// They are also invoked from other methods defined above...
		// ....when the registers are accesible from ports...
		// When a value is set/read to a internal register the MODE pin of the chip is important
		// This PIN says whether the byte that ise been sent is data or simply an instruction
		// This situation is not simulated in the Chip!
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		/** Used from the methods above from different places. */
		inline void incrementReadWriteAddress () const;

		void initializeInternalValues ();

		// Manage the internal registers of the chip...
		// The internal behaviour of the chip is controlled using the internal control registers...
		// The number of internal registers finally used, depends on the type of TEXASINSTRUMENTS (Generation).
		// However it is backwards compatible always.
		// None of both is accesible from outside!
		/** Sets the value of a register. 
			The internal registers can not be read, just set. */
		void setControlRegister (unsigned char rId, const MCHEmul::UByte& v);
		/** To get the status register. 
			When reading several internal bits are set back to initial values. 
			The status register can not be set just read. */
		MCHEmul::UByte readStatus () const;
		/** To peek the value of an internal control register. */
		const MCHEmul::UByte& peekControlRegister (unsigned char rId) const
							{ return (_controlRegisters [(size_t) rId]); } // NO boundary check is done...
		/** Peek the status but doesn't change the value of it. */
		MCHEmul::UByte peekStatus () const;

		/** Executed when the _graphicMode is changed. 
			It can be overloaded because the more complex versions of the TMS99xxFamily allow more screen modes. */
		void setGraphicMode (unsigned char gM);
		/** Which is the graphic mode. */
		unsigned char graphicMode () const
							{ return (_graphicMode); }

		// Internal methods invoked from the chip when some circunstances are detected...
		// That the reason to define them protected, and to declare TMS99xxFamily as a friend class...
		bool launchScreenUpdateInterrupt () const
							{ return (_launchScreenUpdateInterrupt); }
		void setSpriteCollisionDetected (bool cD)
							{ _spriteCollisionDetected = cD; }
		void setFifthSpriteDetected (bool fD)
							{ _fifthSpriteDetected = fD;  }
		void setFifthSpriteNotDrawn (unsigned char sN)
							{ _fifthSpriteNotDrawn = sN; }
		bool screenUpdateHappen ()
							{ return (_screenUpdateHappen); }
		void setSreenUpdateHappen ()
							{ _screenUpdateHappen = true; }
	
		// Mamaging the info of the sprites...
		/** Read the information of the sprite received as parameter. 
			The number of the sprite is from 0 to 31. */
		inline SpriteDefinition readSpriteDefinition (unsigned char nS) const;

		protected:
		/** Read the memory with new data if needed...
			...it will depend on the graphical mode. \n
			The method returns the element in the name pattern table (if makes sense),
			the element in the pattern generation table (if makes sense) 
			and the element in the color name table (again if it makes sense), in the form of a tuple... */
		std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte> 
			readGraphicInfo (unsigned short x, unsigned short y);

		protected:
		/** The Video RAM is stored inside of the TMS99xxFamily. \n
			And its size can vary depending on the generation of TEXASINSTRUMENTS. \n
			The basic size is 16k. \n
			The way that amount is stored will depend on the specfic TMS99xxFamily manufacturer,
			but it ise not important regarding the simulation. */
		std::vector <MCHEmul::UByte> _videoMemory;

		// The important control attributes of the chip...
		/** The SCREEN mode. It would be different depending on the TEXASINSTRUMENTS version... */
		unsigned char _graphicMode;
		/** Enables or not the external video input. */
		bool _externalVideo;
		/** The size of the memory: 4/16k. In TEXASINSTRUMENTS this bit has no effect. */
		bool _16k;
		/** Whether the screen is blank or not. The sprites ar off when set. */
		bool _blankScreen;
		/** Whether the sprites are 16 x 16 or 8 x 8 pixels-wide. */
		bool _sprites16pixels;
		/** Sprites enlarged? */
		bool _spritesEnlarged;
		/** Generate interrupts at the end of the raster?. Yes when set. */
		bool _launchScreenUpdateInterrupt;
		/** Name table address. */
		MCHEmul::Address _nameAddress;
		/** Color table address. */
		MCHEmul::Address _colorAddress;
		/** Pattern's table address. */
		MCHEmul::Address _patternAddress;
		/** Sprites' attributes table address. */
		MCHEmul::Address _spriteAttrsAddress;
		/** Sprites' generator. */
		MCHEmul::Address _spriteGenAddress;
		/** Text color. */
		unsigned char _textColor;
		/** Back drop color (background). */
		unsigned char _backdropColor;
		/** Same info than above but per byte. */
		std::vector <MCHEmul::UByte> _controlRegisters;

		// To reflect that something has happen!
		// These bits are managed from the chip...
		/** To detect or not the collision among sprites. */
		mutable bool _spriteCollisionDetected;
		/** To detect the situation that the 5 sprite not drawn in the same row is detected. */
		bool _fifthSpriteDetected;
		/** Fifth sprite (number from 0 to 31) not drawn when the detection of this situation is active. */
		unsigned char _fifthSpriteNotDrawn;
		/** Screen update interruption generated. */
		mutable bool _screenUpdateHappen;

		// Implementation
		/** In which moment of the use of the port 0x99 is the operation. */
		mutable bool _99setOnce;
		/** The value of the first access to the video 0x99. */
		MCHEmul::UByte _99firstAccessValue;
		/** The address where the next read/write operation will be executed over. */
		mutable MCHEmul::Address _readWriteAddress;
		/** The value read from the previous address when the read operation is selected.
			This value won't be returned until it was requested throught out thr right port. */
		mutable MCHEmul::UByte _readAheadBuffer;
		/** The last value read. */
		mutable MCHEmul::UByte _lastValueRead;

		// Implementation
		/** All used when reading graphical information. */
		bool _dataRead;
		MCHEmul::Address _lastPatternNameTablePositionRead;
		MCHEmul::UByte _lastPatterNameValueRead; // In the last position...
		MCHEmul::Address _lastPatterGeneratorTablePositionRead;
		MCHEmul::UByte _lastPatternGeneratorValueRead; // In the last position...
		MCHEmul::Address _lastColorTablePositionRead;
		MCHEmul::UByte _lastColorValueRead; // In the last position...
	};

	// ---
	inline bool TMS99xxFamilyRegisters::SpriteDefinition::visibleAtVisibleLine 
		(unsigned short vL, unsigned short uB, size_t& dF) const
	{ 
		bool result = false;
		unsigned short en	= (_enlarged ? 1 : 0); // *2 when enlarged?
		unsigned short d	= (_16pixels ? 1 : 0); // *2 when 16pixels height?
		unsigned short mL	= 8 << (en + d); // Total height?

		// Sprites can be defined in a position before the visible position...
		// So the real position could be negative!...
		// When rL = uB, the rLN will be still - 1 = 255, that is the first visible position in a sprite...
		// When rl = uB - 7, there will be only one line visible (the last one).
		int vLN = (int) vL - (int) uB - 1;
		int pY = int (((char) _posY >= 208) ? (char) _posY /** Here become negative. */ : _posY);
		if (vLN >= pY && vLN < (pY + (int) mL))
		{ 
			result = true; 
			
			// Which is the byte visible,
			// The byte can vary from 0 to 15!
			// that takes into account when enlarged and 16pixels height together...
			// But could finally be 16 bytes more, depending on the position x!
			dF = (size_t) (((unsigned short) vLN - (unsigned short) _posY) >> en);
		}

		return (result); 
	}

	// ---
	inline bool TMS99xxFamilyRegisters::SpriteDefinition::visibleAtVisiblePosition
		(unsigned short vP, unsigned short lB, size_t& dP) const
	{
		bool result = false;

		unsigned short en	= (_enlarged ? 1 : 0); // *2 when enlarged?
		unsigned short d	= (_16pixels ? 1 : 0); // *2 when 16pixels width?
		unsigned short mP	= 8 << (en + d); // Total width?

		// Sprites can be defined in a position before the visible position...
		// So the real position could be negative!...
		// When the position of the sprite is 0 and early clock is set, 
		// the sprite will start to be drawn when rP is -32...
		int vPN = (int) vP - (int) lB; // The raster position, less the left border...
		int pX = (int) _posX - (_earlyClock ? 32 : 0); // The position of the border less 32...
		if (vPN >= pX && vPN < (pX + (int) mP))
		{
			result = true;

			// Which is the "bit" visible, that takes into account when enlarged...
			// The bit can vary from 0 to 15!
			// Starting from 0 in the left, and ending (if enlarged and expanded) with 31 in the right side!
			dP = ((1 << (3 + d)) - 1) - (size_t) (((unsigned short) vPN - (unsigned short) pX) >> en);
		}

		return (result);
	}

	// ---
	inline void TMS99xxFamilyRegisters::incrementReadWriteAddress () const
	{
		_readWriteAddress += 1;
		if (_readWriteAddress.value () > 0x3fff) // If if out of the 16k, it starts back!
			_readWriteAddress -= _readWriteAddress.value ();
	}

	// ---
	inline TMS99xxFamilyRegisters::SpriteDefinition TMS99xxFamilyRegisters::readSpriteDefinition (unsigned char nS) const
	{
		TMS99xxFamilyRegisters::SpriteDefinition result =
			TMS99xxFamilyRegisters::SpriteDefinition ();

		std::vector <MCHEmul::UByte> dt = 
			videoData (_spriteAttrsAddress + (size_t) (nS << 2 /** 4 bytes each. */), 4);
		result._posY		= dt [0].value ();
		result._posX		= dt [1].value ();
		result._pattern		= dt [2].value ();
		result._color		= dt [3].value () & 0x0f;
		result._16pixels	= _sprites16pixels; // Common to all sprites...(stored here to speed up the acceses)
		result._enlarged	= _spritesEnlarged; // Common to all sprites...(stored here to speed up the acceses)
		// Bit 4 to 6 should be 0, but they are ignored in the reality...
		// so they are not taken into account here...
		result._earlyClock	= dt [3].bit (7);
		result._data = (_sprites16pixels)
			? videoData (_spriteGenAddress + (size_t) ((result._pattern & 0xfc) << 3), 32) // 32 bytes (16 * 2)
			: videoData (_spriteGenAddress + (size_t) (result._pattern << 3), 8); // 8 bytes

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
