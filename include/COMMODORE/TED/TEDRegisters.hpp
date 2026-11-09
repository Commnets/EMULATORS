/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: TEDRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: TED Registers Emulation.
 *	Versions: 1.0 Initial
 *	Based on https://www.pagetable.com/docs/ted/TED%207360R0%20Preliminary%20Data%20Sheet.pdf
 */

#ifndef __COMMODORE_TEDREGISTERS__
#define __COMMODORE_TEDREGISTERS__

#include <CORE/incs.hpp>
#include <COMMODORE/TED/TEDSoundWrapper.hpp>
#include <COMMODORE/TED/TEDTimer.hpp>

namespace COMMODORE
{
	class TED;

	/** In the TED Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class TEDRegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend TED;

		static const int _TEDREGS_SUBSET = 1010;
			
		enum class GraphicMode
		{
			_CHARMODE = 0,
			_MULTICOLORCHARMODE,
			_BITMAPMODE,
			_MULTICOLORBITMAPMODE,
			_EXTENDEDBACKGROUNDMODE,
			_INVALIDTEXMODE,
			_INVALIDBITMAPMODE1,
			_INVALIDBITMAPMODE2,
		};

		/** Effective raster counters shared with the owning TED. \n
			Horizontal positions use dot units. \n
			VideoLine is the internal vertical capture, not a framebuffer coordinate. */
		struct RasterPosition final
		{
			RasterPosition ()
				: _vertical (0), _horizontal (0), _videoLine (0),
				  _horizontalWritePending (false), _horizontalWriteValue (0)
							{ }

			/** Replace only bit 8; values outside the normal frame remain valid. */
			void writeVerticalHigh (unsigned char value)
							{ _vertical = (_vertical & 0x00ff) | ((value & 0x01) << 8); }
			/** Replace only bits 7..0 of the current counter. */
			void writeVerticalLow (unsigned char value)
							{ _vertical = (_vertical & 0x0100) | value; }

			/** Retain the bus value until the following dot-clock tick. */
			inline void queueHorizontalWrite (unsigned char value);

			unsigned short _vertical;
			unsigned short _horizontal;
			unsigned short _videoLine;
			bool _horizontalWritePending;
			unsigned char _horizontalWriteValue;
		};

		/** Colors in TED are made always of a proper color (0 - 15)
			and the luninance. (0 - 7). */
		class Color final : public MCHEmul::InfoClass
		{
			public:
			Color ()
				: MCHEmul::InfoClass ("TEDColor"),
				  _color (0), _luminance (0)
							{ }

			Color (unsigned char c, unsigned char l)
				: MCHEmul::InfoClass ("TEDColor"),
				  _color (c), _luminance (l)
							{ }

			/** The luminance is in bits 4, 5 & 6 y color in bits 0, 1, 2, 3. */
			unsigned char asChar () const
							{ return (((_luminance & 0x07) << 4) | (_color & 0x0f)); }

			/** To get the nextluminance. */
			Color nextLuminance () const
							{ return (Color (_color, (_luminance == 7) ? 0 : (_luminance + 1))); }

			/**
			  *	The name of the fields are: \n
			  * The ones coming from the parent class and: \n
			  * COLOR		= Attribute: Color index, from 0 to 15. \n
			  * LUMINANCE	= Attribute: Luminance, from 0 to 7.
			  */
			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			/** From 0 to 15. */
			unsigned char _color;
			/** From 0 to 7. */
			unsigned char _luminance;
		};

		/** Shared character counters and a published diagnostic copy of the raster. \n
			Raster coordinates and character counters have distinct update points. */
		class GraphicalInfo final : public MCHEmul::InfoClass
		{
			public:
			GraphicalInfo ()
				: MCHEmul::InfoClass ("TEDGraphicalInfo"),
				  _currentRasterLine (0), _currentRasterColumn (0),
				  _currentCharacterPosition (0), _currentCharacterRow (0)
							{ }

			GraphicalInfo (unsigned short rL, unsigned short rC, 
				unsigned short cP, unsigned char cR)
				: MCHEmul::InfoClass ("TEDGraphicalInfo"),
				  _currentRasterLine (rL), _currentRasterColumn (rC),
				  _currentCharacterPosition (cP), _currentCharacterRow (cR)
							{ }

			/**
			  *	The name of the fields are: \n
			  * The ones coming from the parent class and: \n
			  * RASTERY	= Attribute: Last published raster line, not the live counter. \n
			  * RASTERX	= Attribute: Last published raster column in dot units. \n
			  * CHARPOS	= Attribute: Character position reload counter, from 0 to 1023. \n
			  * CHARROW	= Attribute: Shared vertical character subaddress, from 0 to 7.
			  */
			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			/** Diagnostic raster coordinates copied by the owning TED. */
			unsigned short _currentRasterLine;
			unsigned short _currentRasterColumn; 
			/** Character position reload counter. From 0 to 1023. */
			unsigned short _currentCharacterPosition; 
			/** Vertical subaddress inside a character. From 0 to 7. */
			unsigned char _currentCharacterRow;
		};

		TEDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x20); }

		// Clock mode
		bool singleClockModeForced () const
							{ return (_singleClockModeForced); }

		// Backgrund & Boder colors
		const Color& backgroundColor (size_t p = 0) const
							{ return (_backgroundColor [p]); }
		Color& backgroundColor (size_t p = 0)
							{ return (_backgroundColor [p]); }
		const Color& borderColor () const
							{ return (_borderColor); }
		Color& borderColor ()
							{ return (_borderColor); }

		// Screen behaviour
		unsigned char verticalScrollPosition () const
							{ return (_verticalScrollPosition); }
		unsigned char horizontalScrollPosition () const
							{ return (_horizontalScrollPosition); }
		bool textDisplay25RowsActive () const
							{ return (_textDisplay25RowsActive); }
		bool textDisplay40ColumnsActive () const
							{ return (_textDisplay40ColumnsActive); }
		bool blankEntireScreen () const
							{ return (_blankEntireScreen); }
		bool freezeActive () const
							{ return (_freezeActive); }
		bool PALActive () const
							{ return (_PALActive); }
		bool reverseVideoActive () const
							{ return (_reverseVideoActive); }
		unsigned short cursorPosition () const
							{ return (_cursorPosition); }

		// Graphical modes
		bool graphicBitModeActive () const
							{ return (_graphicBitModeActive); }
		bool graphicExtendedColorTextModeActive () const
							{ return (_graphicExtendedColorTextModeActive); }
		bool graphicMulticolorTextModeActive () const
							{ return (_graphicMulticolorTextModeActive); }
		GraphicMode graphicModeActive () const
							{ return (_graphicModeActive); }
		unsigned char characterIndexMask () const
							{ return (_characterIndexMask); }
		bool textMode () const
							{ return (_textMode); }

		// Flash counter
		void incrementFlashCounter ()
						{ _flashCounter = (_flashCounter + 1) & 0x01f; }
		bool flashCounterOn () const
						{ return ((_flashCounter & 0x10) != 0); }

		// IRQs...
		// To know whether a IRQ could be generated in different scenarios...
		// To generate the IRQ the condition has to be reached
		// FF0A enables sources; FF09 reports and acknowledges their pending flags.
		/** Whether the raster IRQ source is enabled. */
		bool rasterIRQActive () const
							{ return (_rasterIRQActive); }
		/** Whether the reserved lightpen IRQ source is enabled. */
		bool lightPenIRQActive () const
							{ return (_lightPenIRQActive); }
		/** Raster line number when the TED (if active) will generate an IRQ. */
		unsigned short IRQRasterLineAt () const
							{ return (_IRQRasterLineAt);  }
		// Activate the IRQ. 
		// All these methods are called from TED simulation...
		void activateRasterIRQ ()
							{ _rasterIRQHappened = true; }
		void activateLightPenOnScreenIRQ ()
							{ _lightPenIRQHappened = true; }
		/** Combined request from enabled pending TED sources. \n
			CPU interrupt acceptance is independent of this level. */
		inline bool launchIRQ () const;
		/** To have a code for the reason of the interruption. 
			There might be several reasons defined within the code. */
		inline unsigned int reasonIRQCode () const;

		/** To know whether the source of the bit dot or character data is in ROM or in RAM. */
		bool ROMActiveToFetchCharAndBitmap () const
							{ return (_ROMActiveToFetchCharAndBitmap); }
		/** To know whether the ROM configuration is or or not active. 
			This variable reflects the situation of the memory and it is managed externally. */
		bool ROMMemoryConfigurationActive () const
							{ return (_ROMMemoryConfigurationActive); }
		/** The is usually changed externally accrding with the memory configuration.
			It has to be usually involked in simulate method of the class TED. */
		void setROMMemoryConfigurationActive (bool r)
							{ _ROMMemoryConfigurationActive = r; }

		// Graphical info: Used from TED during the simulation...
		const GraphicalInfo& graphicalInfo () const
							{ return (_graphicalInfo); }
		GraphicalInfo& graphicalInfo () // This method is used to change data...
							{ return (_graphicalInfo); }
		void setGraphicalInfo (const GraphicalInfo& gI)
							{ _graphicalInfo = gI; }

		// Memory zones
		const MCHEmul::Address& charDataMemory () const
							{ return (_charDataMemory); }
		const MCHEmul::Address& screenMemory () const 
							{ return (_screenMemory); }
		const MCHEmul::Address& attributeMemory () const 
							{ return (_attributeMemory); }
		const MCHEmul::Address& bitmapMemory () const 
							{ return (_bitmapMemory); }

		// Related with the keyboard latch...
		const MCHEmul::UByte& keyboardLatch () const
							{ return (_keyboardLatch); }
		// ...abd the value in the keyboard pins...
		const MCHEmul::UByte& keyboardPins () const
							{ return (_keyboardPins); }
		void setKeyboardPins (const MCHEmul::UByte& kp)
							{ _keyboardPins = kp; }

		/** Updates the derived raster coordinates used by diagnostics. */
		void setCurrentRasterPosition (unsigned short rL, unsigned short rC)
							{ _graphicalInfo._currentRasterLine = rL; _graphicalInfo._currentRasterColumn = rC; }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  * MODE				= Attribute: The mode working in the TED. \n
		  *	40C					= Attribute: Whether there is 40 columns or not. \n
		  * 25R					= Attribute: Whether there is 25 rows or not. \n
		  * SCROLLX				= Attribute; Value of the horizontal scroll. \n
		  * SCROLLY				= Attribute; Value of the vertical scroll. \n
		  * CURSOR				= Attribute: Current cursor position. \n
		  * BkColor1			= InfoStructure; Background 1 color value. \n
		  * BkColor2			= InfoStructure; Background 2 color value. \n
		  * BkColor3			= InfoStructure; Background 3 color value. \n
		  * BkColor4			= InfoStructure; Background 4 color value. \n
		  * BorderColor			= InfoStructure; Border color value. \n
		  * SINGLECLK			= Attribute; Whether the single clock mode is or not forced. \n
		  * PAL					= Attribute: PAL selection requested through the registers. \n
		  * FREEZE				= Attribute: Requested FREEZE control. \n
		  * FLASHCOUNTER		= Attribute: Full five-bit FLASH counter. \n
		  * FLASHPHASE			= Attribute: Bit 4 of the FLASH counter. \n
		  * IRQ					= Attribute: Raster IRQ enable, retained for compatibility. \n
		  * RASTERIRQPENDING	= Attribute: Retained raster event, regardless of its enable. \n
		  * IRQREQUEST			= Attribute: Combined TED request, not CPU acceptance. \n
		  * IRQREASON			= Attribute: Internal request reason code, not an FF09 bitmask. \n
		  * IRQLINE				= Attribute; The number of scan line where the IRQ should be requested if on. \n
		  * SCREENADDRESS		= Attribute; The address of the video matrix. \n
		  * CHARADDRESS			= Attribute; The address of the characters definition. \n
		  * CHARSINROM			= Attribute; Whether the characters are in ROM or in RAM. \n
		  * ATTRIBUTEADDRESS	= Attribute; The address of the attribute definition. \n
		  * BITMAPADDRESS		= Attribute; The address of the bit maps. \n
		  * GraphicalInfo		= InfoStructure: Shared character counters and published raster copy.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		/** Reads a register through the observed-access path. */
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (readRegisterValue (p, true)); }
		/** Inspects the same value without emitting register-access diagnostics. */
		virtual const MCHEmul::UByte& peekValue (size_t p) const override
							{ return (readRegisterValue (p, false)); }

		// Implementation
		/** To look at the chips & pieces related with this. */
		void lookAtSoundLibWrapper (MCHEmul::SoundLibWrapper* w)
							{ _soundWrapper = dynamic_cast <TEDSoundLibWrapper*> (w); }
		void lookAtTimers (TEDTimer* t1, TEDTimer* t2, TEDTimer* t3)
							{ _T1 = t1, _T2 = t2; _T3 = t3; }

		/** Shared decoding for observed reads and debugger inspection. */
		const MCHEmul::UByte& readRegisterValue (size_t p, bool observed) const;
		/** Just to initialize the internal values. */
		void initializeInternalValues ();
		/** Depending on how bits ar set, a no valid mode could be set. */
		void setGraphicModeActive ();

		/** Non-owning raster-state link, established before register use. */
		void linkToRasterPosition (RasterPosition* position)
							{ _rasterPosition = position; }
		/** Non-owning CPU reference for observed register-access diagnostics. */
		void linkToDebugCPU (const MCHEmul::CPU* cpu)
							{ _debugCPU = cpu; }
		unsigned short currentRasterLine () const
							{ return (_rasterPosition -> _vertical); }
		unsigned short currentRasterPositionInLine () const
							{ return (_rasterPosition -> _horizontal); }

		/** The alignement of the char Data memory will depend on the mode,
			so any time the mode is changed, it must be recalculated. */
		inline void recalculateCharDataMemory ();

		private:
		//-----
		std::string debugInterruptAsString () const;
		void debugRegisterWrite (size_t p, const MCHEmul::UByte& value, bool after) const;
		void debugRegisterRead (size_t p, const MCHEmul::UByte& value) const;
		//-----

		protected:
		// Elements related with this one...
		/** The sound wrapper. */
		TEDSoundLibWrapper* _soundWrapper;
		/** The timers. */
		TEDTimer *_T1, *_T2, *_T3;

		// Info Registers
		/** When the single clock mode is always forced for the CPU. */
		bool _singleClockModeForced;
		/** Screen related variables. */
		std::vector <Color> _backgroundColor;
		Color _borderColor;
		/** ScrollYRegister & ScrollXRegister. */
		unsigned char _verticalScrollPosition, _horizontalScrollPosition;
		/** Visible screen control. */
		bool _textDisplay25RowsActive, _textDisplay40ColumnsActive;
		bool _blankEntireScreen;
		bool _freezeActive;
		bool _PALActive;
		bool _reverseVideoActive;
		unsigned short _cursorPosition;
		/** Graphical modes. */
		bool _graphicBitModeActive;
		bool _graphicExtendedColorTextModeActive;
		bool _graphicMulticolorTextModeActive;
		GraphicMode _graphicModeActive;
		bool _textMode; // True when a text mode is active
		/** IRQ Control. */
		bool _rasterIRQActive;
		bool _lightPenIRQActive;
		/** Nine-bit raster comparator programmed and read through FF0A.0/FF0B. */
		unsigned short _IRQRasterLineAt;
		/** Flash control counter. */
		unsigned char _flashCounter;

		// Managed from the TED simulation directly...
		/** About the graphical Info. */
		GraphicalInfo _graphicalInfo;
		/** Whether the raster line has reached the one defined to generate an IRQ. */
		bool _rasterIRQHappened;
		/** Whether the lightpen is on the screen. */
		bool _lightPenIRQHappened;

		/** Location of the Graphical Memory. 
			Info calculated when the registers are accessed. */
		bool _ROMActiveToFetchCharAndBitmap;
		bool _ROMMemoryConfigurationActive;
		MCHEmul::Address _charDataMemory;	
		MCHEmul::Address _screenMemory;
		MCHEmul::Address _attributeMemory;
		MCHEmul::Address _bitmapMemory;		

		/** The keyboard latch, 
			and the info at the entrace of the keyboard pins. */
		MCHEmul::UByte _keyboardLatch;
		/** This byte is what it is in the keyboard pins before it is latched into the previous one.
			It has to be set by anyone else out of this register. */
		MCHEmul::UByte _keyboardPins;

		// Implementation
		/** Character-code bits used by the current fetch configuration. */
		unsigned char _characterIndexMask;
		mutable MCHEmul::UByte _lastValueRead;

		/** Non-owning link to raster state owned by TED. */
		RasterPosition* _rasterPosition;
		/** Non-owning diagnostic link, cleared before register initialization. */
		const MCHEmul::CPU* _debugCPU;
};

	// ---
	inline void TEDRegisters::RasterPosition::queueHorizontalWrite (unsigned char value)
	{
		_horizontalWriteValue = value;
		_horizontalWritePending = true;
	}

	// ---
	inline bool TEDRegisters::launchIRQ () const
	{ 
		return ((_rasterIRQHappened && _rasterIRQActive) ||
				_T1 -> launchInterrupt () ||
				_T2 -> launchInterrupt () ||
				_T3 -> launchInterrupt () ||
				(_lightPenIRQHappened && _lightPenIRQActive)); 
	}

	// ---
	inline unsigned int TEDRegisters::reasonIRQCode () const
	{
		return (((_rasterIRQHappened && _rasterIRQActive) ? 1 : 0) +
				((_T1 -> interruptRequested () && _T1 -> interruptEnabled ()) ? 2 : 0) +
				((_T2 -> interruptRequested () && _T2 -> interruptEnabled ()) ? 4 : 0) +
				((_T3 -> interruptRequested () && _T3 -> interruptEnabled ()) ? 8 : 0) +
				((_lightPenIRQHappened && _lightPenIRQActive) ? 16 : 0)); 
	}

	// ---
	inline void TEDRegisters::recalculateCharDataMemory ()
	{
		_characterIndexMask = _graphicExtendedColorTextModeActive
			? 0x3f : (_reverseVideoActive ? 0x7f : 0xff);

		// Only the 128-character configuration preserves the additional base bit.
		const unsigned char mask =
			(_characterIndexMask == 0x7f) ? 0xfc : 0xf8;

		_charDataMemory = MCHEmul::Address
			(2, (unsigned int)
				((MCHEmul::PhysicalStorageSubset::readValue (0x13).value () & mask) << 8));
	}
}

#endif
  
// End of the file
/*@}*/
