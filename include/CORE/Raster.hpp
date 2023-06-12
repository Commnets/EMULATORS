/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Raster.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: The Raster info in a usually a cotodic ray screen.
 *	Versions: 1.0 Initial
 *	Based on https://www.cebix.net/VIC-Article.txt.
 */

#ifndef __CORE_RASTER__
#define __CORE_RASTER__

#include <CORE/incs.hpp>

namespace MCHEmul
{
	/** The RasterData describes the infomation the raster need to move across the screen. */
	class RasterData final : public MCHEmul::InfoClass
	{
		public:
		RasterData () = delete;

		RasterData (
					unsigned short fp,		// First position
					unsigned short fvp,		// First Visible position
					unsigned short fdp,		// First Display position
					unsigned short ldp,		// Last Display position
					unsigned short lvp,		// Last Visible position
					unsigned short lp,		// Last position
					unsigned short mp,		// Maximum positions
					unsigned short pr1,		// Positions to reduce in the visible zone 1 & 2
					unsigned short pr2
				  );

		unsigned short currentPosition () const
						{ return (_currentPosition); }
		unsigned short currentPositionAtBase0 () const
						{ return (_currentPosition_0); }

		// Managing the blank zone...
		bool isInBlankZone () const
						{ return ((_currentPosition_0 >= _firstPosition_0 && 
										_currentPosition_0 < _firstVisiblePosition_0) ||
								  (_currentPosition_0 > _lastVisiblePosition_0 && 
										_currentPosition_0 <= _lastPosition_0)); }
		bool isInLastBlankZone () const
						{ return (_currentPosition_0 > _lastVisiblePosition_0 && 
								  _currentPosition_0 <= _lastPosition_0); }

		// Managin the visible zone...
		bool isInVisibleZone () const 
						{ return ((_currentPosition_0 >= _firstVisiblePosition_0 && 
								   _currentPosition_0 <= _lastVisiblePosition_0)); }
		unsigned short currentVisiblePosition () const // The 
						{ return (_currentPosition_0 - _firstVisiblePosition_0); }
		unsigned short lastVisiblePosition () const
						{ return (_lastVisiblePosition_0 - _firstVisiblePosition_0); }
		unsigned short visiblePositions () const
						{ return (_lastVisiblePosition_0 - _firstVisiblePosition_0 + 1); }

		// Managing the display
		/** The DISPLAY is the zone where the drawing can happen but not taking 
			into account potential reductions neither in the columns nor in the rows. */
		bool isInDisplayZone () const
						{ return (_currentPosition_0 >= _originalFirstDisplayPosition_0 && 
								  _currentPosition_0 <= _originalLastDisplayPosition_0); }
		unsigned short firstDisplayPosition () const
						{ return (_originalFirstDisplayPosition_0 - _firstVisiblePosition_0); }
		unsigned short lastDisplayPosition () const
						{ return (_originalLastDisplayPosition_0 - _firstVisiblePosition_0); }
		unsigned short displayPositions () const
						{ return (_originalLastDisplayPosition_0 - _originalFirstDisplayPosition_0 + 1); }

		// Managing the screen
		/** The SCREEN is the zone where the drawing can happen but taking
			into account potential reductions either in the columns on in the rows. */
		bool isInScreenZone () const
						{ return (_currentPosition_0 >= _firstDisplayPosition_0 &&
								  _currentPosition_0 <= _lastDisplayPosition_0); }
		unsigned short firstScreenPosition () const
						{ return (_firstDisplayPosition_0 - _firstVisiblePosition_0); }
		unsigned short lastScreenPosition () const
						{ return (_lastDisplayPosition_0 - _firstVisiblePosition_0); }
		unsigned short screenPositions () const
						{ return (_lastDisplayPosition_0 - _firstDisplayPosition_0 + 1); }

		/** Returns true when the limit of the raster is reached. 
			The parameter is the number of positions to increment the rasterData. */
		inline bool add (unsigned short i);
		bool next ()
						{ return (add (1)); }

