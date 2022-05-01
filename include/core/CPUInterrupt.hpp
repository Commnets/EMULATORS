/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: CPUInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: Defines a generic way to manage interrupts in a CPU.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPUINTERRUPT__
#define __MCHEMUL_CPUINTERRUPT__

#include <vector>

#include <core/Address.hpp>

namespace MCHEmul
{
	/** A CPU Interrup is something that is able to stop the normal progress of the cpu execution. */
	class CPUInterrupt
	{
		public:
		CPUInterrupt () = delete;

		CPUInterrupt (const Address& a, unsigned int nc)
			: _address (a), _everyClockCycles (nc), _active (false /** by default. */)
							{ }

		CPUInterrupt (const CPUInterrupt&) = delete;

		virtual ~CPUInterrupt ()
							{ }

		CPUInterrupt& operator = (const CPUInterrupt&) = delete;

		constexpr const Address& address () const
							{ return (_address); }
		constexpr unsigned int everyClockCycles () const
							{ return (_everyClockCycles); }
		constexpr bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ _active = a; }

		bool isTime (unsigned cc) const;

		private:
		Address _address;
		unsigned int _everyClockCycles;
		bool _active;

		// Implementation
		mutable unsigned int _lastClockCyclesExecuted;
	};

	using CPUInterrups = std::vector <CPUInterrupt*>;
}

#endif
  
// End of the file
/*@}*/


