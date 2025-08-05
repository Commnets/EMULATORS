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
		The ULA generates the clock for the CPU reducing the speed half, so the CPU is loaded with a 3.5 MHz clock. \n
		The simulation of ULA also receives the events comming the different IO elements like keyboard and joystick. \n
		The events comming from the joystick are translated into actions and kept in the ULARegisters. \n
		Those events are also translated into key presses to simulate the CURSOR type of joystick. \n
		The class PortManager will see those events when the port 31 is scaneed, to simulate the KEMPSTON type of joystick. */
	class ULA : public MCHEmul::GraphicalChip
	{
		public:
		/** The ULA has a very limited sound possibilities, just on / off.
			To generate a proper wave form and a note the software is needd,
			and to do so the interrupt system is used. */
		class SoundFunction : public MCHEmul::SoundChip
		{
			public:
			friend ULA;

			static const unsigned int _ID = 2100;

			/** 
			  *	Constructor.
			  *	@param cF	Chip frequency in clocks / second.
			  * @param sF	Sampling frequency in samples / second. It cannot be 0.
			  */
			SoundFunction (unsigned int cF, unsigned int sF);

			/** The main characteristics of the SoundChip. */
			virtual SDL_AudioFormat type () const override
								{ return (_SOUNDSAMPLINGFORMAT); }
			virtual int maxFrequency () const override
								{ return (_SOUNDSAMPLINGCLOCK >> 1); }
			virtual unsigned char numberChannels () const override
								{ return (_SOUNDCHANNELS); }

			virtual bool initialize () override;

			virtual bool simulate (MCHEmul::CPU* cpu) override;

			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			private:
			/** Invoked when SoundFunction is built. */
			void setULARegisters (ULARegisters* uR)
								{ _ULARegisters = uR; }
			// -----
			// Different debug methods to simplify the internal code
			// and to make simplier the modification in case it is needed...
			/** Debug special situations...
				Take care using this instructions _deepDebugFile could be == nullptr... */
			void debugULASoundCycle (MCHEmul::CPU* cpu, unsigned int i);
			// -----

			private:
			unsigned int _chipFrequency;

			// Implementation
			/** A Reference to the ULA Registers. */
			ULARegisters* _ULARegisters;
			/** The number of cycles the CPU was executed once the simulated method finishes. */
			unsigned int _lastCPUCycles;
			/** Number of clocks to get a sample. */
			unsigned int _clocksPerSample;
			/** Counter from 0 to _clockPerSample. */
			unsigned int _counterClocksPerSample;
		};

		static const unsigned int _ID = 210;

		// 44,1MHz (more or less standard in current sound cards)
		static const unsigned int _SOUNDSAMPLINGCLOCK		= 44100;
		// 8 bits sound data, very simple nothing complicated...
		static const unsigned short _SOUNDSAMPLINGFORMAT	= AUDIO_U8;
		// Number of channels..
		static const unsigned char _SOUNDCHANNELS			= 1;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The ULA constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the ULA version),
			a reference to the portFE that is used to read the keyboard,
			and additional attributes. */
		ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			unsigned int cF,
			unsigned char f,
			int vV, const MCHEmul::Attributes& attrs = { });

		virtual ~ULA () override;

		/** To get the sound function. */
		const SoundFunction* soundFunction () const
							{ return (_soundFunction); }
		SoundFunction* soundFunction ()
							{ return (_soundFunction); }

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

		/** Called from Computer specific cycle method,
			to determine whether the CPU accesed or not to the screen memory. \n
			This is important to detect contentions and then to whether stop the CPU. */
		void setScreenMemoryAccessedFromCPU (bool sM)
							{ _screenMemoryAccessedFromCPU = sM; }

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Read the graphical info (when needed) and draw the graphics. \n
			Returns true when the graphics are read. \n
			That returns variable will be used to determine whether 
			the CPU has to be stopped or not. */
		bool readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu);
		/** Read the memory with new data. */
		inline void readGraphicInfo (unsigned short x, unsigned short y);
		/** Draw the important events, in case this option is set. */
		void drawEvents ();

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugULACycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		protected:
		/** A reference to the sound function, that is created at construction tine. */
		SoundFunction* _soundFunction;
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
		/** Blinking frequency. */
		unsigned char _blinkingFrequency; // in Mhz... Will depend on the type of ULA...
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** Active when the screen memory is accesed (16k - 32k). 
			It can be checked from the ULA to define whether to stop the CPU or not. */
		bool _screenMemoryAccessedFromCPU;

		// Implementation
		/** Number of cycles the the CPU is stopped. */
		int _cyclesStopped;

		/** The events that can be drawn
			associated to the movement of the raster line. */
		struct EventsStatus
		{
			/** When the visible part starts. */
			MCHEmul::OBool _screenPart;
			/** Any time a graphic is read. */
			MCHEmul::OBool _graphicRead;
			/** Any time a contention situation is generated. */
			MCHEmul::OBool _contentedSituation;
		};

		mutable EventsStatus _eventStatus;

	};

	// ---
	inline void ULA::readGraphicInfo (unsigned short x, unsigned short y)
	{
		_memory -> setActiveView (_ULAView);

		// Remember that this is from ULA's point of view!
		_videoSignalData._dataLatch = _memory -> value (MCHEmul::Address (2, 
			(unsigned int) ((y & 0x38) << 2) + ((y & 0x07) << 8) + ((y & 0xc0) << 5) + ((x >> 3) & 0x1f)));
		_videoSignalData._attributeLatch = _memory -> value (MCHEmul::Address ({ 0x00, 0x18 }, false) + 
			(unsigned int) (((y >> 3) << 5) + (x >> 3)));

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

		ULA_PAL (int vV, unsigned int cF);
	};

	/** The version para NTSC systems. */
	class ULA_NTSC final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_NTSC (int vV, unsigned int cF);
	};
}

#endif
  
// End of the file
/*@}*/
