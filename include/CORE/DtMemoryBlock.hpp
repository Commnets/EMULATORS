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
	class DataMemoryBlock;
	using DataMemoryBlocks = std::vector <DataMemoryBlock>;
	class DataMemoryBlock final
	{
		public:
		DataMemoryBlock () = default;

		DataMemoryBlock (const Address& a, const std::vector <UByte>& dt)
			: _name (""), _startAddress (a), _bytes (dt)
							{ }

		DataMemoryBlock (const Address& a, std::vector <UByte>&& dt) noexcept
			: _name (""), _startAddress (a), _bytes (std::move (dt))
							{ }

		// Get & Set methods
		/** The name. */
		const std::string& name () const
							{ return (_name); }
		void setName (const std::string& n)
							{ _name = n; }
		/** The address where the info should be start from. */
		const Address& startAddress () const
							{ return (_startAddress); }
		const std::vector <UByte>& bytes () const
							{ return (_bytes); }

		/** Save the memory block.
			It receives only the size of the address at the header and whether that address
			has be kept into big or little endian. */
		bool save (const std::string& fN, size_t nB, bool bE = true) const;

		// Loading info...
		/** To load a binary file. \n
			The binary file has inside the address where to start to put the data into at the very first bytes loaded. \n
			If there were errors the variable "e" will be true. \n
			The method needs also the length of that address in the header and 
			whether it is store in big endian or little endian format. */
		static DataMemoryBlock loadBinaryFile (const std::string& fN, bool& e, size_t nB, bool bE = true);
		/** To load a file made up of block of data. \n
			This type of file is made up of several data memory blocks. \n
			If there were errors the variable "e" will be true.
			The method needs also the length of that address in the header and 
			whether it is store in big endian or little endian format. */
		static DataMemoryBlocks loadBlocksFile (const std::string& fN, bool& e, size_t nB, bool bE = true);

		// Saving blocks info...
		/** The structure of the file will repeat per block and will include after the address 
			4 bytes indicating the number of data bytes that follows. */
		static bool saveDataBlocks (const std::string& fN, const DataMemoryBlocks& blks, size_t nB, bool bE = true);

		private:
		std::string _name; // The name is very optional...
		Address _startAddress;
		std::vector <MCHEmul::UByte> _bytes;
	};
}

#endif
  
// End of the file
/*@}*/
