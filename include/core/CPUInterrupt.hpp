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

#include <map>

#include <core/Address.hpp>

namespace MCHEmul
{
	class CPU;

	/** A CPU Interrupt is something that is able to stop the normal progress of the cpu execution. */
	class CPUInterrupt
	{
		public:
		CPUInterrupt () = delete;

		CPUInterrupt (int id)
			: _id (id), _active (false /** by default. */)
							{ }

		CPUInterrupt (const CPUInterrupt&) = delete;

		virtual ~CPUInterrupt ()
							{ }

		CPUInterrupt& operator = (const CPUInterrupt&) = delete;

		int id () const
							{ return (_id); }

		bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ _active = a; }

		/** Receive the CPU the interrupts works for. \n
			It receives also a erference to a variabl where to load the number of cycles it took the execution (when the return is ok). \n
			It returns true if ok and false if not. */
		bool executeOver (CPU* c, unsigned int& nC);

		protected:
		// These methods are invoked by xecuteOver (defined above);
		/** To determine whether it is the time to execute the interruption. */
		virtual bool isTime (CPU* c) const = 0;
		/** To really execute the interrupt. 
			Receives as parameter the CPU the Interrupt works for. */
		virtual void executeOverImpl (CPU* c, unsigned int& nC) = 0;

		protected:
		int _id;
		bool _active;

		// Implementation
		mutable unsigned int _lastClockCyclesExecuted;
	};

	using CPUInterrups = std::map <int, CPUInterrupt*>;
}

#endif
  
// End of the file
/*@}*/


