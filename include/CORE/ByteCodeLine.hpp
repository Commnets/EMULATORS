/** \ingroup CORE */
/*@{*/

/**	
*	@file	
*	File: ByteCodeLine.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Forniés (EMULATORS library) \n
*	Creation Date: 27/06/2021 \n
*	Description: To manage a line of code.
*	Versions: 1.0 Initial
*/

#ifndef __CORE_BYTECODELINE__
#define __CORE_BYTECODELINE__

#include <CORE/incs.hpp>

namespace MCHEmul
{
	/** The instruction can be grotup in a line of bytes with info around. */
	struct ByteCodeLine final
	{
		ByteCodeLine ()
			: _address ({ 0x00 }), _bytes (), _label (""), _instruction (nullptr), _actionOn (0)
						{ }

		ByteCodeLine (const Address& a, const std::vector <UByte>& b, const std::string& n, 
				const Instruction* i, unsigned int act)
			: _address (a), _bytes (b), _label (n), _instruction (i), _actionOn (act)
						{ }

		ByteCodeLine (const ByteCodeLine&) = default;

		ByteCodeLine& operator = (const ByteCodeLine&) = default;

		std::string asString (UByte::OutputFormat oF, 
			char s /** separator */, size_t l = 0 /** Minimum length per UByte */) const;

		friend std::ostream& operator << (std::ostream& o, const ByteCodeLine& c)
						{ return (o << c.asString (UByte::OutputFormat::_HEXA, ' ', 2)); }

		Address _address;
		std::vector <UByte> _bytes;
		std::string _label;
		const Instruction* _instruction;
		unsigned int _actionOn;
	};
}

#endif

// End of the file
/*@}*/