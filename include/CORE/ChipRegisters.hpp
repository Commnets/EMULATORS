/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: ChipRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: All Registers emulated inherit from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __CORE_CHIPREGISTERS__
#define __CORE_CHIPREGISTERS__

#include <CORE/Memory.hpp>

namespace MCHEmul
{
	/** Just to print basically any register. */
	class ChipRegisters : public PhysicalStorageSubset
	{
		public:
		ChipRegisters (int id, PhysicalStorage* ps, size_t pp, const Address& a, size_t s)
			: PhysicalStorageSubset (id, ps, pp, a, s)
							{ }

		/** To be redefined later to return the specific number of registers of the chip. */
		virtual size_t numberRegisters () const = 0;
		/** To get the value of the registers. */
		virtual std::vector <MCHEmul::UByte> valueRegisters () const;

		/**
		  *	The name of the fields are: \n
		  * BYTES = Attribute: Bytes controlled by this register.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** Like "readValue" but not affecting the value in anycase. \n
			By default it behaves like "readValue" (@see readValue). */
		virtual const UByte& peekValue (size_t nB) const
							{ return (readValue (nB)); }
	};
}

#endif
  
// End of the file
/*@}*/
