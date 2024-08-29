/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Trap.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 27/08/2024 \n
 *	Description: Define a trap.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_TRAP__
#define __MCHEMUL_TRAP__

#include <CORE/global.hpp>
#include <CORE/Address.hpp>
#include <CORE/UByte.hpp>

namespace MCHEmul
{
	class Memory;

	/** In many ocassions a kernel routine can not be fully simulated. \n
		The reasons are usually because it is not possible either to simulate the hardware
		which that software is interacting with. \n
		It is then needed to simulate the full routine instead. \n
		A trap defines the point where that situation could happen in the memory. \n
		The traps are not used in any CORE classes by default, but they are a mechanisim in the hand of 
		the programmer. They might be used in a IODEvice simulation or in CPU specfici simulation e.g.
		*/
	struct Trap
	{
		/** Usually constructor using an initializer list. */
		int _id;									// For a quicker identification...
		std::string _name;							// Name of the trap (to recognize)
		Address _addressIn;							// PC where the trap has to enter...
		Address _addressOut;						// PC where the trap has to finishes!...
		std::vector <MCHEmul::UByte> _fingerTip;	// To verify that is the position expected actually!

		/** To verify the finger tip defined for the trap against the memory.
			Returs true if the finger tip matches (also when it is empty) and false in other circunstance. */
		bool verifyFingerTipAgainst (Memory* mem) const;

		/** Just to simplify the representation of the Trap in logs of info structures. */
		std::string asString () const
						{ return (std::to_string (_id) + ":" + _name + "(" +
							_addressIn.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0') + "," +
							_addressOut.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0') + ")"); }
	};

	/** To simplify the definition of a set of Traps. */
	using Traps = std::vector <Trap>;
}

#endif
  
// End of the file
/*@}*/
