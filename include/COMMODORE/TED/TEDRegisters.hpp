/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: TEDRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: TED Registers Emulation
 *	Versions: 1.0 Initial
 *	Based on https://www.pagetable.com/docs/ted/TED%207360R0%20Preliminary%20Data%20Sheet.pdf
 */

#ifndef __COMMODORE_TEDREGISTERS__
#define __COMMODORE_TEDREGISTERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** In the TED Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class TEDRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		static const int _TEDREGS_SUBSET = 1060;
			
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

		TEDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x40); }

		// Managing the different attributes of the elements managed by TED
		// Foreground (border) & background.
		unsigned char foregroundColor () const
							{ return (_foregroundColor); }
		unsigned char backgroundColor (size_t p = 0) const
							{ return ( _backgroundColor [p]); }
		
		// ScrollX & ScrollY.
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
		bool videoResetActive () const
							{ return (_videoResetActive); }

		// Graphical modes.
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
		// To generate the IRQ the codition has to be reached
		// To activate or desactivate this sources of IRQ the register $d01A has to be accessed...
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

		// Temporal variables to know, when an raster or lightpen IRQ happened, where was the element that generated that.
		// This temporal variables are set from the TED directly...
		unsigned short currentRasterLine () const
							{ return (_currentRasterLine); }
		void setCurrentRasterLine (unsigned short rL)
							{ _currentRasterLine = rL; }
		unsigned short currentLightPenHorizontalPosition () const
							{ return (_currentLightPenHorizontalPosition); }
		unsigned short currentLightPenVerticalPosition () const
							{ return (_currentLightPenVerticalPosition); }
		void currentLightPenPosition (unsigned short& x, unsigned short& y) const
							{ x = _currentLightPenHorizontalPosition; y = _currentLightPenVerticalPosition; }
		void setCurrentLightPenPosition (unsigned short x, unsigned short y)
							{ _currentLightPenHorizontalPosition = x; _currentLightPenVerticalPosition = y; }
		bool lightPenActive () const
							{ return (_lightPenActive); }
		void setLigthPenActive (bool lP)
							{ _lightPenActive = lP; }

		// Memory address for the different elements managed from TED...
		// These methods are invoked from the VIC mainly when reading elements...
		const MCHEmul::Address& initAddressBank () const
							{ return (_initAddresbankPos); } 
		const MCHEmul::Address& charDataMemory () const
							{ return (_charDataMemoryPos); }
		const MCHEmul::Address& screenMemory () const 
							{ return (_screenMemoryPos); }
		const MCHEmul::Address& bitmapMemory () const 
							{ return (_bitmapMemoryPos); }
		
		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chipregisters plus:
		  * MODE			= Attribute: The mode working in the TED. \n
		  *	40C				= Attribute: Whether there is 40 columns or not. \n
		  * 25R				= Attribute: Whether there is 25 rows or not. \n
		  * SCROLLX			= Attribute; Value of the horizontal scroll. \n
		  * SCROLLY			= Attribute; Value of the vertical scroll. \n
		  * FORECOLOR		= Attribute; Main color. \n
		  * BKCOLOR1		= Attribute; Background 1 color value. \n
		  * BKCOLOR2		= Attribute; Background 2 color value. \n
		  * BKCOLOR3		= Attribute; Background 3 color value. \n
		  * BKCOLOR4		= Attribute; Background 4 color value. \n
		  * IRQ				= Attribute; Whether the IRQ is or not requested. \n
		  * IRQLINE			= Attribute; The number of scan line where the IRQ should be requested if on. \n
		  * CHARADDRESS		= Attribute; The address of the characters definition. \n
		  * SCREENADDRESS	= Attribute; The address of the video matrix. \n
		  * BITMAPADDRESS	= Attribute; The address of the bit maps. \n
		  * LIGHTPENX		= Attribute; Where the lightpen is in the horizontal space. \n
		  * LIGHTPENY		= Attribute; Where the lightpen is in the vertical space. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		/** Just to initialize the internal values. */
		void initializeInternalValues ();
		/** Depending on how bits ar set, a no valid mode could be set. */
		void setGraphicModeActive ();
		/** Calculate the internal memory positions. */
		inline void calculateMemoryPositions ();

		private:
		// The TED registers
		/** Screen related variables. */
		unsigned char _foregroundColor;
		std::vector <unsigned char> _backgroundColor;
		/** ScrollYRegister & ScrollXRegister. */
		unsigned char _verticalScrollPosition, _horizontalScrollPosition;
		/** Visible screen control. */
		bool _textDisplay25RowsActive, _textDisplay40ColumnsActive;
		bool _blankEntireScreen;
		bool _videoResetActive;
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

		// Some of this variables are set by the emulation of the TED
		// The TED chip also uses this object as a temporary storage
		unsigned short _currentRasterLine; // Where the raster is now...
		unsigned short _currentLightPenHorizontalPosition, _currentLightPenVerticalPosition; // Where the light pen is...
		bool _lightPenActive;
		/** Whether the raster line has reached the one defined to generate an IRQ. */
		bool _rasterIRQHappened;
		/** Whether the lightpen is on the screen. */
		bool _lightPenIRQHappened;

		/** Location of the Graphical Memory. */
		MCHEmul::Address _charDataMemory; // Info about the characters (the address with in the first 16k)
		MCHEmul::Address _screenMemory; // Where the characters to draw are (The address within the first 16k)
		MCHEmul::Address _bitmapMemory; // Where the bitmap to draw is (The address within the first 16k)

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
		MCHEmul::Address _initAddresbankPos;
		MCHEmul::Address _charDataMemoryPos; 
		MCHEmul::Address _screenMemoryPos;
		MCHEmul::Address _bitmapMemoryPos;
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

	// ---
	inline void TEDRegisters::calculateMemoryPositions ()
	{ 
		// All will depend on the bank...
		_initAddresbankPos			= MCHEmul::Address ({ 0x00, 0x00 }, false) + 
			((size_t) 0x4000 /* 16284 = 16k */ * 1); /** TED Only addresses 16k. */
		_screenMemoryPos			= _screenMemory + ((size_t) 0x4000 * 1);
		_charDataMemoryPos			= _charDataMemory + ((size_t) 0x4000 * 1); 
		_bitmapMemoryPos			= _bitmapMemory + ((size_t) 0x4000 * 1);
	}
}

#endif
  
// End of the file
/*@}*/
