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
		static bool check (bool ok, const char* name, std::ostream& o);
	};
}

#endif

// End of the file
/*@}*/
