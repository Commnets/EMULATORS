/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Pulse.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 06/01/2024 \n
 *	Description: To control when a pulse is generated.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_PULSE__
#define __MCHEMUL_PULSE__

#include <CORE/OBool.hpp>

namespace MCHEmul
{
	/** Class to control the generation of a pulse. \n
		When a pulse is set, the class determines whether the pulse wen from 0 to 1 or from 1 to 0,
		setting the OBool_F variables with the result. \n
		When that variables are checked to know whether one or another was generated, 
		their values goes back to false. */
	class Pulse final
	{
		public:
		Pulse (bool v)
			: _value (v),
			  _positiveEdge (false),
			  _negativeEdge (false)
							{ }

		/** Returns true if the new value is different than the old one, 
			and false is the value is the same. */
		inline bool set (bool v);

		bool value () const
							{ return (_value); }

		/** Returns true is a positive/negative edge has happened. \n
			Bear in mind that the automatic bool operator will be applied,
			so the value of _positiveEdge and _negativeEdge will go back to false after this method. */
		bool positiveEdge () const
							{ return (_positiveEdge); }
		bool negativeEdge () const
							{ return (_negativeEdge); }
		bool transition () const
							{ return (_positiveEdge || _negativeEdge); }
		/** Same method than the ones above but with 
			no changing the value of the variables requested. */
		bool peekPositiveEdge () const
							{ return (_positiveEdge.peekValue ()); }
		bool peekNegativeEdge () const
							{ return (_negativeEdge.peekValue ()); }
		bool peekTransition () const
							{ return (_positiveEdge.peekValue () || _negativeEdge.peekValue ()); }

		operator bool () const
							{ return (_value); }

		private:
		bool _value;

		// Implementation
		/** true when the pulse goes from false = 0 to true = 1. */
		OBool _positiveEdge;
		/** true when the pulse goes from true = 1 to false = 0. */
		OBool _negativeEdge;
	};

	// ---
	inline bool Pulse::set (bool v)
	{
		bool result = (_value != v);
		
		// Only if the value has changed, the edges are actualized...
		if (result)
		{
			_positiveEdge = !_value && v; // from false = 0 to true = 1
			_negativeEdge = _value && !v; // from true = 1 to false = 0
			_value = v;					  // ...and fibally the new value is adopted...
		}

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/