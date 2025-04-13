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
		static const unsigned int _ID = 1200;

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

		/** Destroy the internal physycal memory when created. */
		virtual ~TMS99xxFamilyRegisters ();

		/** This chip has only 2 registers valid: 0 & 1 */
		virtual size_t numberRegisters () const override
							{ return (0x02); }

		// The class is designed to be accessed from wither a port or from a memory position.
		// When it is accessed from a port the following methods must be used...
		// ...and they invoke the ones used when the positions are accesed from a memory...
		// ...readValue, peekValue & setValue defined below)
		/** To read a register. 
			When reading a register the internal situation could be modified. */
		MCHEmul::UByte readRegister (unsigned char rId) const
							{ return (readValue ((size_t) rId)); }
		/** Like the previous one but not changing tthe internal situation of the registers. */
		MCHEmul::UByte peekRegister (unsigned char rId) const
							{ return (peekValue ((size_t) rId)); }
		/** To change the value of a register. \n
			The internal situation is also change. */
		void setRegister (unsigned char rId, const MCHEmul::UByte& v)
							{ setValue ((size_t) rId, v); }

		// Manage the internal video memory of the chip...
		// Take care because no boundaries checks are done!
		size_t videoMemorySize () const
							{ return (_videoMemory.size ()); }
		const std::vector <MCHEmul::UByte>& videoMemory () const
							{ return (_videoMemory); }
		const MCHEmul::UByte& videoData (const MCHEmul::Address& pos) const
							{ return (_videoMemory [(size_t) pos.value ()]); }
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

		/** To get the content of the screen memory. \n
			The information finally retrieved will depend on the active graphical mode. */
		MCHEmul::UBytes screenMemorySnapShot () const;
		/** To get the information about the colors. \n
			The information finally collected will depend on the active graphical mode . */
		MCHEmul::UBytes colorMemorySnapShot () const;

		protected:
		// This methods are directly invoked when the registers are accesible from a memory position
		// They are also invoked from other methods defined above...
		// ....when the registers are accesible from ports...
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

		// Internal methods invoked from the chip when some circunstances are detected...
		// That the reason to define them protected, and to declare TMS99xxFamily as a friend class...
		bool launchScreenUpdateInterrupt () const
							{ return (_launchScreenUpdateInterrupt); }
		void setSpriteCollisionDetected ()
							{ _spriteCollisionDetected = true; }
		void setFifthSpriteDetected ()
							{ _fifthSpriteDetected = true;  }
		void setFifthSpriteNotDrawn (unsigned char sN)
							{ _fifthSpriteNotDrawn = sN; }
		bool screenUpdateHappen ()
							{ return (_screenUpdateHappen); }
		void setSreenUpdateHappen ()
							{ _screenUpdateHappen = true; }

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
		/** Sprites' table address. */
		MCHEmul::Address _spriteAddress;
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
		bool _99setOnce;
		/** The value of the first access to the video 0x99. */
		MCHEmul::UByte _99firstAccessValue;
		/** The address where the next read/write operation will be executed over. */
		mutable MCHEmul::Address _readWriteAddress;
		/** The value read from the previous address when the read operation is selected.
			This value won't be returned until it was requested throught out thr right port. */
		mutable MCHEmul::UByte _readAheadBuffer;
		
		/** This variable is created when the register is created 
			to be accessed fromports and not from internal memory. */
		MCHEmul::PhysicalStorage* _internalPhysicalMemory;
		/** The last value read. */
		mutable MCHEmul::UByte _lastValueRead;
	};

	// ---
	inline void TMS99xxFamilyRegisters::incrementReadWriteAddress () const
	{
		_readWriteAddress += 1;
		if (_readWriteAddress.value () > 0x3fff) // If if out of the 16k, it starts back!
			_readWriteAddress -= _readWriteAddress.value ();
	}
}

#endif
  
// End of the file
/*@}*/
