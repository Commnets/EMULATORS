/** \ingroup FZ80 */
/*@{*/

/**
 * @file
 * File: MCycle.hpp \n
 * Framework: CPU Emulators library \n
 * Author: Ignacio Cea Fornies (EMULATORS library) \n
 * Creation Date: 16/10/2026 \n
 * Description: Z80 machine cycles and precalculated timing data. \n
 * Versions: 1.0 Initial.
 */

#ifndef __FZX80_MCYCLE__
#define __FZX80_MCYCLE__

#include <CORE/CPUTransaction.hpp>
#include <initializer_list>

namespace FZ80
{
	/** One nominal Z80 machine cycle. Local T-state indexes start at zero. \n
		Inserted waits and runtime addresses are not part of this description. */
	struct MCycle final
	{
		public:
		/** No corresponding timing point is declared in this descriptor. */
		static const size_t _NOCYCLE = 
			(std::numeric_limits <size_t>::max)();

		enum class Type
		{
			_OPCODEFETCH,
			_MEMORYREAD,
			_MEMORYWRITE,
			_IOREAD,
			_IOWRITE,
			_INTERNAL,
			_INTERRUPTACKNOWLEDGE
		};

		/** cS describes the nominal T-states. \n
			dC identifies the data sampling or write point. \n
			cC identifies the reference point supplied to contention logic. \n
			rC identifies the beginning of refresh when applicable. \n
			_NOCYCLE means that no corresponding timing point is declared. */
		MCycle (Type t, std::initializer_list <unsigned int> cS,
			size_t dC, size_t cC, size_t rC = _NOCYCLE);

		Type type () const
								{ return (_type); }
		const MCHEmul::CycleStructure& tCycles () const
								{ return (_tCycles); }
		size_t numberTCycles () const
								{ return (_tCycles.size ()); }
		size_t dataCycle () const
								{ return (_dataCycle); }
		size_t contentionCycle () const
								{ return (_contentionCycle); }
		size_t refreshCycle () const
								{ return (_refreshCycle); }

		private:
		Type _type;
		MCHEmul::CycleStructure _tCycles;
		/** Local timing points; no absolute CPU clock is stored here. */
		size_t _dataCycle, _contentionCycle, _refreshCycle;
	};

