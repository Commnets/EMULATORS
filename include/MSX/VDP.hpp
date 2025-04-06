/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: VDP.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: The VDP Chip.
 *				 This chip is used to produce video signal. 
 *				 It is used in many computers, specially in the standard MSX.
 *	Versions: 1.0 Initial
 *  Based on the standard (toshiba): https://www.cs.columbia.edu/~sedwards/papers/TMS9918.pdf
 */

#ifndef __MSX_VDP__
#define __MSX_VDP__

#include <CORE/incs.hpp>
#include <MSX/VDPBehaviour.hpp>

namespace MSX
{
	/** Base class used to produce the video signal. 
		The basic structure is valid for all versions of MSX system: MSX1, MSX2,...*/
	class VDP : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 210;

		VDP (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			 unsigned int cF,
			 VDPBehaviour* vdpR /** The VDP will be owner of the VDPBehaviour */);

		virtual ~VDP () override;

		/** Managing the registers. */
		const VDPBehaviour* behaviour () const
							{ return (_VDPBehaviour); }
		VDPBehaviour* behaviour ()
							{ return (_VDPBehaviour); }

		/** What the chip does when a port is requested/set might 
			depend on the generation of the MSX, that here is reflected within the Behaviour object. 
			The portId starts from 0x00 to 0x06. */
		MCHEmul::UByte readPortValue (unsigned char prtId) const
							{ return (_VDPBehaviour -> readPortValue (prtId)); }
		void setPortValue (unsigned char prtId, const MCHEmul::UByte& v)
							{ _VDPBehaviour -> setPortValue (prtId, v); }

		/** Size & position of the screen... */
		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always within the visible screen. */
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
		  * Behaviour	= InfoStructure: Info about the Behaviour.
		  * Raster		= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		/** To get snapshots of the graphical memory. 
			The info returned will depend on the graphic mode connected. 
			And the number of graphic modes will depend on the MSX generation. */
		MCHEmul::UBytes screenMemorySnapShot () const
							{ return (_VDPBehaviour -> screenMemorySnapShot ()); }
		MCHEmul::UBytes colorMemorySnapShot () const
							{ return (_VDPBehaviour -> colorMemorySnapShot ()); }

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
		void debugVDPCycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		protected:
		/** A reference to the VDP registers. */
		VDPBehaviour* _VDPBehaviour;
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
	inline void VDP::readGraphicInfo (unsigned short x, unsigned short y)
	{
		// TODO
	}

	/** The version of the VDP for PAL systems. */
	class VDP_PAL final : public VDP
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VDP_PAL (unsigned int cF, VDPBehaviour* vdpR)
			: VDP (_VRASTERDATA, _HRASTERDATA, cF, vdpR)
							{ }
	};

	/** The version for NTSC systems. */
	class VDP_NTSC final : public VDP
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VDP_NTSC (unsigned int cF, VDPBehaviour* vdpR)
			: VDP (_VRASTERDATA, _HRASTERDATA, cF, vdpR)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
