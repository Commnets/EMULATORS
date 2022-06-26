/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: CPUArchitecture.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Defines common elemnts to any CPU Architcture.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPUARCHITECTURE__
#define __MCHEMUL_CPUARCHITECTURE__

#include <CORE/global.hpp>
#include <CORE/Register.hpp>

namespace MCHEmul
{
	/** The architecture of CPU is related usually with the maximum size of its registers in number of bytes. */
	class CPUArchitecture final
	{
		public:
		CPUArchitecture () = delete;

		CPUArchitecture (size_t nb /** Bytes */, size_t iL, bool bE = true, const Attributes& attrs = { });

		CPUArchitecture (const CPUArchitecture&) = default;

		CPUArchitecture& operator = (const CPUArchitecture&) = default;

		size_t numberBytes () const
							{ return (_numberBytes); }
		size_t numberBits () const
							{ return (_numberBits); }
		size_t instructionLength () const
							{ return (_instructionLength); }
		bool bigEndian () const
							{ return (_bigEndian); }

		/** To get a register of a specific length only if is possible. */
		Register registerLength (int id, const std::string& n, size_t nb /** Bytes. */) const
							{ return ((nb > 0 && nb <= _numberBytes) 
								? Register (id, n, UBytes (std::vector <UByte> (nb, UByte::_0))) : NoRegister); }
		/** To get a exampla of the longest possible register. It is constant so it would have to be copied once it is got. */ 
		const Register& longestRegisterPossible () const
							{ return (_longestRegisterPossible); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		friend std::ostream& operator << (std::ostream& o, const CPUArchitecture& a);

		private:
		const size_t _numberBytes = 0; // Adjusted at construction level
		const size_t _numberBits = 0; // Calculated at construction level
		const size_t _instructionLength = 0; // Adjusted at construction level
		const bool _bigEndian = true; // Are the most significant Ubyte (in an address) the first or the last in values?
		const Attributes _attributes = { }; // Optionaly adjusted at construction level
		const Register _longestRegisterPossible = NoRegister; // Calculated at construction level
	};
}

#endif
  
// End of the file
/*@}*/
