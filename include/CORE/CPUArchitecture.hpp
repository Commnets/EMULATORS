/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CPUArchitecture.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Defines common elemnts to any CPU Architcture.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPUARCHITECTURE__
#define __MCHEMUL_CPUARCHITECTURE__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/Register.hpp>

namespace MCHEmul
{
	/** The architecture of CPU is related usually with the maximum size of its registers in number of bytes. */
	class CPUArchitecture final : public InfoClass
	{
		public:
		CPUArchitecture (size_t nb /** Bytes */, size_t iL, bool bE = true, const Attributes& attrs = { });

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
		/** To get a example of the longest possible register. It is constant so it would have to be copied once it is got. */ 
		const Register& longestRegisterPossible () const
							{ return (_longestRegisterPossible); }
		/** To get the maxmim address possible. */
		const Address& longestAddressPossible () const
							{ return (_longestAddressPossible); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		/**
		  *	The name of the fields are: \n
		  *	ATTRS			= InfoStructure: Attributes. \n
		  *	NBYTES			= Attribute: The number of bytes of the architecture. \n
		  *	NBITS			= Attribute: The number of bits of the architecture. \n
		  *	NBYTESPERINST	= Attribute: The number of bytes per instruction. \n
		  *	ENDIAN			= Attribute: YES if the architecture is defined as big endian or NO in otehr case.
		  */
		virtual InfoStructure getInfoStructure () const override;

		private:
		const size_t _numberBytes;
		const size_t _numberBits;
		const size_t _instructionLength;
		const bool _bigEndian;
		const Attributes _attributes;
		const Register _longestRegisterPossible;
		const Address _longestAddressPossible;
	};
}

#endif
  
// End of the file
/*@}*/
