#include <CORE/DtMemoryBlock.hpp>

const std::string MCHEmul::DataMemoryBlock::_NOATTRIBUTE = "";

// ---
bool MCHEmul::DataMemoryBlock::save (const std::string& fN, size_t nB, bool bE) const
{
	MCHEmul::UBytes sAB = MCHEmul::UBytes (_startAddress.bytes (), bE); 
	sAB.setMinLength (nB);
	std::vector <MCHEmul::UByte> sABb = sAB.bytes ();

	std::vector <MCHEmul::UByte> dt = _bytes;
	dt.insert (dt.begin (), sABb.begin (), sABb.end ());

	return (MCHEmul::UBytes::saveBytesTo (fN, dt));
}

// ---
MCHEmul::DataMemoryBlock MCHEmul::DataMemoryBlock::loadBinaryFile (const std::string& fN, bool& e, size_t nB, bool bE)
{
	e = false;

	std::vector <MCHEmul::UByte> by = UBytes::loadBytesFrom (fN, e);
	if (e)
		return (MCHEmul::DataMemoryBlock ());

	// The length of the data file has to be at least the needed to hold the starting address
	// That is defined by the number of bytes of the cpu behind...
	if (by.size () < nB)
	{
		e = true;

		return (MCHEmul::DataMemoryBlock ());
	}

	// The first bytes (nB) of the data represents the address where to load the rest...
	// That address has to be represented in the format of the computer behind (little or big endian)
	MCHEmul::DataMemoryBlock result 
		(MCHEmul::Address (std::vector <MCHEmul::UByte> (by.begin (), by.begin () + nB), bE),
			std::vector <MCHEmul::UByte> (by.begin () + nB, by.end ()));

	return (result);
}

// ---
MCHEmul::DataMemoryBlocks MCHEmul::DataMemoryBlock::loadBlocksFile (const std::string& fN, bool& e, size_t nB, bool bE)
{
	e = false;

	std::vector <MCHEmul::UByte> by = UBytes::loadBytesFrom (fN, e);
	if (e)
		return (MCHEmul::DataMemoryBlocks ());

	// The length of the data file has to be at least the needed to hold the starting address
	// That is defined by the number of bytes of the cpu behind...
	if (by.size () < (nB + 4 /** 4 bytes for keeping the size of one block. */))
	{
		e = true;

		return (MCHEmul::DataMemoryBlocks ());
	}

	MCHEmul::DataMemoryBlocks result;
	std::vector <MCHEmul::UByte>::const_iterator i = by.begin ();
	while (i != by.end ())
	{
		// Reads a block...
		MCHEmul::Address sA (std::vector <MCHEmul::UByte> (i, i + nB), bE);
		MCHEmul::UInt bS (std::vector <MCHEmul::UByte> (i + nB, i + nB + 4));
		MCHEmul::DataMemoryBlock pR
		(sA, std::vector <MCHEmul::UByte> (i + nB + 4, i + nB + 4 + (size_t) bS.asUnsignedInt ()));

		// Adds to the total...
		result.emplace_back (pR);

		// Next...
		i += nB + 4 + (size_t) bS.asUnsignedInt ();
	}

	return (result);
}

// ---
bool MCHEmul::DataMemoryBlock::saveDataBlocks 
	(const std::string& fN, const MCHEmul::DataMemoryBlocks& blks, size_t nB, bool bE)
{
	std::vector <MCHEmul::UByte> dt;
	for (const auto& i : blks)
	{
		std::vector <MCHEmul::UByte> pdt = i._bytes;

		// Insert the length to include
		MCHEmul::UInt l = MCHEmul::UInt ((unsigned int) pdt.size ()); 
		l.setMinLength (4); // ...with a minimum length of 4 bytes (is the maximum size admitted by the emulation)
		pdt.insert (pdt.begin (), l.bytes ().begin (), l.bytes ().end ());

		// Then the address...(@see above), that becomes first...
		// In the format understood by the cpu (little or big endian)
		// ...and with number of bytes that are also managed by the cpu behind!
		MCHEmul::UBytes sAB = MCHEmul::UBytes (i._startAddress.bytes (), bE);
		sAB.setMinLength (nB);
		std::vector <MCHEmul::UByte> sABb = sAB.bytes (); 
		pdt.insert (pdt.begin (), sABb.begin (), sABb.end ());

		// At this point the address is first and then the length of the data kept.

		// Adds the block to the total list to be saved later...
		dt.insert (dt.end (), pdt.begin (), pdt.end ());
	}

	return (MCHEmul::UBytes::saveBytesTo (fN, dt));
}
