/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: DtMemoryBlock.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 01/11/2022 \n
 *	Description: Define the typical structure for a block memory.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_DTMEMORYBLOCK__
#define __MCHEMUL_DTMEMORYBLOCK__

#include <CORE/UBytes.hpp>
#include <CORE/Address.hpp>

namespace MCHEmul
{
	/** The structure is very useful to manage blocks of memory content. 
		The block of memory can represent many things, usually code or data associated. */
	struct DataMemoryBlock final
	{
		DataMemoryBlock () = default;

		DataMemoryBlock (const Address& a, const std::vector <UByte>& dt)
			: _startAddress (a), _bytes (dt)
							{ }

		DataMemoryBlock (const DataMemoryBlock&) = default;

		DataMemoryBlock& operator = (const DataMemoryBlock&) = default;

		Address _startAddress;
		std::vector <MCHEmul::UByte> _bytes;
	};

	// To simplify the management of a set of them...
	using DataMemoryBlocks = std::vector <DataMemoryBlock>;
}

#endif
  
// End of the file
/*@}*/
