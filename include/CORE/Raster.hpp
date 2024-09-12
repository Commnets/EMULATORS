/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Raster.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: The Raster info in a usually a catodic ray screen.
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

		// The default copy constructors are ok...

		unsigned short currentPosition () const
						{ return (_currentPosition); }
		unsigned short previousPosition () const // Without affecting the current one...
						{ RasterData c = *this; c.add (-1); return (c.currentPosition ()); }
		unsigned short nextPosition () const // Without affecting the current one...
						{ RasterData c = *this; c.add (1); return (c.currentPosition ()); }
		unsigned short currentPositionAtBase0 () const
						{ return (_currentPosition_0); }
		unsigned short nextPositionAtBase0 () const
						{ RasterData c = *this; c.add (1); return (c.currentPositionAtBase0 ()); }

		/** To restart it. No calculus are done. */
		void reset ()
						{ _currentPosition = _firstPosition; 
						 _currentPosition_0 = _firstPosition_0; }

		// Managing the blank zone...
		bool isInBlankZone () const
						{ return ((_currentPosition_0 >= _firstPosition_0 && 
										_currentPosition_0 < _firstVisiblePosition_0) ||
								  (_currentPosition_0 > _lastVisiblePosition_0 && 
										_currentPosition_0 <= _lastPosition_0)); }
		bool isInFirstBlankZone () const
						{ return (_currentPosition_0 >= _firstPosition_0 && 
								  _currentPosition_0 < _firstVisiblePosition_0); }
		bool isInLastBlankZone () const
						{ return (_currentPosition_0 > _lastVisiblePosition_0 && 
								  _currentPosition_0 <= _lastPosition_0); }

		// Managing the visible zone...
		bool isInVisibleZone () const 
						{ return ((_currentPosition_0 >= _firstVisiblePosition_0 && 
								   _currentPosition_0 <= _lastVisiblePosition_0)); }
		unsigned short currentVisiblePosition () const 
						{ return (_currentPosition_0 - _firstVisiblePosition_0); }
		unsigned short lastVisiblePosition () const
						{ return (_lastVisiblePosition_0 - _firstVisiblePosition_0); }
		unsigned short visiblePositions () const
						{ return (_lastVisiblePosition_0 - _firstVisiblePosition_0 + 1); }
		unsigned short positionInVisibleZone (unsigned short p) const
						{ return (toBase0 (p) - _firstVisiblePosition_0); }

		// Managing the display
		/** The DISPLAY is the zone where the drawing can happen but not taking 
			into account potential reductions neither in the columns nor in the rows. */
		bool isInDisplayZone () const
						{ return (_currentPosition_0 >= _originalFirstDisplayPosition_0 && 
								  _currentPosition_0 <= _originalLastDisplayPosition_0); }
		// Respect the origin of the display position...
		// It might be enourmous if _currentPosition < _originalFirstDisplayPosition
		unsigned short currentDisplayPosition () const 
						{ return (_currentPosition_0 - _originalFirstDisplayPosition_0); }
		// Respect the origin of the visible position...
		unsigned short firstDisplayPosition () const 
						{ return (_firstDisplayPosition_inVisibleZone); }
		unsigned short lastDisplayPosition () const
						{ return (_lastDisplayPosition_inVisibleZone); }
		// Absolute value...
		unsigned short displayPositions () const
						{ return (_originalLastDisplayPosition_0 - _originalFirstDisplayPosition_0 + 1); }
		// Respect the origin of the display position...
		// It might be enormous if p (in base 0) < _originalFirstDisplayPosition
		unsigned short positionInDisplayZone (unsigned short p) const
						{ return (toBase0 (p) - _originalFirstDisplayPosition_0); }

		// Managing the screen
		/** The SCREEN is the zone where the drawing can happen but taking
			into account potential reductions either in the columns on in the rows. */
		// Applu the same rational to this values than to display's ones...
		bool isInScreenZone () const
						{ return (_currentPosition_0 >= _firstDisplayPosition_0 &&
								  _currentPosition_0 <= _lastDisplayPosition_0); }
		unsigned short currentScreenPosition () const 
						{ return (_currentPosition_0 - _firstDisplayPosition_0); }
		unsigned short firstScreenPosition () const
						{ return (_firstScreenPosition_inVisibleZone); }
		unsigned short lastScreenPosition () const
						{ return (_lastScreenPosition_inVisibleZone); }
		unsigned short screenPositions () const
						{ return (_lastDisplayPosition_0 - _firstDisplayPosition_0 + 1); }
		unsigned short positionInScreenZone (unsigned short p) const
						{ return (toBase0 (p) - _firstDisplayPosition_0); }

		/** Returns true when the limit of the raster is reached. 
			The parameter is the number of positions to increment the rasterData. */
		inline bool add (int i);
		bool next ()
						{ return (add (1)); }
		/** It does a simulation of how many positions will overpass the limit when adding i more. 
			If no overpass were calculated the number returned will be -1. */
		inline int simulateAdd (int i);

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
		// To speed up even more...
		unsigned short _firstDisplayPosition_inVisibleZone;
		unsigned short _lastDisplayPosition_inVisibleZone;
		unsigned short _firstScreenPosition_inVisibleZone;
		unsigned short _lastScreenPosition_inVisibleZone;

		// Implementation
		unsigned short _currentPosition;
		unsigned short _currentPosition_0;
		bool _displayZoneReduced;
	};

	// ---
	inline bool RasterData::add (int i)
	{
		bool result = false;

		// Move to the next or previous position 
		// and adjust it to the limits if needed...
		int cP = (int) _currentPosition_0 + i;
		if (result = (cP >= (int) _maxPositions))
			while (cP >= (int) _maxPositions)
				cP -= (int) _maxPositions;
		else
		if (result = (cP < (int) 0))
			while (cP < 0)
				cP += (int) _maxPositions;
		_currentPosition_0 = (unsigned short) cP;

		cP += (int) _firstPosition; // Now is no longer in base 0...
		if (cP >= (int) _maxPositions)
			cP -= (int) _maxPositions;
		_currentPosition = (unsigned short) cP;

		return (result);
	}

	// ---
	inline int RasterData::simulateAdd (int i)
	{
		int result = -1;

		int cP = (int) _currentPosition_0 + i;
		if ((cP >= (int) _maxPositions))
			result = _maxPositions - cP;

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

		// Recalculate the very basic numbers...
		_firstDisplayPosition_inVisibleZone	= 
			_originalFirstDisplayPosition_0 - _firstVisiblePosition_0;
		_lastDisplayPosition_inVisibleZone	= 
			_originalLastDisplayPosition_0 - _firstVisiblePosition_0;
		_firstScreenPosition_inVisibleZone	= 
			_firstDisplayPosition_0 - _firstVisiblePosition_0;
		_lastScreenPosition_inVisibleZone	= 
			_lastDisplayPosition_0 - _firstVisiblePosition_0;
	}

	/** The Raster simulates the set of sequential horizontal lines that, 
		in a CRT monitor, draws an image in the screen. */
	class Raster final : public InfoClass
	{
		public:
		Raster () = delete;

		Raster (const RasterData& vD, const RasterData& hD, unsigned char st = 8)
			: InfoClass ("Raster"),
				_vRasterData (vD), _hRasterData (hD),
			    _step (st)
						{ }

		const RasterData& vData () const
						{ return (_vRasterData); }
		RasterData& vData ()
						{ return (_vRasterData); }
		const RasterData& hData () const
						{ return (_hRasterData); }
		RasterData& hData ()
						{ return (_hRasterData); }
		unsigned char step () const
						{ return (_step); }

		/** Horizontal & Vertical positions are moved back to the beginning. */
		void reset ()
						{ _vRasterData.reset (); _hRasterData.reset (); }

		unsigned short currentLine () const
						{ return (_vRasterData.currentPosition ()); }
		unsigned short previousLine () const
						{ return (_vRasterData.previousPosition ()); }
		unsigned short nextLine () const
						{ return (_vRasterData.nextPosition ()); }
		unsigned short currentLineAtBase0 () const
						{ return (_vRasterData.currentPositionAtBase0 ()); }
		unsigned short nextLineAtBase0 () const
						{ return (_vRasterData.nextPositionAtBase0 ()); }
		unsigned short currentColumn () const
						{ return (_hRasterData.currentPosition ()); }
		unsigned short currentColumnAtBase0 () const
						{ return (_hRasterData.currentPositionAtBase0 ()); }

		// Managing the blan zone
		bool isInVBlankZone () const
						{ return (_vRasterData.isInBlankZone ()); }
		bool isInFirstVBlankZone () const
						{ return (_vRasterData.isInFirstBlankZone ()); }
		bool isInLastVBlankZone () const
						{ return (_vRasterData.isInLastBlankZone ()); }

		// Managing the visible zone
		bool isInVisibleZone () const
						{ return (_vRasterData.isInVisibleZone () && _hRasterData.isInVisibleZone ()); }
		unsigned short visibleLines () const
						{ return (_vRasterData.visiblePositions ()); }
		unsigned short visibleColumns () const
						{ return (_hRasterData.visiblePositions ()); }
		void currentVisiblePosition (unsigned short& x, unsigned short& y) const
						{ x = _hRasterData.currentVisiblePosition (); y = _vRasterData.currentVisiblePosition (); }
		unsigned short lineInVisibleZone (unsigned short l) const
						{ return (_vRasterData.positionInVisibleZone (l)); }
		unsigned short columnInVisibleZone (unsigned short c) const
						{ return (_hRasterData.positionInVisibleZone (c)); }

		// Managing the display zone
		/** The display is where drawing is possible. The reduced zones if any are not considered. */
		bool isInDisplayZone () const
						{ return (_vRasterData.isInDisplayZone () && _hRasterData.isInDisplayZone ()); }
		void firstDisplayPosition (unsigned short& x, unsigned short& y) const
						{ x = _hRasterData.firstDisplayPosition (); y = _vRasterData.firstDisplayPosition (); }
		void displayPositions (unsigned short& x1, unsigned short& y1, unsigned short& x2, unsigned short& y2) const
						{ x1 = _hRasterData.firstDisplayPosition (); y1 = _vRasterData.firstDisplayPosition ();
						  x2 = _hRasterData.lastDisplayPosition (); y2 = _vRasterData.lastDisplayPosition (); }
		void currentDisplayPosition (unsigned short& x, unsigned short& y) const
						{ x = _hRasterData.currentDisplayPosition (); y = _vRasterData.currentDisplayPosition (); }
		unsigned short lineInDislayZone (unsigned short l) const
						{ return (_vRasterData.positionInDisplayZone (l)); }
		unsigned short columnInDisplayZone (unsigned short c) const
						{ return (_hRasterData.positionInDisplayZone (c)); }

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
			The raster moves _step pixels per cycle. */
		inline bool moveCycles (unsigned short nC);
		/** Similates whether the raster moves to the next line after adding nC cycles. 
			Returns true when the raster should move to another line, and false if not. */
		bool simulateMoveCycles (unsigned short nC)
						{ return (_hRasterData.simulateAdd (nC * _step) != -1); }

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
		unsigned char _step;
	};

	// ---
	inline bool Raster::moveCycles (unsigned short nC)
	{ 
		bool result = _hRasterData.add (nC * (unsigned short) _step /** columuns = piexels per cycle. */);
		if (result) _vRasterData.next (); 
		return (result); 
	}
}

#endif
  
// End of the file
/*@}*/
