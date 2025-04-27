/** \ingroup TEXASINSTRUMENTS */
/*@{*/

/**	
 *	@file	
 *	File: TMS99xxFamily.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: The TMS99xxFamily Chip.
 *				 This chip is used to produce video signal. 
 *				 It is used in many computers, specially in the standard MSX (Generation 1).
 *	Versions: 1.0 Initial
 *  Based on the standard (toshiba): https://www.cs.columbia.edu/~sedwards/papers/TMS9918.pdf
 */

#ifndef __TEXASINSTRUMENTS_TMS99XXFAMILY__
#define __TEXASINSTRUMENTS_TMS99XXFAMILY__

#include <CORE/incs.hpp>
#include <TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamilyRegisters.hpp>

namespace TEXASINSTRUMENTS
{
	/** Base class used to produce the video signal. */
	class TMS99xxFamily : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 1120;

		/** The register of the chip can be provided from outside. \n	
			It not, a temporal ones is created in this constructor and they are owned by this class. \n
			It is important to notice that the constructor received two important attributes.
			@param cF	How many times is the video chip quicker than the CPU. 
			@param iId	The id of the interrupt to launch over the CPU when the raster reached the end of the visible screen. */
		TMS99xxFamily (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
			TMS99xxFamilyRegisters* reg, // It can be nullptr, and then a temporal one will be created...
			unsigned int cF /** How much quicker (times) the video chip is compared with the CPU. */, 
			int iId, /** Interrupt Id to launch when possible */
			const MCHEmul::Attributes& attrs);

		/** Destriy the temporal register class is it was created inside as temporal. */
		virtual ~TMS99xxFamily () override;

		// Ta access to the internal registers...
		// ...although, you shouldn't do that...
		const TMS99xxFamilyRegisters* registers () const
							{ return (_TMS9918ARegisters); }
		TMS99xxFamilyRegisters* registers ()
							{ return (_TMS9918ARegisters); }

		// The access to the chip can be throught out the pins connected to the outside...
		// ...What this actions do, will depend on the internal situation of the chip
		MCHEmul::UByte readRegister (unsigned char rId) const
							{ return (_TMS9918ARegisters -> readRegister (rId)); }
		MCHEmul::UByte peekRegister (unsigned char rId) const // Same that previous but not changind the content...
							{ return (_TMS9918ARegisters -> peekRegister (rId)); }
		void setRegister (unsigned char rId, const MCHEmul::UByte& v)
							{ _TMS9918ARegisters -> setRegister (rId, v); }

		/** Size & position of the screen. */
		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always within the visible screen. */
		inline void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2)
							{ _raster.displayPositions (x1, y1, x2, y2); }

		// Manages the internal video memory
		// Relies in the registers...
		// Take care because no boundaries checks are done...
		size_t videoMemorySize () const
							{ return (_TMS9918ARegisters -> videoMemorySize ()); }
		const std::vector <MCHEmul::UByte>& videoMemory () const
							{ return (_TMS9918ARegisters -> videoMemory ()); }
		const MCHEmul::UByte& videoData (const MCHEmul::Address& pos) const
							{ return (_TMS9918ARegisters -> videoData (pos)); }
		void setVideoData (const MCHEmul::Address& pos, const MCHEmul::UByte& v)
							{ _TMS9918ARegisters -> setVideoData (pos, v); }

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
		  * The ones from the parent company plus:
		  * TMS99xxFamilyRegisters	= InfoStructure: Info about the registers.
		  * Raster				= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		/** To get snapshots of the graphical memory. 
			The info returned will depend on the active graphic mode. */
		MCHEmul::UBytes screenMemorySnapShot () const
							{ return (_TMS9918ARegisters -> screenMemorySnapShot ()); }
		MCHEmul::UBytes colorMemorySnapShot () const
							{ return (_TMS9918ARegisters -> colorMemorySnapShot ()); }

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
		void debugTMS9918ACycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		protected:
		/** A reference to the TMS99xxFamily registers. */
		TMS99xxFamilyRegisters* _TMS9918ARegisters;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** The number of cycles to be executed by every CPU clycle. \n
			That is how much quicker the video chip is compared with the CPU. */
		unsigned int _clockFactor;
		/** The id of the interrupt to launch when enabled. */
		int _interruptId;
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

		// Implementation
		/** When the Registers are created internally because they will be accesed always through out the chip
			and this one froma port and not like a memory address (that belongs to the computer) this
			variable is set and must be deleted. */
		TMS99xxFamilyRegisters* _internalRegisters;
	};

	// ---
	inline void TMS99xxFamily::readGraphicInfo (unsigned short x, unsigned short y)
	{
		// TODO
	}

	/** The version of the TMS99xxFamily for PAL systems. */
	class TMS9929A final : public TMS99xxFamily
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		TMS9929A (TMS99xxFamilyRegisters* reg, unsigned int cF, int iId);
	};

	/** The version of the TMS99xxFamily for NTSC systems. 
		There was also a TMS9928A with some differences in the way the video signal was generated,
		but it is not taking into account here for the simuilation .*/
	class TMS9918A final : public TMS99xxFamily
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		TMS9918A (TMS99xxFamilyRegisters* reg, unsigned int cF, int iId);
	};
}

#endif
  
// End of the file
/*@}*/
