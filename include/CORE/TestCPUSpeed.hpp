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
#include <CORE/Instruction.hpp>

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

		/** See info below. 
			This is a similar instruction just to test all instruction set. */
		void testAllInstructionSet (std::ostream& o, unsigned int nt = 1, bool sM = false, bool pS = false) const;

		private:
		/** 
		  *	To ckeck the full set of instructions. 
		  *	The method receives:
		  *	@param o		: A reference to the stream where to output the info.
		  *	@param inst		: The instruction set to test. It will be recursive.
		  *	@param sPI		: This is a reference to a vector that are filled up with speeds per instruction.
		  *	@param iT		: This is a reference to a vector that are filled up with the aspect of avery instruction.
		  *	@param nt		: Number of times every instruction has to be tested. 
		  *	@param sM		: Whether to simulate or not the access to the memory. False by default.
		  *	@param pS		: Whether to print out or not the status of the system after every execution. False by default. 
		  */
		void testInstructionSet (std::ostream& o, const Instructions& inst, 
			std::map <unsigned int, unsigned int>& sPI, std::map <unsigned int, std::string>& iT,
			unsigned int nt = 1, bool sM = false, bool pS = false) const;

		// Implementation
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

