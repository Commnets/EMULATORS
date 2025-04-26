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

#include <CORE/CPUArchitecture.hpp>
#include <CORE/Memory.hpp>

namespace MCHEmul
{
	/** Just to print basically any register. */
	class ChipRegisters : public PhysicalStorageSubset
	{
		public:
		/** Constructor used when the registers are not part of the memory,
			but just something accesible thought out the methods readRegiter, setRegister or peekRegister. \b
			A temporal inernal Physycal storage is created. \b
			It will be deleted when this object was deleted too. \
			The number of bytes to use for the address of the physical storage created behind is needed. \n
			The number of registers of the chip is also needed as parameter, because at construction time, 
			the method numberRegisters is not still valid and accesible. */
		ChipRegisters (int id, size_t nB, size_t nR);
		/** This constructor is used when the registers are accesible from a memory position. */
		ChipRegisters (int id, PhysicalStorage* ps, size_t pp, const Address& a, size_t s)
			: PhysicalStorageSubset (id, ps, pp, a, s),
			  _internalStorage (nullptr)
							{ }

		virtual ~ChipRegisters ();

		// To access the registers from outside...
		// Take care because using these methods no boundary check at all is done!
		// They can not be overloaded. The methods to overload if another 
		// behaviour were wanted are the ones invoked from these ones
		const UByte& readRegister (size_t nR) const
							{ return (readValue (nR)); }
		void setRegister (size_t nR, const UByte& v)
							{ setValue (nR, v); }
		const UByte& peekRegister (size_t nR) const
							{ return (peekValue (nR)); }

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

		protected:
		PhysicalStorage* _internalStorage;
	};
}

#endif
  
// End of the file
/*@}*/
