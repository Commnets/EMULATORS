/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: ULA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The "ULA" Chip.
 *				 This chip is used to produce the video signal, the sound, and also to read the keyboard.
 *	Versions: 1.0 Initial
 *	Based on: https://pdfcoffee.com/the-zx-spectrum-ula-how-to-design-a-micro-chris-smith-pdf-free.html
 */

#ifndef __ZXSPECTRUM_ULA__
#define __ZXSPECTRUM_ULA__

#include <CORE/incs.hpp>
#include <ZXSpectrum/ULARegisters.hpp>

namespace ZXSPECTRUM
{
	/** The ULA is loaded with a 7MHz clock. \n
		In the PAL/NTSC mode, the horizontal raster takes 64us to go through a line, which is 448 7MH clock cycles. \n
		In the PAL mode, the vertical raster go through 312 lines to produce images 50 per second, 
		whilst in NTSC does 264 lines to produce images 60 per second. \n
		The ULA generates the clock for the CPU reducing the speed half, so the CPU is loaded with a 3.5 MHz clock. */
	class ULA : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 210;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The ULA constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the ULA version),
			a reference to the portFE that is used to read the keyboard,
			and additional attributes. */
		ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, const MCHEmul::Attributes& attrs = { });

		virtual ~ULA () override;

		/** To get a reference to the ULARegisters. */
		const ULARegisters* registers () const
							{ return (_ULARegisters); }
		ULARegisters* registers ()
							{ return (_ULARegisters); }

		// Size & position of the scren...
		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always with in the visible screen. */
		inline void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2)
							{ _raster.displayPositions (x1, y1, x2, y2); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }
		MCHEmul::Raster& raster ()
							{ return (_raster); }

		/** To activate or desactivate the visualization of events. */
		void setShowEvents (bool sE)
							{ _showEvents = sE; }

		virtual bool initialize () override;

		/** Simulates cycles in the ULA. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * ULARegisters	= InfoStructire: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Read the graphical info (when needed) and draw the graphics. */
		void readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu);
		/** Read the memory with new data. */
		inline void readGraphicInfo (unsigned short x, unsigned short y);
		/** Draw the important events, in case this option is set. */
		void drawEvents ();

		protected:
		/** A reference to the ULA registers. */
		ULARegisters* _ULARegisters;
		/** The number of the memory view used to read the data. */
		int _ULAView;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To show or no the main events that affects the visualization. */
		bool _showEvents;

		// Implementation
		/** To control the status of the video display. 
			This structure respresents internal registerrs of the ULA to generate the video signal. */
		struct VideoSignalData
		{
			VideoSignalData ()
				: _vidEN (false),
				  _dataLatch (MCHEmul::UByte::_0), 
				  _shiftRegister (MCHEmul::UByte::_0), 
				  _shiftCounter (0), 
				  _lastBitShifted (false),
				  _attributeLatch (MCHEmul::UByte::_0), 
				  _attribute (MCHEmul::UByte::_0),
				  _flash (false), 
				  _flashCounter (0)
							{ }

			/** Anytime the raster enters the display zone the varibale holding 
				all info this stuff has to be reinilized. */
			inline void initializeDisplayZone ();

			/** Invoked in every ULA clock. \n
				Returns true when a new data has to be loaded.
				and false in other circusntance. */
			inline bool clock ();

			/** True when the pixel display zone is being generated, false in other case. */
			MCHEmul::Pulse _vidEN;
			/** The data with the video info latched from the memory. */
			MCHEmul::UByte _dataLatch;
			/** Contains the video information transfer from the previous latch 
				to be displayed and it is shifed in every ULA clock cycle. 
				This register is loaded from _dataLatch every 8 ULA clocks (at the beginning of the clock 0). */
			MCHEmul::UByte _shiftRegister;
			/** From 0 to 7. When 0, reload the info from _dataLatch. */
			unsigned char _shiftCounter;
			/** Last bit shifted. */
			bool _lastBitShifted;
			/** The fata with the attribute info latched from the memory. */
			MCHEmul::UByte _attributeLatch;
			/** Contains the attribute of the pixels shifted. */
			MCHEmul::UByte _attribute;

			/** The information for the Flash. */
			bool _flash;
			/** Every 32 frames the flas changes. */
			unsigned char _flashCounter;
		};

		/** The video signal data. */
		VideoSignalData _videoSignalData;
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** Whether the vertical raster has entered the first VBlank zone already. */
		bool _firstVBlankEntered;
	};

	// ---
	inline void ULA::readGraphicInfo (unsigned short x, unsigned short y)
	{
		_memory -> setActiveView (_ULAView);

		unsigned short vP = ((y & 0x38) << 2) + ((y & 0x07) << 8) + ((y & 0xc0) << 5) + ((x >> 3) & 0x1f);
		_videoSignalData._dataLatch = _memory -> value (MCHEmul::Address (2, (unsigned int) vP));
		_videoSignalData._attributeLatch = _memory -> value (MCHEmul::Address ({ 0x00, 0x18 }, false) + 
				(unsigned int) (vP >> 3 /** 8 times less. */));

		_memory -> setCPUView ();
	}

	// ---
	inline void ULA::VideoSignalData::initializeDisplayZone ()
	{
		_dataLatch = MCHEmul::UByte::_0;
		_shiftRegister = MCHEmul::UByte::_0;
		_shiftCounter = 0;
		_lastBitShifted = false;
		_attributeLatch = MCHEmul::UByte::_0;
		_attribute = MCHEmul::UByte::_0;

		// The status of the flash is not initialized...
	}

	// ---
	inline bool ULA::VideoSignalData::clock ()
	{
		bool result = false;

		if (_shiftCounter == 0)
		{
			_shiftRegister = _dataLatch;

			_attribute = _attributeLatch;
		}

		if (result = (++_shiftCounter == 8))
			_shiftCounter = 0;

		_lastBitShifted = _shiftRegister.shiftLeftC (false, 1);

		return (result);
	}

	/** The version para PAL systems. */
	class ULA_PAL final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_PAL (int vV);
	};

	/** The version para NTSC systems. */
	class ULA_NTSC final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_NTSC (int vV);
	};
}

#endif
  
// End of the file
/*@}*/