	/** Nominal T-state characteristics describe framework operations, not pin levels. \n
		M1 samples the opcode at T3, when its refresh portion also begins. */
	#define _Z80_M1() FZ80::MCycle ( \
		FZ80::MCycle::Type::_OPCODEFETCH, \
		{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ, \
		  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_INTERNAL }, 2, 0, 2)

	/** Ordinary memory transfers occupy three nominal T-states. */
	#define _Z80_MR3() FZ80::MCycle ( \
		FZ80::MCycle::Type::_MEMORYREAD, \
		{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ, \
		  MCHEmul::CPUCycle::_READ }, 2, 0)

	#define _Z80_MW3() FZ80::MCycle ( \
		FZ80::MCycle::Type::_MEMORYWRITE, \
		{ MCHEmul::CPUCycle::_WRITE, MCHEmul::CPUCycle::_WRITE, \
		  MCHEmul::CPUCycle::_WRITE }, 2, 0)

	/** The mandatory I/O wait is included in the four nominal T-states. \n
		The contention reference is the cycle start; machine wiring resolves subphases. */
	#define _Z80_IOR4() FZ80::MCycle ( \
		FZ80::MCycle::Type::_IOREAD, \
		{ MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_READ, \
		  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ }, 3, 0)

	#define _Z80_IOW4() FZ80::MCycle ( \
		FZ80::MCycle::Type::_IOWRITE, \
		{ MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_WRITE, \
		  MCHEmul::CPUCycle::_WRITE, MCHEmul::CPUCycle::_WRITE }, 3, 0)

	/** Maskable-interrupt entry before its ordinary memory transfers. \n
		Generic T-state characteristics are unspecified for this special bus phase. \n
		No ordinary memory transfer or contention point is declared here. */
	#define _Z80_INTACK7() FZ80::MCycle ( \
		FZ80::MCycle::Type::_INTERRUPTACKNOWLEDGE, \
		{ MCHEmul::CPUCycle::_NOTDEFINED, MCHEmul::CPUCycle::_NOTDEFINED, \
		  MCHEmul::CPUCycle::_NOTDEFINED, MCHEmul::CPUCycle::_NOTDEFINED, \
		  MCHEmul::CPUCycle::_NOTDEFINED, MCHEmul::CPUCycle::_NOTDEFINED, \
		  MCHEmul::CPUCycle::_NOTDEFINED }, \
		FZ80::MCycle::_NOCYCLE, FZ80::MCycle::_NOCYCLE)

	/** Nonmaskable-interrupt entry before its two stack writes. \n
		Does not classify the ignored fetch as an ordinary operand read. \n
		Recognition and refresh contention remain outside this descriptor's scope. */
	#define _Z80_NMIACK5() FZ80::MCycle ( \
		FZ80::MCycle::Type::_INTERRUPTACKNOWLEDGE, \
		{ MCHEmul::CPUCycle::_NOTDEFINED, MCHEmul::CPUCycle::_NOTDEFINED, \
		  MCHEmul::CPUCycle::_NOTDEFINED, MCHEmul::CPUCycle::_NOTDEFINED, \
		  MCHEmul::CPUCycle::_NOTDEFINED }, \
		FZ80::MCycle::_NOCYCLE, FZ80::MCycle::_NOCYCLE)

	/** Extends an ordinary opcode fetch with explicitly declared T-states. \n
		The opcode sampling and refresh points retain their ordinary positions. \n
		At least one additional T-state must be supplied. */
	#define _Z80_M1_EXT(...) FZ80::MCycle ( \
		FZ80::MCycle::Type::_OPCODEFETCH, \
		{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ, \
		  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_INTERNAL, \
		  __VA_ARGS__ }, 2, 0, 2)

	/** Extends an ordinary memory read with explicitly declared T-states. \n
		The byte is sampled before the additional T-states. \n
		At least one additional T-state must be supplied. */
	#define _Z80_MR_EXT(...) FZ80::MCycle ( \
		FZ80::MCycle::Type::_MEMORYREAD, \
		{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ, \
		  MCHEmul::CPUCycle::_READ, __VA_ARGS__ }, 2, 0)

	/** Extends an ordinary memory write with explicitly declared T-states. \n
		The byte transfer retains its ordinary position before the extension. \n
		At least one additional T-state must be supplied. */
	#define _Z80_MW_EXT(...) FZ80::MCycle ( \
		FZ80::MCycle::Type::_MEMORYWRITE, \
		{ MCHEmul::CPUCycle::_WRITE, MCHEmul::CPUCycle::_WRITE, \
		  MCHEmul::CPUCycle::_WRITE, __VA_ARGS__ }, 2, 0)

	/** Extended fetches remain one M-cycle and perform one opcode read. */
	#define _Z80_M1_5() _Z80_M1_EXT ( \
		MCHEmul::CPUCycle::_INTERNAL)

	#define _Z80_M1_6() _Z80_M1_EXT ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL)

	/** Extended reads remain one M-cycle and transfer one byte. */
	#define _Z80_MR4() _Z80_MR_EXT ( \
		MCHEmul::CPUCycle::_INTERNAL)

	#define _Z80_MR5() _Z80_MR_EXT ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL)

	/** Extended writes retain one byte transfer followed by two internal T-states. */
	#define _Z80_MW5() _Z80_MW_EXT ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL)

	/** Describes an independent internal span without a byte transfer. \n
		Every supplied T-state must have the _INTERNAL characteristic. \n
		The contention reference is the span start; it does not determine waits. */
	#define _Z80_INTERNAL(...) FZ80::MCycle ( \
		FZ80::MCycle::Type::_INTERNAL, \
		{ __VA_ARGS__ }, FZ80::MCycle::_NOCYCLE, 0)

	#define _Z80_I1() _Z80_INTERNAL ( \
		MCHEmul::CPUCycle::_INTERNAL)

	#define _Z80_I2() _Z80_INTERNAL ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL)

	#define _Z80_I3() _Z80_INTERNAL ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL, \
		MCHEmul::CPUCycle::_INTERNAL)

	#define _Z80_I4() _Z80_INTERNAL ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL, \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL)

	#define _Z80_I5() _Z80_INTERNAL ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL, \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL, \
		MCHEmul::CPUCycle::_INTERNAL)

	#define _Z80_I7() _Z80_INTERNAL ( \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL, \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL, \
		MCHEmul::CPUCycle::_INTERNAL, MCHEmul::CPUCycle::_INTERNAL, \
		MCHEmul::CPUCycle::_INTERNAL)

	/** One complete nominal machine-cycle sequence. */
	using MCyclesStructure = std::vector <MCycle>;

	/** Alternative complete sequences; element zero is the nominal one. */
	using MCyclesStructures = std::vector <MCyclesStructure>;

	/** Precalculated positions relative to the transaction start, without waits. \n
		The end position is exclusive. Optional absent points remain _NOCYCLE. */
	struct MCycleData final
	{
		MCycleData (const MCycle& mC, size_t fC);

		MCycle::Type _type;
		size_t _firstCycle, _endCycle;
		size_t _dataCycle, _contentionCycle, _refreshCycle;
	};

	/** Direct mapping from a nominal T-state to its M-cycle and local position. */
	struct MCyclePosition final
	{
		size_t _mCycle;
		size_t _localCycle;
	};

	/** Timing indexes for one complete sequence, exposed through const references. \n
		Construction performs all scans and allocations. Runtime queries are indexed. \n
		No reference to the source sequence is retained. */
	class MCyclesData final
	{
		public:
		/** An empty descriptor supports transitional declarations. */
		MCyclesData ();
		explicit MCyclesData (const MCyclesStructure& mCS);

		size_t numberTCycles () const
								{ return (_positions.size ()); }
		size_t numberMCycles () const
								{ return (_cycles.size ()); }

		/** tC must identify an existing nominal T-state, excluding inserted waits. */
		const MCyclePosition& positionAt (size_t tC) const
								{ return (_positions [tC]); }
		/** mC must identify an existing M-cycle. */
		const MCycleData& cycleData (size_t mC) const
								{ return (_cycles [mC]); }

		/** Lists contain M-cycle indexes in execution order, not T-state indexes. \n
			Each transfer appears once regardless of its nominal duration. */
		const std::vector <size_t>& memoryReads () const
								{ return (_memoryReads); }
		const std::vector <size_t>& memoryWrites () const
								{ return (_memoryWrites); }
		const std::vector <size_t>& ioCycles () const
								{ return (_ioCycles); }
		const std::vector <size_t>& internalCycles () const
								{ return (_internalCycles); }
		const std::vector <size_t>& interruptAcknowledges () const
								{ return (_interruptAcknowledges); }

		private:
		std::vector <MCycleData> _cycles;
		std::vector <MCyclePosition> _positions;
		/** Opcode fetches are memory reads; refresh is not another byte transfer. */
		std::vector <size_t> _memoryReads, _memoryWrites;
		std::vector <size_t> _ioCycles, _internalCycles;
		std::vector <size_t> _interruptAcknowledges;
	};

	/** One precalculated descriptor per alternative sequence. */
	using MCyclesDatas = std::vector <MCyclesData>;
}

#endif

// End of the file
/*@}*/
