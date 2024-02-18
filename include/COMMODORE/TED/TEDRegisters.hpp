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

namespace COMMODORE
{
	class TED;
	class TEDTimer;
	class C6529B;

	/** In the TED Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class TEDRegisters final : public MCHEmul::ChipRegisters
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
							{ return (((_luminance & 0x0f) << 4) | (_color & 0x0f)); }

			/** To get the nextluminance. */
			Color nextLuminance () const
							{ return (Color (_color, (_luminance == 7) ? 0 : (_luminance + 1))); }

			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			/** From 0 to 15. */
			unsigned char _color;
			/** From 0 to 7. */
			unsigned char _luminance;
		};

		/** The TED has all infomation about the way the raster is made accesible. \n
			This is a key difference between it and the rest of the CBM graphical chips, specially VICII. */
		class GraphicalInfo final : public MCHEmul::InfoClass
		{
			public:
			GraphicalInfo ()
				: MCHEmul::InfoClass ("TEDGraphicalInfo"),
				  _currentRasterLine (0), _currentRasterColumn (0),
				  _currentCharacterPosition (0), _currentCharacterRow (0),
				  _flashCounter (0)
							{ }

			GraphicalInfo (unsigned short rL, unsigned short rC, 
				unsigned short cP, unsigned char cR, unsigned char fC)
				: MCHEmul::InfoClass ("TEDGraphicalInfo"),
				  _currentRasterLine (rL), _currentRasterColumn (rC),
				  _currentCharacterPosition (cP), _currentCharacterRow (cR),
				  _flashCounter (fC)
							{ }

			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			/** Where the raster is now. */
			unsigned short _currentRasterLine;
			unsigned short _currentRasterColumn; 
			/** From 0 to 40. */
			unsigned short _currentCharacterPosition; 
			/** From 0 to 7. */
			unsigned char _currentCharacterRow;
			/** Counting for flashing. */
			unsigned char _flashCounter; 
		};

		TEDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x40); }

		// Clock mode
		bool singleClockModeActive () const
							{ return (_singleClockModeActive); }

		// Backgrund & Boder colors
		const Color& backgroundColor (size_t p = 0) const
							{ return (_backgroundColor [p]); }
		Color& backkgroundColor (size_t p = 0)
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
		bool textMode () const
							{ return (_textMode); }

		// IRQs...
		// To know whether a IRQ could be generated in different scenarios...
		// To generate the IRQ the condition has to be reached
		// To activate or desactivate this sources of IRQ the register $ff09 has to be accessed...
		/** IRQ launched when raster was at line...? */
		bool rasterIRQActive () const
							{ return (_rasterIRQActive); }
		/** IRQ launched when lightpen touche the screen? */
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
		/** To know whether the TED might launch a IRQ (from its internal perspective only).
			The IRQ will be or not actually launched depending on other elements like whether the IRQ flag is or not active. */
		inline bool launchIRQ () const;
		/** To have a code for the reason of the interruption. 
			There might be several reasons defined within the code. */
		inline unsigned int reasonIRQCode () const;

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

		// Related with the access to the memory...
		bool hiromSelected () const
							{ return (_HIROMSelected); }
		// Any time the value above changes, this variable is set to true
		// and when checked it goes back to false...
		bool romAccessChanged () const
							{ return (_ROMAccessChanged); }
		bool peekROMAccessChanged () const
							{ return (_ROMAccessChanged.peekValue ()); }

		// Managing the status of the joystick and keyboard...
		const MCHEmul::UByte& joystickStatus (size_t j)
							{ return (_joystickStatus [j]); }
		void setJoystickStatus (size_t j, const MCHEmul::UByte& js)
							{ _joystickStatus [j] = js; }
		/** and these ones are to manage the matrix of keys pressed...
			...and also the inputs comming from the joystick 1... */
		bool keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_keyboardStatusMatrix [r].bit (c)); }
		const MCHEmul::UByte& keyboardStatusMatrix (size_t r) const
							{ return (_keyboardStatusMatrix [r]); }
		void setKeyboardStatusMatrix (size_t r, size_t c, bool s)
							{ _keyboardStatusMatrix [r].setBit (c, s);
							  _rev_keyboardStatusMatrix [c].setBit (r, s); }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  * MODE			= Attribute: The mode working in the TED. \n
		  *	40C				= Attribute: Whether there is 40 columns or not. \n
		  * 25R				= Attribute: Whether there is 25 rows or not. \n
		  * SCROLLX			= Attribute; Value of the horizontal scroll. \n
		  * SCROLLY			= Attribute; Value of the vertical scroll. \n
		  * BKCOLOR1		= InfoStructure; Background 1 color value. \n
		  * BKCOLOR2		= InfoStructure; Background 2 color value. \n
		  * BKCOLOR3		= InfoStructure; Background 3 color value. \n
		  * BKCOLOR4		= InfoStructure; Background 4 color value. \n
		  * BORDER			= InfoStructure; Border color value. \n
		  * IRQ				= Attribute; Whether the IRQ is or not requested. \n
		  * IRQLINE			= Attribute; The number of scan line where the IRQ should be requested if on. \n
		  * CHARADDRESS		= Attribute; The address of the characters definition. \n
		  * SCREENADDRESS	= Attribute; The address of the video matrix. \n
		  * BITMAPADDRESS	= Attribute; The address of the bit maps. \n
		  * GRAPHICALINFO	= InfoStructure; Info about how the graphics are being drawn.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		/** To look at the chips & pieces related with this. */
		void lookAtSoundLibWrapper (MCHEmul::SoundLibWrapper* w)
							{ _soundWrapper = dynamic_cast <TEDSoundLibWrapper*> (w); }
		void lookAtTimers (TEDTimer* t1, TEDTimer* t2, TEDTimer* t3)
							{ _T1 = t1, _T2 = t2; _T3 = t3; }
		void lookAtC2569B (C6529B* c)
							{ _c6529B = c; }

		/** Just to initialize the internal values. */
		void initializeInternalValues ();
		/** Depending on how bits ar set, a no valid mode could be set. */
		void setGraphicModeActive ();

		private:
		// Elements related with this one...
		/** The sound wrapper. */
		TEDSoundLibWrapper* _soundWrapper;
		/** The timers. */
		TEDTimer *_T1, *_T2, *_T3;
		/** The C65219B Chip. 
			Used to read the keyboard. */
		C6529B* _c6529B;

		// Info Registers
		/** Related with clock cycle. */
		bool _singleClockModeActive;
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
		/** Raster Control. */
		unsigned short _IRQRasterLineAt; // To define where to launch the IRQ. When reading therre is other variable...

		// Managed from the TED simulation directly...
		/** About the graphical Info. */
		GraphicalInfo _graphicalInfo;
		/** Whether the raster line has reached the one defined to generate an IRQ. */
		bool _rasterIRQHappened;
		/** Whether the lightpen is on the screen. */
		bool _lightPenIRQHappened;
		/** Whether the access to the HIROM has or not been selected. */
		bool _HIROMSelected;
		MCHEmul::OBool _ROMAccessChanged;

		/** Location of the Graphical Memory. 
			Info calculated when the registers are accessed. */
		bool _ROMSourceActive;
		MCHEmul::Address _charDataMemory;	
		MCHEmul::Address _screenMemory;
		MCHEmul::Address _attributeMemory;
		MCHEmul::Address _bitmapMemory;		

		/** Status of the joystick. 
			Initially the number of joysticks to be managed is not known. */
		std::vector <MCHEmul::UByte> _joystickStatus;
		/** 0 at the bit representing the part of the keyboard matrix pressed. 
			In this case the matrix is always 8 * 8 = 64 positions. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};

	// ---
	inline bool TEDRegisters::launchIRQ () const
	{ 
		return ((_rasterIRQHappened && _rasterIRQActive) ||
				(_lightPenIRQHappened && _lightPenIRQActive)); 
	}

	// ---
	inline unsigned int TEDRegisters::reasonIRQCode () const
	{
		return (((_rasterIRQHappened && _rasterIRQActive) ? 1 : 0) +
				((_lightPenIRQHappened && _lightPenIRQActive) ? 2 : 0)); 
	}
}

#endif
  
// End of the file
/*@}*/
