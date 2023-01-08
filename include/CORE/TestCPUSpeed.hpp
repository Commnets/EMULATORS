/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: TestCPUSpeed.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2021 \n
 *	Description: To test how quiick the CPU is.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_TESTCPU__
#define __MCHEMUL_TESTCPU__

#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	class CPU;
	class Memory;
	class TestCPUSpeed final
	{
		public:
		TestCPUSpeed () = delete;

		TestCPUSpeed (CPU* cpu, Memory* mem)
			: _cpu (cpu), _memory (mem)
							{ assert (_cpu != nullptr && _memory != nullptr); }

		TestCPUSpeed (const TestCPUSpeed&) = delete;

		TestCPUSpeed& operator = (const TestCPUSpeed&) = delete; 

		~TestCPUSpeed ();

		TestCPUSpeed (TestCPUSpeed&&) = delete;

		TestCPUSpeed& operator = (TestCPUSpeed&&) = delete; 

		/** Number of clocks cycles lasted in a second executing the same instruction. \n
			The instruction is read from the memory and 
			all info about it has to be in the data received as parameter. \n
			The parameter sM is true when an access to memory has to be simulated too. */
		unsigned int clocksInASecondExcutingInstruction (const UBytes& b, bool sM = false) const;

		/** To ckeck the full set of instructions. 
			The method receives also the number of times to test every instruction,
			Whether to simulate access to memory and whether to output the status of the cpu after test. */
		void testInstructionSet (std::ostream& o, unsigned int nt = 1, bool sM = false, bool pS = false) const;

		private:
		std::vector <UByte> randomVector (size_t i) const;
		void printOutCPUStatus (std::ostream& o) const;

		private:
		CPU* _cpu;
		Memory *_memory;
	};
}

#endif
  
// End of the file
/*@}*/

