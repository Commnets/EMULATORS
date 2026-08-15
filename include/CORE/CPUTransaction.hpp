/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CPUTransaction.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/08/2026 \n
 *	Description: Defines the bus-cycle information shared by CPU transactions. \n
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPUTRANSACTION__
#define __MCHEMUL_CPUTRANSACTION__

#include <CORE/global.hpp>

namespace MCHEmul
{
	/** Generic types of cycles that can make up a CPU transaction. \n
		The identifiers are independent bits so that a cycle can eventually
		combine more than one characteristic. */
	struct CPUCycle final
	{
		/** No bus-cycle behaviour has been defined. */
		static const unsigned int _NOTDEFINED = 0;
		/** The cycle is internal to the CPU. */
		static const unsigned int _INTERNAL = 1;
		/** The cycle reads information from the bus. */
		static const unsigned int _READ = 2;
		/** The cycle writes information to the bus. */
		static const unsigned int _WRITE = 4;
		/** Every possible type of cycle. */
		static const unsigned int _ALL = std::numeric_limits <unsigned int>::max ();
	};

	/** Sequence describing the cycles of a CPU transaction. */
	using CycleStructure = std::vector <unsigned int>;
	/** Alternative cycle structures supported by a CPU transaction. \n
		Element zero always describes its nominal execution. */
	using CycleStructures = std::vector <CycleStructure>;

	/** Information precalculated from one cycle structure of a CPU transaction. \n
		The structure is calculated once when the transaction is constructed,
		avoiding repeated scans in timing-sensitive execution paths. \n
		Cycle indexes are zero-based. _NOCYCLE indicates that no matching cycle exists. \n
		Every alternative structure has its own BusCycleData. */
	struct BusCycleData final
	{
		static const size_t _NOCYCLE = std::numeric_limits <size_t>::max ();

		BusCycleData ();
		BusCycleData (const CycleStructure& cS);

		/** Returns a cycle structure whose size matches nC. \n
			When cS is empty, every cycle is marked as _NOTDEFINED. \n
			A non-empty structure is returned unchanged and must already have the expected size. */
		static CycleStructure normalizedCycleStructure
			(unsigned int nC, const CycleStructure& cS);
		/** Returns a normalized collection of cycle structures. \n
			When cSs is empty, one nominal structure containing nC _NOTDEFINED cycles is returned. \n
			Otherwise, structure zero must contain exactly nC cycles and no structure can be empty. */
		static CycleStructures normalizedCycleStructures
			(unsigned int nC, const CycleStructures& cSs);

		/** Returns the first read cycle whose index is greater than or equal to c. \n
			_NOCYCLE is returned when no such read exists. */
		size_t nextReadCycle (size_t c) const
							{ assert (c <= _numberCycles); return (_nextReadCycles [c]); }

		size_t _numberCycles;
		size_t _numberReadCycles;
		size_t _numberWriteCycles;

		size_t _firstReadCycle;
		size_t _lastReadCycle;
		size_t _firstWriteCycle;
		size_t _lastWriteCycle;

		size_t _trailingWriteCycles;
		size_t _maximumConsecutiveWriteCycles;

		unsigned int _lastCycleType;

		/** Entry n contains the first read cycle whose index is greater than or equal to n. \n
			The final entry, numberCycles, always contains _NOCYCLE. */
		std::vector <size_t> _nextReadCycles;
	};

	/** Precalculated bus information for every alternative cycle structure. \n
		Element n always describes element n of the corresponding CycleStructures. */
	using BusCycleDatas = std::vector <BusCycleData>;
}

#endif
  
// End of the file
/*@}*/