		/** The display zone will reduced in both sides by half of the _positionsToReduce value. */
		inline void reduceDisplayZone (bool s);
		bool isDisplayZoneReduced () const
						{ return (_displayZoneReduced); }

		void initialize ()
						{ _currentPosition = _firstPosition; _currentPosition_0 = _firstPosition_0; }

		/**
		  *	The name of the fields are: \n
		  * POSITION	= Attribute: Position of the raster.
		  * POSITION_0	= Attribute: Position of the raster in base 0.
		  * FIRST		= Attribute: Initial position of the raster.
		  * LAST		= Attribute: Last position of the raster.
		  */
		virtual InfoStructure getInfoStructure () const override;

		friend std::ostream& operator << (std::ostream& o, const RasterData& r)
						{ return (o << *((InfoClass*) &r)); }

		protected:
		/** Internal method used return a value considering the firrst position as 0. */
		unsigned short toBase0 (unsigned short m) const
						{ int t = (int) m - (int) _firstPosition; 
						  return ((t < 0) ? (unsigned short) t + _maxPositions : (unsigned short) t); }

		protected:
		const unsigned short _firstPosition;
		const unsigned short _firstVisiblePosition;
		unsigned short _firstDisplayPosition;
		const unsigned short _originalFirstDisplayPosition;
		unsigned short _lastDisplayPosition;
		const unsigned short _originalLastDisplayPosition;
		const unsigned short _lastVisiblePosition;
		const unsigned short _lastPosition;
		const unsigned short _maxPositions;
		const unsigned short _positionsToReduce1;
		const unsigned short _positionsToReduce2;

		// Implementation
		// To speed up calculus... They are also set at construction time.
		unsigned short _firstPosition_0; 
		unsigned short _firstVisiblePosition_0;
		unsigned short _firstDisplayPosition_0; 
		unsigned short _originalFirstDisplayPosition_0; 
		unsigned short _lastDisplayPosition_0;
		unsigned short _originalLastDisplayPosition_0; 
		unsigned short _lastVisiblePosition_0;
		unsigned short _lastPosition_0;

		// Implementation
		unsigned short _currentPosition;
		unsigned short _currentPosition_0;
		bool _displayZoneReduced;
	};

	// ---
	inline bool RasterData::add (unsigned short i)
	{
		bool result = false;

		int cP = (int) _currentPosition_0;
		cP += (int) i; // Can move to the next (o nexts) lines...
		if (result = (cP >= (int) _maxPositions))
			while (cP >= (int) _maxPositions)
				cP -= (int) _maxPositions;

		cP += (int) _firstPosition; // Now is no longer in base 0...
		if (cP >= (int) _maxPositions)
			cP -= (int) _maxPositions;

		_currentPosition = (unsigned short) cP;
		_currentPosition_0 = toBase0 (_currentPosition);

		return (result);
	}

	// ---
	inline void RasterData::reduceDisplayZone (bool s)
	{
		if (_displayZoneReduced == s)
			return; // If nothing changes, nothing to do...

		if (_displayZoneReduced = s)
		{
			_firstDisplayPosition	+= _positionsToReduce1;
			_firstDisplayPosition_0	+= _positionsToReduce1;
			_lastDisplayPosition	-= _positionsToReduce2;
			_lastDisplayPosition_0	-= _positionsToReduce2;
		}
		else
		{
			_firstDisplayPosition	-= _positionsToReduce1;
			_firstDisplayPosition_0	-= _positionsToReduce1;
			_lastDisplayPosition	+= _positionsToReduce2;
			_lastDisplayPosition_0	+= _positionsToReduce2;
		}
	}

	/** The Raster simulates the set of sequential horizontal lines that, 
		in a CRT monitor, draws an image in the screen. */
	class Raster final : public InfoClass
	{
		public:
		Raster () = delete;

		Raster (const RasterData& vD, const RasterData& hD)
			: InfoClass ("Raster"),
				_vRasterData (vD), _hRasterData (hD)
						{ }

		const RasterData& vData () const
						{ return (_vRasterData); }
		RasterData& vData ()
						{ return (_vRasterData); }
		const RasterData& hData () const
						{ return (_hRasterData); }
		RasterData& hData ()
						{ return (_hRasterData); }

