/** \ingroup FZ80 */
/*@{*/

/**
 * @file
 * File: MCycleTests.hpp \n
 * Framework: CPU Emulators library \n
 * Author: Ignacio Cea Fornies (EMULATORS library) \n
 * Creation Date: 16/10/2026 \n
 * Description: Structural checks for Z80 machine-cycle metadata. \n
 * Versions: 1.0 Initial.
 */

#ifndef __FZX80_MCYCLETESTS__
#define __FZX80_MCYCLETESTS__

#include <ostream>

namespace FZ80
{
	/** These tests validate metadata, not electrical Z80 timing or contention. */
	class MCycleTests final
	{
		public:
		static bool run (std::ostream& o);

		private:
		static bool testInstructionMetadata (std::ostream& o);
		static bool testInstructionMacros (std::ostream& o);
		static bool testAuxiliaryMacros (std::ostream& o);
		static bool testPrefixedLD (std::ostream& o);
		static bool testINCDEC (std::ostream& o);
		static bool testADDSUB (std::ostream& o);
		static bool testLogicalCP (std::ostream& o);
		static bool testALUCTRL (std::ostream& o);
		static bool testRotations (std::ostream& o);
		static bool testShifts (std::ostream& o);
		static bool testBIT (std::ostream& o);
		static bool testSETRES (std::ostream& o);
		static bool testSTACK (std::ostream& o);
		static bool testJUMP (std::ostream& o);
		static bool testCALL (std::ostream& o);
		static bool testEX (std::ostream& o);
		static bool testBlockTransferCompare (std::ostream& o);
		static bool testSimpleIO (std::ostream& o);
		static bool testBlockIO (std::ostream& o);
		static bool check (bool ok, const char* name, std::ostream& o);
	};
}

#endif

// End of the file
/*@}*/
