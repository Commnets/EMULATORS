/** \ingroup TEXASINSTRUMENTS */
/*@{*/

/**	
 *	@file	
 *	File: TMS99XX.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: The VSD Chip.
 *				 This chip is used to produce video signal. 
 *				 It is used in many computers, specially in the standard MSX.
 *	Versions: 1.0 Initial
 *  Bsed on the standard: https://www.cs.columbia.edu/~sedwards/papers/TMS9918.pdf
 */

#ifndef __TEXASINSTRUMENTS_TMS9918__
#define __TEXASINSTRUMENTS_TMS9918__

#include <CORE/incs.hpp>
#include <TEXASINST/TMS99XX/TMS99XXRegisters.hpp>

namespace TEXASINSTRUMENTS
{
	class TMS99XX : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 210;

		TMS99XX (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			unsigned int cF,
			const MCHEmul::Attributes& attrs = { });

		virtual ~TMS99XX () override;

		/** To get a reference to the TMS9918Registers. */
		const TMS99XXRegisters* registers () const
							{ return (_TMS99XXRegisters); }
		TMS99XXRegisters* registers ()
							{ return (_TMS99XXRegisters); }

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

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * TMS9918Registers	= InfoStructire: Info about the registers.
		  * Raster			 	= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Read the graphical info (when needed) and draw the graphics. \n
			Returns true when the graphics are read. \n
			That returns variable will be used to determine whether 
			the CPU has to be stopped or not. */
		void readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu);
		/** Read the memory with new data. */
		inline void readGraphicInfo (unsigned short x, unsigned short y);
		/** Draw the important events, in case this option is set. */
		void drawEvents ();

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugTMS99XXCycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		protected:
		/** A reference to the TMS9918 registers. */
		TMS99XXRegisters* _TMS99XXRegisters;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To show or no the main events that affects the visualization. */
		bool _showEvents;
		
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;

		/** The events that can be drawn
			associated to the movement of the raster line. */
		struct EventsStatus
		{
			// TODO
		};

		mutable EventsStatus _eventStatus;

	};

	// ---
	inline void TMS99XX::readGraphicInfo (unsigned short x, unsigned short y)
	{
		// TODO
	}

	/** The version para PAL systems. */
	class TMS9929A_PAL final : public TMS99XX
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		TMS9929A_PAL (unsigned int cF);
	};

	/** The version para NTSC systems. */
	class TMS9928A_NTSC final : public TMS99XX
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		TMS9928A_NTSC (unsigned int cF);
	};
}

#endif
  
// End of the file
/*@}*/
