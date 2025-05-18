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
	/** Base class used to produce the video signal. \n
		The speed of this chip is 10,74MHz. It generates a signal 3 times slower to the main chip. \n
		In this simlation this is done in the other way around, that is: the chip determines the speed the of the video chip. 
		This is the reason of one of the attributes at construction time (cF). \n 
		In the graphical video standard the horizontal raster takes 64us to go from one sode to the other. 
		It means 10,74 cycles/us * 64us/raster = 687 cycles/raster \n
		The horizonal raster is made up of, initially, 342 "pixels", so a pixels is drawn every 687/342 = 2cyles. \n
		That is the same whatever the visual system is, either PAL or NTSC. \n
		What is not equal is the number of vertical lines: in PAL is 313 and in NTSC is 262. \n
		See the way they have been calculated at the beginning of the cpp file. */
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
							{ return (_TMS99xxFamilyRegisters); }
		TMS99xxFamilyRegisters* registers ()
							{ return (_TMS99xxFamilyRegisters); }

		// The access to the chip can be throught out the pins connected to the outside...
		// ...What this actions do, will depend on the internal situation of the chip
		MCHEmul::UByte readRegister (unsigned char rId) const
							{ return (_TMS99xxFamilyRegisters -> readRegister (rId)); }
		MCHEmul::UByte peekRegister (unsigned char rId) const // Same that previous but not changind the content...
							{ return (_TMS99xxFamilyRegisters -> peekRegister (rId)); }
		void setRegister (unsigned char rId, const MCHEmul::UByte& v)
							{ _TMS99xxFamilyRegisters -> setRegister (rId, v); }

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
							{ return (_TMS99xxFamilyRegisters -> videoMemorySize ()); }
		const std::vector <MCHEmul::UByte>& videoMemory () const
							{ return (_TMS99xxFamilyRegisters -> videoMemory ()); }
		const MCHEmul::UByte& videoData (const MCHEmul::Address& pos) const
							{ return (_TMS99xxFamilyRegisters -> videoData (pos)); }
		void setVideoData (const MCHEmul::Address& pos, const MCHEmul::UByte& v)
							{ _TMS99xxFamilyRegisters -> setVideoData (pos, v); }
		unsigned char graphicMode () const
							{ return (_TMS99xxFamilyRegisters -> graphicMode ()); }

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
		std::vector <MCHEmul::UByte> patternNameTableSnapShot () const
							{ return (std::move (_TMS99xxFamilyRegisters -> patternNameTableSnapShot ())); }
		std::vector <MCHEmul::UByte> patternGenerationTableSnapShot () const
							{ return (std::move (_TMS99xxFamilyRegisters -> patternGenerationTableSnapShot ())); }
		std::vector <MCHEmul::UByte> colorNameTableSnapShot () const
							{ return (std::move (_TMS99xxFamilyRegisters -> colorNameTableSnapShot ())); }
		MCHEmul::Strings spriteDrawSnapShot (size_t nS) const // From 1 to 32
							{ return (std::move (_TMS99xxFamilyRegisters -> spriteDrawSnapShot (nS))); }
		MCHEmul::Strings spritesDrawSnapShot (const std::vector <size_t>& nS) const // From 1 to 32 or empty for everything...
							{ return (std::move (_TMS99xxFamilyRegisters -> spritesDrawSnapShot (nS))); }

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Do a specific action per raster line. */
		void actionPerRasterLineAndCyle ();
		/** Read the graphical info (when needed) and draw the graphics. \n
			Returns true when the graphics are read. \n
			That returns variable will be used to determine whether 
			the CPU has to be stopped or not. \n
			Returns true when a collision between two sprites were detected. */
		bool readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu);
		/** Draw the graphics, sprites and detect collisuions depending on the graphics mode. \n
			The tuple received is from the method readGraphicInfo in the Register class (@see)
			and it is used in all method below this one. \n
			The method returns true when a collision between two or more sprites was detected. */
		bool drawGraphicsSpritesAndDetectCollisions 
			(unsigned short x, unsigned short y, unsigned short xS, unsigned short yS,
			 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data);
		/** Invoked from the previous one. 
			The methods receive the position within the real screen and tuple with graphical info. */
		void drawGraphicsScreenGraphicsMode (unsigned short x, unsigned short y,
			 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data);
		void drawGraphicsScreenTextMode (unsigned short x, unsigned short y,
			 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data);
		void drawGraphicsScreenMulticolorMode (unsigned short x, unsigned short y,
			 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data);
		/** Returns true when drawing the collision between 2 sprites is detected. */
		bool drawSprites (unsigned short x, unsigned short y, unsigned short xS);
		/** Returns true when the pixels is being drawn, even if its color were 0 (transparent). */
		bool drawSprite (unsigned short x, unsigned short y, unsigned short xS, unsigned char nS);

		/** Draw the important events, in case this option is set. */
		void drawEvents ();

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugTMS9918ACycle (MCHEmul::CPU* cpu, unsigned int i);
		void debugVideoNoActive ();
		// -----

		protected:
		/** A reference to the TMS99xxFamily registers. */
		TMS99xxFamilyRegisters* _TMS99xxFamilyRegisters;
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

		// The information needed to paint the sprites
		// The only information that I have found, is that the sprite information is read at the beginning of every frame.
		// That information is "copied" into the internal registers, sno no matter whether it is changed during the visualization cycles
		// because it would affect what is have been read until the next frame!
		// I haven't found detailed information about the internals of the chip.
		struct SpriteInfo
		{
			public:
			SpriteInfo (const TMS99xxFamilyRegisters::SpriteDefinition& i = 
				TMS99xxFamilyRegisters::SpriteDefinition ())
				: _definition (i),
				  _visible (false), // When created none is visible...
				  _bytes ({ 0x00, 0x00 }) // Two bytes max in a sprite extended!
							{ }

			// The info of the sprite.
			TMS99xxFamilyRegisters::SpriteDefinition _definition;

			/** Load the information about the sprite. \n
				The info (bytes) that will be displayed if there were visible. \n
				It will return true if the sprite were visible in the line. \n 
				This method is invoked at the beginning oif every raster line. \n 
				The method receives also the size of the borders. */
			inline bool actualizeInfoAtVisibleLine
				(unsigned short y, unsigned short uB, unsigned short lB);

			// Implementation
			// This info is actualized with the previous method...
			/** To describe whether the sprite is or not visible. \n
				This attribute gets update at the beginning of every raster line within the visible zone. */
			bool _visible;

			/** The bytes (max 2) that will define the structure if the sprite were visible. */
			MCHEmul::UBytes _bytes;
		};

		// Related with the way the raster line works...
		/** This switch "marks" when the raster moves forward.
			The raster moves forward one pixel very 2 cycles. */
		bool _2CYCLE;
		// Related with the sprites
		/** This information is calculated at the beginning of every frame 
			and updated at beginning of every raster line. */
		std::vector <SpriteInfo> _spriteInfo;
	};

	// ---
	inline bool TMS99xxFamily::SpriteInfo::actualizeInfoAtVisibleLine 
		(unsigned short vL, unsigned short lB, unsigned short uB)
	{
		size_t bV;
		if ((_visible = _definition.visibleAtVisibleLine (vL, uB, bV)))
		{
			assert (!_definition._data.empty ()); // Just in case...

			_bytes [0] = _definition._data [bV];
			_bytes [1] = _definition._16pixels 
				? _definition._data [bV + 16] : MCHEmul::UByte::_0;
		}

		return (_visible);
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
