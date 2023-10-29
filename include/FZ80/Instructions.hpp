/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Instructions.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included. \n
 *				 Only the stable ones. \n
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INSTRUCTIONS__
#define __FZX80_INSTRUCTIONS__

#include <CORE/incs.hpp>
#include <FZ80/CZ80.hpp>

namespace FZ80
{
	class Instruction : public MCHEmul::Instruction
	{
		public:
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: MCHEmul::Instruction (c, mp, cc, t, false)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
