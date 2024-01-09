/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: OBool.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 06/01/2024 \n
 *	Description: One time bool.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_OBOOL__
#define __MCHEMUL_OBOOL__

namespace MCHEmul
{
	/** This class behaves like a bool,
		but any time its value (and in any way) is requested, 
		it sets back to FALSE after returning its value. */
	class OBool final
	{
		public:
		OBool (bool b)
			: _value (b)
							{ }

		// Copy constructor,... by default
		// The only way to change its value is from another OBool (using default equal assignment: OBool a = OBool b),
		// or from another bool (constructor + assignment: OBool a = false/true)

		bool value () const
							{ bool r = _value; _value = false; return (r); }
		/** This is the only way to access the value of the variable and do not change its value. */
		bool peekValue () const
							{ return (_value); }

		/** To manage its value like any other bool always,
			But remember that doing so, its value comes back to false. */
		operator bool () const
							{ return (value ()); }

		private:
		mutable bool _value;
	};
}

#endif
  
// End of the file
/*@}*/