		unsigned short currentLine () const
						{ return (_vRasterData.currentPosition ()); }
		unsigned short currentLineAtBase0 () const
						{ return (_vRasterData.currentPositionAtBase0 ()); }
		unsigned short currentColumn () const
						{ return (_hRasterData.currentPosition ()); }
		unsigned short currentColumnAtBase0 () const
						{ return (_hRasterData.currentPositionAtBase0 ()); }

		// Managing the blan zone
		bool isInVBlank () const
						{ return (_vRasterData.isInBlankZone ()); }
		bool isInLastVBlank () const
						{ return (_vRasterData.isInLastBlankZone ()); }

		// Managing the visible zone
		/** The visible zone is the complete c64 sreen. 
			The size will be different in PAL and in NTSC. */
		bool isInVisibleZone () const
						{ return (_vRasterData.isInVisibleZone () && _hRasterData.isInVisibleZone ()); }
		unsigned short visibleLines () const
						{ return (_vRasterData.visiblePositions ()); }
		unsigned short visibleColumns () const
						{ return (_hRasterData.visiblePositions ()); }
		void currentVisiblePosition (unsigned short& x, unsigned short& y) const
						{ x = _hRasterData.currentVisiblePosition (); y = _vRasterData.currentVisiblePosition (); }

		// Managing the display zone
		/** The display is where drawing is possible. The reduced zones if any are not considered. */
		bool isInDisplayZone () const
						{ return (_vRasterData.isInDisplayZone () && _hRasterData.isInDisplayZone ()); }
		void firstDisplayPosition (unsigned short& x, unsigned short& y) const
						{ x = _hRasterData.firstDisplayPosition (); y = _vRasterData.firstDisplayPosition (); }
		void displayPositions (unsigned short& x1, unsigned short& y1, unsigned short& x2, unsigned short& y2)
						{ x1 = _hRasterData.firstDisplayPosition (); y1 = _vRasterData.firstDisplayPosition ();
							x2 = _hRasterData.lastDisplayPosition (); y2 = _vRasterData.lastDisplayPosition (); }

		/** To go from the display zone to the screen one. */
		void reduceDisplayZone (bool v, bool h)
						{ _vRasterData.reduceDisplayZone (v); _hRasterData.reduceDisplayZone (h); }

		// Managing the screen
		/** The screen is where the drawing is possible, considering the reduced zones if any.
			If there hadn't reduced zones the screen and the display would be equivalent. */
		bool isInScreenZone () const
						{ return (_vRasterData.isInScreenZone () && _hRasterData.isInScreenZone ()); }
		void firstScreenPosition (unsigned short& x, unsigned short& y, bool o = false) const
						{ x = _hRasterData.firstScreenPosition (); y = _vRasterData.firstScreenPosition (); }
		void screenPositions (unsigned short& x1, unsigned short& y1, unsigned short& x2, unsigned short& y2) const
						{ x1 = _hRasterData.firstScreenPosition (); y1 = _vRasterData.firstScreenPosition ();
							x2 = _hRasterData.lastScreenPosition (); y2 = _vRasterData.lastScreenPosition (); }
			
		/** Returns true when the raster goes to the next line. \n
			The Parameter is the number of cycles to move the raster. \n
			The raster moves 8 pixels per cycle. */
		bool moveCycles (unsigned short nC)
						{ bool result = _hRasterData.add (nC * 8 /** columuns = piexels per cycle. */);
							if (result) _vRasterData.next (); 
							return (result); }

		void initialize ()
						{ _vRasterData.initialize (); _hRasterData.initialize (); }

		/**
		  *	The name of the fields are: \n
		  * RasterX		= InfoStructure: Horizontal raster info.
		  * RasterY		= InfoStructure: Vertical raster info.
		  */
		virtual InfoStructure getInfoStructure () const override;

		friend std::ostream& operator << (std::ostream& o, const Raster& r)
						{ return (o << *((InfoClass*) &r)); }

		private:
		RasterData _vRasterData, _hRasterData;
	};
}

#endif
  
// End of the file
/*@}*/
