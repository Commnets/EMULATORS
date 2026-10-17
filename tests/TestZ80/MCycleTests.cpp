#include "MCycleTests.hpp"
#include <FZ80/MCycle.hpp>
#include <FZ80/LD.hpp>
#include <FZ80/INC.hpp>
#include <FZ80/DEC.hpp>
#include <FZ80/ADD.hpp>
#include <FZ80/SUB.hpp>
#include <FZ80/AND.hpp>
#include <FZ80/OR.hpp>
#include <FZ80/XOR.hpp>
#include <FZ80/CP.hpp>
#include <FZ80/ALUCTRL.hpp>
#include <FZ80/ROTATELEFT.hpp>
#include <FZ80/ROTATERIGHT.hpp>
#include <FZ80/SHIFTLEFT.hpp>
#include <FZ80/SHIFTRIGHT.hpp>
#include <FZ80/BIT.hpp>
#include <FZ80/SET.hpp>
#include <FZ80/RES.hpp>
#include <FZ80/STACK.hpp>
#include <FZ80/JUMP.hpp>
#include <FZ80/CALL.hpp>
#include <FZ80/EX.hpp>
#include <FZ80/INOUT.hpp>

namespace FZ80
{
	/** Exercises the family forwarding constructors without executing an opcode. */
	class MCycleTestInstruction final : public LD_General
	{
		public:
		using LD_General::LD_General;

		protected:
		virtual bool executeImpl (bool& finish) override
							{ finish = true; return (true); }
	};

	// Test-only instructions exercise nested macro expansion and commas inside braces.
	_INSTZ80_MC_FROM (0x21, 3, 10, "LD HL,[$2]", MCycleMacroSingleTest, LD_General,
		_Z80_M1_5 (), _Z80_MR4 (), _Z80_I1 ());

	_INSTZ80_MCS_FROM (0, 1, 4, "NOP", MCycleMacroAlternativesTest, LD_General,
		FZ80::MCyclesStructure { _Z80_M1 () },
		FZ80::MCyclesStructure {
			_Z80_M1_5 (), _Z80_I2 () });
}

// ---
bool FZ80::MCycleMacroSingleTest::executeImpl (bool& finish)
{
	finish = true;
	return (true);
}

// ---
bool FZ80::MCycleMacroAlternativesTest::executeImpl (bool& finish)
{
	finish = true;
	return (true);
}

// ---
bool FZ80::MCycleTests::run (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const unsigned int r = MCHEmul::CPUCycle::_READ;
	const unsigned int w = MCHEmul::CPUCycle::_WRITE;
	const unsigned int i = MCHEmul::CPUCycle::_INTERNAL;

	const MCyclesData empty;
	const MCyclesData emptySequence (MCyclesStructure { });
	ok &= check (empty.numberTCycles () == 0 && empty.numberMCycles () == 0 &&
		empty.memoryReads ().empty () && empty.memoryWrites ().empty () &&
		empty.ioCycles ().empty () && empty.internalCycles ().empty () &&
		empty.interruptAcknowledges ().empty () && emptySequence.numberTCycles () == 0 &&
		emptySequence.numberMCycles () == 0, "empty descriptors", o);

	// Deliberately synthetic timing points test translation independently of
	// the hardware defaults which will be supplied by later instruction macros.
	MCyclesStructure sequence {
		MCycle (MCycle::Type::_OPCODEFETCH, { r, r, r, i }, 2, 0, 3),
		MCycle (MCycle::Type::_MEMORYREAD, { r, r, r }, 2, 0),
		MCycle (MCycle::Type::_MEMORYREAD, { r, r, r }, 2, 1) };
	const MCyclesData timing (sequence);
	ok &= check (timing.numberTCycles () == 10 && timing.numberMCycles () == 3 &&
		timing.memoryReads () == std::vector <size_t> ({ 0, 1, 2 }),
		"three logical reads in ten T-states", o);

	// Explicit boundary expectations catch a T-state assigned to its neighbour.
	const size_t machineIndexes [] = { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2 };
	const size_t localIndexes [] = { 0, 1, 2, 3, 0, 1, 2, 0, 1, 2 };
	bool positionsOK = true;
	for (size_t t = 0; t < 10; t++)
		positionsOK &= timing.positionAt (t)._mCycle == machineIndexes [t] &&
			timing.positionAt (t)._localCycle == localIndexes [t];
	ok &= check (positionsOK, "all T-state positions and boundaries", o);
	ok &= check (timing.cycleData (0)._firstCycle == 0 &&
		timing.cycleData (0)._endCycle == 4 && timing.cycleData (0)._dataCycle == 2 &&
		timing.cycleData (0)._refreshCycle == 3 &&
		timing.cycleData (1)._firstCycle == 4 && timing.cycleData (1)._endCycle == 7 &&
		timing.cycleData (1)._dataCycle == 6 && timing.cycleData (1)._contentionCycle == 4 &&
		timing.cycleData (1)._refreshCycle == absent &&
		timing.cycleData (2)._firstCycle == 7 && timing.cycleData (2)._endCycle == 10 &&
		timing.cycleData (2)._dataCycle == 9 && timing.cycleData (2)._contentionCycle == 8,
		"translated timing points and exclusive ends", o);

	// Metadata must remain usable after its source container is destroyed.
	sequence.clear ();
	const MCyclesData copied (timing);
	ok &= check (timing.cycleData (2)._dataCycle == 9 &&
		copied.positionAt (9)._mCycle == 2 && copied.memoryReads ().size () == 3,
		"source lifetime and value-copy independence", o);

	const MCyclesData mixed (MCyclesStructure {
		MCycle (MCycle::Type::_MEMORYWRITE, { w, w, w }, 2, 0),
		MCycle (MCycle::Type::_IOREAD, { i, r, r, r }, 3, 0),
		MCycle (MCycle::Type::_IOWRITE, { i, w, w, w }, 3, 0),
		MCycle (MCycle::Type::_INTERNAL, { i }, absent, absent),
		MCycle (MCycle::Type::_INTERRUPTACKNOWLEDGE, { i, r, i }, 1, absent, 2) });
	ok &= check (mixed.numberTCycles () == 15 && mixed.numberMCycles () == 5 &&
		mixed.memoryReads ().empty () &&
		mixed.memoryWrites () == std::vector <size_t> ({ 0 }) &&
		mixed.ioCycles () == std::vector <size_t> ({ 1, 2 }) &&
		mixed.internalCycles () == std::vector <size_t> ({ 3 }) &&
		mixed.interruptAcknowledges () == std::vector <size_t> ({ 4 }),
		"memory, I/O, internal and acknowledge classification", o);
	ok &= check (mixed.cycleData (3)._firstCycle == 11 &&
		mixed.cycleData (3)._dataCycle == absent &&
		mixed.cycleData (3)._contentionCycle == absent &&
		mixed.cycleData (3)._refreshCycle == absent &&
		mixed.cycleData (4)._dataCycle == 13 &&
		mixed.cycleData (4)._contentionCycle == absent &&
		mixed.cycleData (4)._refreshCycle == 14 &&
		mixed.cycleData (4)._type == MCycle::Type::_INTERRUPTACKNOWLEDGE,
		"absent points survive nonzero offsets", o);

	const MCyclesStructures alternatives {
		MCyclesStructure { MCycle (MCycle::Type::_INTERNAL, { i }, absent, absent) },
		MCyclesStructure { MCycle (MCycle::Type::_INTERNAL, { i, i }, absent, 1) } };
	const MCyclesDatas alternateData {
		MCyclesData (alternatives [0]), MCyclesData (alternatives [1]) };
	ok &= check (alternateData [0].numberTCycles () == 1 &&
		alternateData [1].numberTCycles () == 2 &&
		alternateData [1].positionAt (1)._localCycle == 1 &&
		alternateData [1].cycleData (0)._contentionCycle == 1,
		"independent alternative descriptions", o);
	ok &= testInstructionMetadata (o);
	ok &= testInstructionMacros (o);
	ok &= testAuxiliaryMacros (o);
	ok &= testPrefixedLD (o);
	ok &= testINCDEC (o);
	ok &= testADDSUB (o);
	ok &= testLogicalCP (o);
	ok &= testALUCTRL (o);
	ok &= testRotations (o);
	ok &= testShifts (o);
	ok &= testBIT (o);
	ok &= testSETRES (o);
	ok &= testSTACK (o);
	ok &= testJUMP (o);
	ok &= testCALL (o);
	ok &= testEX (o);
	ok &= testBlockTransferCompare (o);
	ok &= testSimpleIO (o);
	ok &= testBlockIO (o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testInstructionMetadata (std::ostream& o)
{
	bool ok = true;
	const unsigned int r = MCHEmul::CPUCycle::_READ;
	const unsigned int i = MCHEmul::CPUCycle::_INTERNAL;
	const size_t absent = MCycle::_NOCYCLE;

	// An empty brace argument must keep selecting the compatibility constructor.
	const MCycleTestInstruction legacy (0x21, 3, 10, { }, "LD HL,[$2]");
	const LD_HL existing;
	ok &= check (!legacy.hasMCycles () && legacy.numberMCyclesStructures () == 0 &&
		legacy.clockCycles () == 10 && legacy.cycleStructure () ==
			MCHEmul::CycleStructure (10, MCHEmul::CPUCycle::_NOTDEFINED) &&
		existing.hasMCycles () && existing.clockCycles () == 10 &&
		existing.mCyclesData ().numberMCycles () == 3,
		"legacy construction and migrated instruction compatibility", o);

	MCyclesStructure sequence { _Z80_M1 (), _Z80_MR3 (), _Z80_MR3 () };
	const MCycleTestInstruction described (0x21, 3, 10, "LD HL,[$2]", sequence);
	sequence.clear ();
	const MCHEmul::CycleStructure expected { r, r, r, i, r, r, r, r, r, r };
	ok &= check (described.hasMCycles () && described.numberMCyclesStructures () == 1 &&
		described.clockCycles () == 10 && described.cycleStructure () == expected &&
		existing.cycleStructure () == expected &&
		described.busCycleData ()._numberCycles == 10 &&
		described.busCycleData ()._numberReadCycles == 9 &&
		described.busCycleData ()._lastReadCycle == 9 &&
		described.mCyclesData ().numberTCycles () == 10 &&
		described.mCyclesData ().numberMCycles () == 3 &&
		described.mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2 }),
		"instruction conversion and ownership of constructor input", o);
	ok &= check (&described.mCycleAt (3) == &described.mCyclesStructure () [0] &&
		&described.mCycleAt (4) == &described.mCyclesStructure () [1] &&
		described.mCyclePositionAt (9)._localCycle == 2 &&
		&described.mCycleDataAt (4) == &described.mCyclesData ().cycleData (1) &&
		described.mCycleDataAt (4)._dataCycle == 6 &&
		described.mCycleDataAt (9)._contentionCycle == 7,
		"instruction queries across M-cycle boundaries", o);

	// Alternatives retain separate generic and Z80 metadata; queries select explicitly.
	MCyclesStructures alternatives {
		MCyclesStructure { _Z80_M1 () },
		MCyclesStructure { _Z80_M1 (), _Z80_MR3 () } };
	const MCycleTestInstruction alternate (0, 1, 4, "NOP", alternatives);
	alternatives.clear ();
	ok &= check (alternate.clockCycles () == 4 && alternate.numberMCyclesStructures () == 2 &&
		alternate.cycleStructures ().size () == 2 &&
		alternate.cycleStructure (0).size () == 4 && alternate.cycleStructure (1).size () == 7 &&
		alternate.mCyclesData (0).numberTCycles () == 4 &&
		alternate.mCyclesData (1).numberTCycles () == 7 &&
		alternate.mCycleAt (6, 1).type () == MCycle::Type::_MEMORYREAD &&
		alternate.mCycleDataAt (6, 1)._dataCycle == 6,
		"independent instruction alternatives and nominal duration", o);

	const MCyclesStructure transfers {
		_Z80_M1 (), _Z80_MR3 (), _Z80_MW3 (), _Z80_IOR4 (), _Z80_IOW4 () };
	ok &= check (transfers [0].numberTCycles () == 4 && transfers [0].dataCycle () == 2 &&
		transfers [0].refreshCycle () == 2 && transfers [0].contentionCycle () == 0 &&
		transfers [1].numberTCycles () == 3 && transfers [1].dataCycle () == 2 &&
		transfers [2].numberTCycles () == 3 && transfers [2].type () == MCycle::Type::_MEMORYWRITE &&
		transfers [2].dataCycle () == 2 && transfers [2].refreshCycle () == absent &&
		transfers [3].numberTCycles () == 4 && transfers [3].type () == MCycle::Type::_IOREAD &&
		transfers [3].dataCycle () == 3 && transfers [3].contentionCycle () == 0 &&
		transfers [4].numberTCycles () == 4 && transfers [4].type () == MCycle::Type::_IOWRITE &&
		transfers [4].dataCycle () == 3 && transfers [4].contentionCycle () == 0,
		"standard M-cycle macro timing points", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testInstructionMacros (std::ostream& o)
{
	bool ok = true;
	const MCycleMacroSingleTest single;
	ok &= check (single.code () == 0x21 && single.clockCycles () == 10 &&
		single.numberMCyclesStructures () == 1 &&
		single.mCyclesData ().numberMCycles () == 3 &&
		single.mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
		single.mCycleDataAt (8)._dataCycle == 7 &&
		single.mCycleDataAt (9)._dataCycle == MCycle::_NOCYCLE &&
		single.mCycleDataAt (9)._contentionCycle == 9,
		"single-sequence instruction macro", o);

	const MCycleMacroAlternativesTest alternatives;
	ok &= check (alternatives.clockCycles () == 4 &&
		alternatives.numberMCyclesStructures () == 2 &&
		alternatives.cycleStructure (0).size () == 4 &&
		alternatives.cycleStructure (1).size () == 7 &&
		alternatives.mCyclePositionAt (6, 1)._localCycle == 1 &&
		alternatives.mCycleAt (6, 1).type () == MCycle::Type::_INTERNAL &&
		alternatives.mCycleDataAt (6, 1)._dataCycle == MCycle::_NOCYCLE &&
		alternatives.mCycleDataAt (6, 1)._contentionCycle == 5,
		"alternative instruction macro with nested braced lists", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testAuxiliaryMacros (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const unsigned int r = MCHEmul::CPUCycle::_READ, i = MCHEmul::CPUCycle::_INTERNAL;
	const MCyclesStructure extended { _Z80_M1_5 (), _Z80_M1_6 (), _Z80_MR4 (), _Z80_MR5 () };
	const MCHEmul::CycleStructures expected {
		{ r, r, r, i, i }, { r, r, r, i, i, i }, { r, r, r, i }, { r, r, r, i, i } };
	bool extendedOK = true;
	for (size_t n = 0; n < extended.size (); n++)
		extendedOK &= extended [n].tCycles () == expected [n] &&
			extended [n].dataCycle () == 2 && extended [n].contentionCycle () == 0 &&
			extended [n].type () == ((n < 2) ? MCycle::Type::_OPCODEFETCH : MCycle::Type::_MEMORYREAD) &&
			extended [n].refreshCycle () == ((n < 2) ? 2 : absent);
	ok &= check (extendedOK, "extended macro characteristics and local timing points", o);

	const MCyclesData extendedData (extended);
	ok &= check (extendedData.numberTCycles () == 20 && extendedData.numberMCycles () == 4 &&
		extendedData.memoryReads () == std::vector <size_t> ({ 0, 1, 2, 3 }) &&
		extendedData.internalCycles ().empty () &&
		extendedData.cycleData (1)._dataCycle == 7 && extendedData.cycleData (1)._refreshCycle == 7 &&
		extendedData.cycleData (2)._dataCycle == 13 && extendedData.cycleData (3)._dataCycle == 17,
		"extended transfers are counted once and keep translated sampling points", o);

	const MCyclesStructure internal { _Z80_I1 (), _Z80_I2 (), _Z80_I3 (), _Z80_I4 (), _Z80_I5 (), _Z80_I7 () };
	const size_t lengths [] = { 1, 2, 3, 4, 5, 7 };
	bool internalOK = true;
	for (size_t n = 0; n < internal.size (); n++)
		internalOK &= internal [n].type () == MCycle::Type::_INTERNAL &&
			internal [n].tCycles () == MCHEmul::CycleStructure (lengths [n], i) &&
			internal [n].dataCycle () == absent && internal [n].refreshCycle () == absent &&
			internal [n].contentionCycle () == 0;
	const MCyclesData internalData (internal);
	ok &= check (internalOK && internalData.numberTCycles () == 22 &&
		internalData.internalCycles () == std::vector <size_t> ({ 0, 1, 2, 3, 4, 5 }) &&
		internalData.memoryReads ().empty () && internalData.memoryWrites ().empty () &&
		internalData.ioCycles ().empty () && internalData.interruptAcknowledges ().empty () &&
		internalData.cycleData (5)._contentionCycle == 15 && internalData.cycleData (5)._dataCycle == absent,
		"internal macro lengths, absent transfers and translated references", o);

	// Equal T-state characteristics do not imply equal M-cycle boundaries.
	const MCyclesStructure prolonged { _Z80_M1_5 () };
	const MCyclesStructure separate { _Z80_M1 (), _Z80_I1 () };
	const MCyclesData prolongedData (prolonged);
	const MCyclesData separateData (separate);
	ok &= check (Instruction::toCycleStructure (prolonged) == Instruction::toCycleStructure (separate) &&
		prolongedData.numberMCycles () == 1 && separateData.numberMCycles () == 2 &&
		prolongedData.positionAt (4)._mCycle == 0 && separateData.positionAt (4)._mCycle == 1 &&
		prolongedData.internalCycles ().empty () && separateData.internalCycles ().size () == 1,
		"prolonged M-cycle versus independent internal span", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testPrefixedLD (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;

	// Zilog UM008011, pages 75/77, 81/83 and 86/87: indexed register
	// transfers and immediate stores have equal totals but different fourth M-cycles.
	const LD_AFromAddressIndexIX readIX;
	const LD_AFromAddressIndexIY readIY;
	const LD_IndirectIndexIXFromA writeIX;
	const LD_IndirectIndexIYFromA writeIY;
	const LD_IndirectIndexIX immediateIX;
	const LD_IndirectIndexIY immediateIY;
	const Instruction* indexed [] = {
		&readIX, &readIY, &writeIX, &writeIY, &immediateIX, &immediateIY };
	bool indexedOK = true;
	for (size_t n = 0; n < 6; n++)
	{
		const Instruction& inst = *indexed [n];
		indexedOK &= inst.clockCycles () == 19 && inst.mCyclesData ().numberTCycles () == 19 &&
			inst.mCyclesData ().numberMCycles () == 5 &&
			inst.mCycleDataAt (10)._dataCycle == 10 &&
			inst.mCycleDataAt (11)._firstCycle == 11 && inst.mCycleDataAt (11)._endCycle == 16 &&
			inst.mCycleAt (11).type () == ((n < 4) ? MCycle::Type::_INTERNAL : MCycle::Type::_MEMORYREAD) &&
			inst.mCycleDataAt (11)._dataCycle == ((n < 4) ? absent : 13) &&
			inst.mCycleAt (16).type () == ((n < 2) ? MCycle::Type::_MEMORYREAD : MCycle::Type::_MEMORYWRITE) &&
			inst.mCycleDataAt (18)._dataCycle == 18 &&
			inst.mCyclesData ().memoryWrites ().size () == ((n < 2) ? 0 : 1);
	}
	ok &= check (indexedOK, "IX/IY indexed data points and internal versus immediate M-cycle", o);

	// Pages 94-97: the second opcode fetch lasts five T-states, not a third M-cycle.
	const LD_AFromI aFromI;
	const LD_AFromR aFromR;
	const LD_IFromA iFromA;
	const LD_RFromA rFromA;
	const Instruction* special [] = { &aFromI, &aFromR, &iFromA, &rFromA };
	bool specialOK = true;
	for (const auto* inst : special)
		specialOK &= inst -> clockCycles () == 9 && inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleAt (8).numberTCycles () == 5 &&
			inst -> mCycleDataAt (8)._dataCycle == 6 && inst -> mCycleDataAt (8)._refreshCycle == 6;
	ok &= check (specialOK, "I/R transfers preserve the extended second fetch", o);

	const LD_SPFromIX spFromIX;
	const LD_SPFromIY spFromIY;
	const Instruction* stackPointers [] = { &spFromIX, &spFromIY };
	bool stackPointersOK = true;
	for (const auto* inst : stackPointers)
		stackPointersOK &= inst -> clockCycles () == 10 && inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCycleAt (9).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleAt (9).numberTCycles () == 6 &&
			inst -> mCycleDataAt (9)._refreshCycle == 6 && inst -> mCyclesData ().memoryWrites ().empty ();
	ok &= check (stackPointersOK, "SP from IX/IY retains internal time inside the second fetch", o);

	const LD_IXFromAddress load;
	const U3LD_AddressFromHL store;
	ok &= check (load.clockCycles () == 20 && store.clockCycles () == 20 &&
		load.mCyclesData ().numberMCycles () == 6 && store.mCyclesData ().numberMCycles () == 6 &&
		load.mCycleDataAt (16)._dataCycle == 16 && load.mCycleDataAt (19)._dataCycle == 19 &&
		load.mCyclesData ().memoryWrites ().empty () &&
		store.mCyclesData ().memoryWrites () == std::vector <size_t> ({ 4, 5 }) &&
		store.mCycleDataAt (16)._dataCycle == 16 && store.mCycleDataAt (19)._dataCycle == 19,
		"prefixed absolute transfers keep two ordered data bytes", o);

	const U1LD_AFromB registerAlias;
	const U2LD_A immediateAlias;
	ok &= check (registerAlias.clockCycles () == 8 && registerAlias.mCyclesData ().numberMCycles () == 2 &&
		registerAlias.mCyclesData ().memoryReads ().size () == 2 &&
		immediateAlias.clockCycles () == 11 && immediateAlias.mCyclesData ().numberMCycles () == 3 &&
		immediateAlias.mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
		immediateAlias.mCycleDataAt (10)._dataCycle == 10,
		"ignored index prefixes remain separate opcode fetches", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testINCDEC (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const INC_A incA;
	const DEC_A decA;
	const U1INC_A prefixedINC;
	const DEC_IYL prefixedDEC;
	ok &= check (incA.clockCycles () == 4 && decA.clockCycles () == 4 &&
		incA.mCyclesData ().numberMCycles () == 1 && decA.mCyclesData ().numberMCycles () == 1 &&
		prefixedINC.clockCycles () == 8 && prefixedDEC.clockCycles () == 8 &&
		prefixedINC.mCyclesData ().numberMCycles () == 2 && prefixedDEC.mCyclesData ().numberMCycles () == 2 &&
		prefixedINC.mCycleDataAt (7)._refreshCycle == 6 && prefixedDEC.mCycleDataAt (7)._refreshCycle == 6,
		"INC/DEC register fetches and index prefixes", o);

	// Zilog UM008011, pages 198-203: the two internal T-states belong to the fetch.
	const INC_BC incBC;
	const DEC_SP decSP;
	const INC_IX incIX;
	const DEC_IY decIY;
	ok &= check (incBC.clockCycles () == 6 && decSP.clockCycles () == 6 &&
		incBC.mCyclesData ().numberMCycles () == 1 && decSP.mCyclesData ().numberMCycles () == 1 &&
		incBC.mCycleAt (5).numberTCycles () == 6 && decSP.mCycleAt (5).numberTCycles () == 6 &&
		incIX.clockCycles () == 10 && decIY.clockCycles () == 10 &&
		incIX.mCyclesData ().numberMCycles () == 2 && decIY.mCyclesData ().numberMCycles () == 2 &&
		incIX.mCycleAt (9).numberTCycles () == 6 && decIY.mCycleAt (9).numberTCycles () == 6 &&
		incIX.mCyclesData ().memoryWrites ().empty () && decIY.mCyclesData ().memoryWrites ().empty (),
		"INC/DEC 16-bit internal time remains inside the fetch", o);

	// Pages 167 and 171: (HL) uses 4,4,3; reading finishes before the internal T-state.
	const INC_IndirectHL incHL;
	const DEC_IndirectHL decHL;
	const Instruction* indirect [] = { &incHL, &decHL };
	bool indirectOK = true;
	for (const auto* inst : indirect)
		indirectOK &= inst -> clockCycles () == 11 && inst -> mCyclesData ().numberMCycles () == 3 &&
			inst -> mCycleAt (7).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (7).numberTCycles () == 4 && inst -> mCycleDataAt (7)._dataCycle == 6 &&
			inst -> mCycleDataAt (7)._endCycle == 8 && inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 2 });
	ok &= check (indirectOK, "INC/DEC (HL) read-modify-write boundaries", o);

	// Pages 168-171: indexed operations use six M-cycles, 4,4,3,5,4,3.
	const INC_IndirectIndexIX incIndexedIX;
	const INC_IndirectIndexIY incIndexedIY;
	const DEC_IndirectIndexIX decIndexedIX;
	const DEC_IndirectIndexIY decIndexedIY;
	const Instruction* indexed [] = { &incIndexedIX, &incIndexedIY, &decIndexedIX, &decIndexedIY };
	bool indexedOK = true;
	for (const auto* inst : indexed)
		indexedOK &= inst -> clockCycles () == 23 && inst -> mCyclesData ().numberMCycles () == 6 &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleAt (11).type () == MCycle::Type::_INTERNAL &&
			inst -> mCycleDataAt (15)._dataCycle == absent && inst -> mCycleDataAt (15)._endCycle == 16 &&
			inst -> mCycleAt (19).numberTCycles () == 4 && inst -> mCycleDataAt (19)._dataCycle == 18 &&
			inst -> mCycleAt (20).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (22)._dataCycle == 22 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2, 4 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 5 });
	ok &= check (indexedOK, "INC/DEC IX/IY displacement, internal span, read and write ordering", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testADDSUB (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const ADD_AWithB addRegister;
	const ADC_AWithB adcRegister;
	const SUB_AWithB subRegister;
	const SBC_AWithB sbcRegister;
	const ADD_A addImmediate;
	const ADC_A adcImmediate;
	const SUB_A subImmediate;
	const SBC_A sbcImmediate;
	const ADD_AWithIndirectHL addHL;
	const ADC_AWithIndirectHL adcHL;
	const SUB_AWithIndirectHL subHL;
	const SBC_AWithIndirectHL sbcHL;
	const Instruction* ordinary [] = {
		&addRegister, &adcRegister, &subRegister, &sbcRegister,
		&addImmediate, &adcImmediate, &subImmediate, &sbcImmediate,
		&addHL, &adcHL, &subHL, &sbcHL };
	bool ordinaryOK = true;
	for (size_t n = 0; n < 12; n++)
	{
		const Instruction& inst = *ordinary [n];
		ordinaryOK &= inst.clockCycles () == ((n < 4) ? 4 : 7) &&
			inst.mCyclesData ().numberMCycles () == ((n < 4) ? 1 : 2) &&
			inst.mCyclesData ().memoryWrites ().empty ();
		if (n >= 4)
			ordinaryOK &= inst.mCycleAt (4).type () == MCycle::Type::_MEMORYREAD &&
				inst.mCycleDataAt (6)._dataCycle == 6;
	}
	ok &= check (ordinaryOK, "ADD/ADC/SUB/SBC register, immediate and (HL) operand timing", o);

	const U1ADD_AWithA ignoredADD;
	const U2ADC_AWithA ignoredADC;
	const U1SUB_AWithA ignoredSUB;
	const U2SBC_AWithA ignoredSBC;
	const Instruction* ignored [] = { &ignoredADD, &ignoredADC, &ignoredSUB, &ignoredSBC };
	bool ignoredOK = true;
	for (const auto* inst : ignored)
		ignoredOK &= inst -> clockCycles () == 8 && inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCycleAt (7).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 && inst -> mCyclesData ().memoryWrites ().empty ();
	ok &= check (ignoredOK, "arithmetic ignored prefixes retain a separate opcode fetch", o);

	// Zilog UM008011, pages 149-156: indexed arithmetic reads its operand at T19.
	const ADD_AWithIndirectIndexIX indexedADD;
	const ADC_AWithIndirectIndexIY indexedADC;
	const SUB_AWithIndirectIndexIX indexedSUB;
	const SBC_AWithIndirectIndexIY indexedSBC;
	const Instruction* indexed [] = { &indexedADD, &indexedADC, &indexedSUB, &indexedSBC };
	bool indexedOK = true;
	for (const auto* inst : indexed)
		indexedOK &= inst -> clockCycles () == 19 && inst -> mCyclesData ().numberMCycles () == 5 &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleAt (11).type () == MCycle::Type::_INTERNAL &&
			inst -> mCycleDataAt (15)._dataCycle == absent && inst -> mCycleDataAt (15)._endCycle == 16 &&
			inst -> mCycleAt (16).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (18)._dataCycle == 18 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2, 4 }) &&
			inst -> mCyclesData ().memoryWrites ().empty ();
	ok &= check (indexedOK, "indexed arithmetic displacement, internal span and operand read", o);

	// Pages 188-196: seven internal T-states form two M-cycles of four and three.
	// A single I7 would preserve the duration but lose the required boundary.
	const ADD_HLWithBC wideADD;
	const ADD_IXWithSP wideIX;
	const ADD_IYWithDE wideIY;
	const ADC_HLWithHL wideADC;
	const SBC_HLWithBC wideSBC;
	const Instruction* wide [] = { &wideADD, &wideIX, &wideIY, &wideADC, &wideSBC };
	bool wideOK = true;
	for (size_t n = 0; n < 5; n++)
	{
		const Instruction& inst = *wide [n];
		size_t internalStart = (n == 0) ? 4 : 8;
		wideOK &= inst.clockCycles () == ((n == 0) ? 11 : 15) &&
			inst.mCyclesData ().numberMCycles () == ((n == 0) ? 3 : 4) &&
			inst.mCycleAt (internalStart).type () == MCycle::Type::_INTERNAL &&
			inst.mCycleAt (internalStart).numberTCycles () == 4 &&
			inst.mCyclePositionAt (internalStart + 3)._localCycle == 3 &&
			inst.mCyclePositionAt (internalStart + 4)._localCycle == 0 &&
			inst.mCycleAt (internalStart + 4).numberTCycles () == 3 &&
			inst.mCycleDataAt (internalStart + 6)._dataCycle == absent &&
			inst.mCycleDataAt (internalStart + 6)._refreshCycle == absent &&
			inst.mCyclesData ().internalCycles ().size () == 2 &&
			inst.mCyclesData ().memoryReads ().size () == ((n == 0) ? 1 : 2) &&
			inst.mCyclesData ().memoryWrites ().empty ();
	}
	ok &= check (wideOK, "16-bit ADD/ADC/SBC preserve two internal M-cycles without data transfers", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testLogicalCP (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const AND_B andRegister;
	const OR_B orRegister;
	const XOR_B xorRegister;
	const CP_WithB cpRegister;
	const AND andImmediate;
	const OR orImmediate;
	const XOR xorImmediate;
	const CP cpImmediate;
	const AND_IndirectHL andHL;
	const OR_IndirectHL orHL;
	const XOR_IndirectHL xorHL;
	const CP_WithIndirectHL cpHL;
	const Instruction* ordinary [] = {
		&andRegister, &orRegister, &xorRegister, &cpRegister,
		&andImmediate, &orImmediate, &xorImmediate, &cpImmediate,
		&andHL, &orHL, &xorHL, &cpHL };
	bool ordinaryOK = true;
	for (size_t n = 0; n < 12; n++)
	{
		const Instruction& inst = *ordinary [n];
		ordinaryOK &= inst.clockCycles () == ((n < 4) ? 4 : 7) &&
			inst.mCyclesData ().numberMCycles () == ((n < 4) ? 1 : 2) &&
			inst.mCyclesData ().memoryWrites ().empty () && inst.mCyclesData ().internalCycles ().empty ();
		if (n >= 4)
			ordinaryOK &= inst.mCycleAt (4).type () == MCycle::Type::_MEMORYREAD &&
				inst.mCycleDataAt (6)._dataCycle == 6;
	}
	ok &= check (ordinaryOK, "AND/OR/XOR/CP register, immediate and (HL) read timing", o);

	const U1AND_A prefixedAND;
	const U2OR_A prefixedOR;
	const XOR_IXH prefixedXOR;
	const CP_WithIYL prefixedCP;
	const Instruction* prefixed [] = { &prefixedAND, &prefixedOR, &prefixedXOR, &prefixedCP };
	bool prefixedOK = true;
	for (const auto* inst : prefixed)
		prefixedOK &= inst -> clockCycles () == 8 && inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst -> mCycleAt (7).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 && inst -> mCyclesData ().memoryWrites ().empty ();
	ok &= check (prefixedOK, "logical and comparison index-register and ignored-prefix fetches", o);

	// Zilog UM008011, pages 157-164: all four indexed families use 4,4,3,5,3.
	// The internal span calculates the address; it is not another data read.
	const AND_IndirectIndexIX andIX;
	const AND_IndirectIndexIY andIY;
	const OR_IndirectIndexIX orIX;
	const OR_IndirectIndexIY orIY;
	const XOR_IndirectIndexIX xorIX;
	const XOR_IndirectIndexIY xorIY;
	const CP_WithIndirectIndexIX cpIX;
	const CP_WithIndirectIndexIY cpIY;
	const Instruction* indexed [] = { &andIX, &andIY, &orIX, &orIY, &xorIX, &xorIY, &cpIX, &cpIY };
	bool indexedOK = true;
	for (const auto* inst : indexed)
		indexedOK &= inst -> clockCycles () == 19 && inst -> mCyclesData ().numberMCycles () == 5 &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleAt (11).type () == MCycle::Type::_INTERNAL &&
			inst -> mCycleDataAt (15)._dataCycle == absent && inst -> mCycleDataAt (15)._endCycle == 16 &&
			inst -> mCycleAt (16).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (18)._dataCycle == 18 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2, 4 }) &&
			inst -> mCyclesData ().memoryWrites ().empty ();
	ok &= check (indexedOK, "logical and comparison IX/IY ordered reads without memory writes", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testALUCTRL (std::ostream& o)
{
	bool ok = true;
	const DAA daa;
	const CPL cpl;
	const CCF ccf;
	const SCF scf;
	const NOP nop;
	const HALT halt;
	const DI di;
	const EI ei;
	const Instruction* single [] = { &daa, &cpl, &ccf, &scf, &nop, &halt, &di, &ei };
	bool singleOK = true;
	// HALT describes one four-T-state unit; the existing CPU logic owns repetition.
	// Metadata alone neither releases HALT nor changes interrupt acceptance.
	for (const auto* inst : single)
		singleOK &= inst -> clockCycles () == 4 && inst -> mCyclesData ().numberTCycles () == 4 &&
			inst -> mCyclesData ().numberMCycles () == 1 &&
			inst -> mCycleAt (3).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._dataCycle == 2 && inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0 }) &&
			inst -> mCyclesData ().memoryWrites ().empty () && inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (singleOK, "ALU/control single fetches including one HALT unit", o);

	const NEG neg;
	const IM_0 im0;
	const IM_1 im1;
	const IM_2 im2;
	const Instruction* prefixed [] = { &neg, &im0, &im1, &im2 };
	bool prefixedOK = true;
	for (const auto* inst : prefixed)
		prefixedOK &= inst -> clockCycles () == 8 && inst -> mCyclesData ().numberTCycles () == 8 &&
			inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCyclePositionAt (4)._localCycle == 0 &&
			inst -> mCycleAt (7).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._dataCycle == 6 && inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst -> mCyclesData ().memoryWrites ().empty () && inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (prefixedOK, "NEG and IM retain separate ED and operation fetches", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testRotations (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const RLC_A rlca;
	const RL_A rla;
	const RRC_A rrca;
	const RR_A rra;
	const RLC_ASlower rlcRegister;
	const RL_B rlRegister;
	const RRC_ASlower rrcRegister;
	const RR_C rrRegister;
	const Instruction* registers [] = {
		&rlca, &rla, &rrca, &rra, &rlcRegister, &rlRegister, &rrcRegister, &rrRegister };
	bool registersOK = true;
	for (size_t n = 0; n < 8; n++)
	{
		const Instruction& inst = *registers [n];
		registersOK &= inst.clockCycles () == ((n < 4) ? 4 : 8) &&
			inst.mCyclesData ().numberMCycles () == ((n < 4) ? 1 : 2) &&
			inst.mCycleAt (inst.clockCycles () - 1).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCyclesData ().memoryWrites ().empty () && inst.mCyclesData ().internalCycles ().empty ();
	}
	ok &= check (registersOK, "accumulator and CB register rotation fetches", o);

	// Zilog UM008011: (HL) rotations use 4,4,4,3 T-states.
	// The operand read includes an internal T-state before the write starts.
	const RLC_IndirectHL rlcHL;
	const RL_IndirectHL rlHL;
	const RRC_IndirectHL rrcHL;
	const RR_IndirectHL rrHL;
	const Instruction* indirect [] = { &rlcHL, &rlHL, &rrcHL, &rrHL };
	bool indirectOK = true;
	for (const auto* inst : indirect)
		indirectOK &= inst -> clockCycles () == 15 && inst -> mCyclesData ().numberMCycles () == 4 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (8).numberTCycles () == 4 &&
			inst -> mCycleDataAt (11)._dataCycle == 10 &&
			inst -> mCycleDataAt (11)._refreshCycle == absent &&
			inst -> mCycleAt (12).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (14)._dataCycle == 14 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 3 });
	ok &= check (indirectOK, "CB (HL) rotations preserve read-modify-write boundaries", o);

	// Indexed CB rotations use 4,4,3,5,4,3; only DD/FD and CB are M1 fetches.
	// Reading the final operation byte does not generate a third refresh.
	// Undocumented register copies reuse the memory result without another bus cycle.
	const RLC_IndirectIndexIX rlcIX;
	const RLC_IndirectIndexIYCopyA rlcIY;
	const RL_IndirectIndexIXCopyB rlIX;
	const RL_IndirectIndexIY rlIY;
	const RRC_IndirectIndexIXCopyC rrcIX;
	const RRC_IndirectIndexIY rrcIY;
	const RR_IndirectIndexIX rrIX;
	const RR_IndirectIndexIYCopyL rrIY;
	const Instruction* indexed [] = { &rlcIX, &rlcIY, &rlIX, &rlIY, &rrcIX, &rrcIY, &rrIX, &rrIY };
	bool indexedOK = true;
	for (const auto* inst : indexed)
		indexedOK &= inst -> clockCycles () == 23 && inst -> mCyclesData ().numberMCycles () == 6 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleAt (11).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (11).numberTCycles () == 5 &&
			inst -> mCycleDataAt (15)._dataCycle == 13 &&
			inst -> mCycleDataAt (15)._refreshCycle == absent &&
			inst -> mCyclePositionAt (16)._localCycle == 0 &&
			inst -> mCycleAt (16).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (19)._dataCycle == 18 &&
			inst -> mCycleDataAt (19)._refreshCycle == absent &&
			inst -> mCycleAt (20).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (22)._dataCycle == 22 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2, 3, 4 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 5 });
	ok &= check (indexedOK, "DDCB/FDCB rotations read the operation byte without M1 or extra copy cycles", o);

	// RLD/RRD use 4,4,3,4,3: the four internal T-states form a separate M-cycle.
	const RLD rld;
	const RRD rrd;
	const Instruction* digits [] = { &rld, &rrd };
	bool digitsOK = true;
	for (const auto* inst : digits)
		digitsOK &= inst -> clockCycles () == 18 && inst -> mCyclesData ().numberMCycles () == 5 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleAt (11).type () == MCycle::Type::_INTERNAL &&
			inst -> mCycleDataAt (14)._firstCycle == 11 &&
			inst -> mCycleDataAt (14)._endCycle == 15 &&
			inst -> mCycleDataAt (14)._dataCycle == absent &&
			inst -> mCycleAt (15).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (17)._dataCycle == 17 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 4 });
	ok &= check (digitsOK, "RLD/RRD preserve the separate internal M-cycle", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testShifts (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const SLA_A slaRegister;
	const SLL_B sllRegister;
	const SRA_H sraRegister;
	const SRL_L srlRegister;
	const Instruction* registers [] = { &slaRegister, &sllRegister, &sraRegister, &srlRegister };
	bool registersOK = true;
	for (const auto* inst : registers)
		registersOK &= inst -> clockCycles () == 8 && inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst -> mCyclesData ().memoryWrites ().empty () && inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (registersOK, "SLA/SLL/SRA/SRL register operations use two CB fetches", o);

	// Like rotations, (HL) shifts retain an internal T-state in the operand read.
	const SLA_IndirectHL slaHL;
	const SLL_IndirectHL sllHL;
	const SRA_IndirectHL sraHL;
	const SRL_IndirectHL srlHL;
	const Instruction* indirect [] = { &slaHL, &sllHL, &sraHL, &srlHL };
	bool indirectOK = true;
	for (const auto* inst : indirect)
		indirectOK &= inst -> clockCycles () == 15 && inst -> mCyclesData ().numberMCycles () == 4 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (8).numberTCycles () == 4 &&
			inst -> mCycleDataAt (11)._dataCycle == 10 &&
			inst -> mCycleDataAt (11)._refreshCycle == absent &&
			inst -> mCyclePositionAt (12)._localCycle == 0 &&
			inst -> mCycleAt (12).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (14)._dataCycle == 14 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 3 });
	ok &= check (indirectOK, "CB (HL) shifts preserve 4,4,4,3 read-modify-write timing", o);

	// DDCB/FDCB read displacement and operation bytes before the operand.
	// Only the first two M-cycles refresh; copying the result adds no bus transfer.
	const SLA_IndirectIndexIX slaIX;
	const SLA_IndirectIndexIYCopyA slaIY;
	const SLL_IndirectIndexIXCopyB sllIX;
	const SLL_IndirectIndexIY sllIY;
	const SRA_IndirectIndexIX sraIX;
	const SRA_IndirectIndexIYCopyH sraIY;
	const SRL_IndirectIndexIXCopyL srlIX;
	const SRL_IndirectIndexIY srlIY;
	const Instruction* indexed [] = { &slaIX, &slaIY, &sllIX, &sllIY, &sraIX, &sraIY, &srlIX, &srlIY };
	bool indexedOK = true;
	for (const auto* inst : indexed)
		indexedOK &= inst -> clockCycles () == 23 && inst -> mCyclesData ().numberMCycles () == 6 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleDataAt (10)._refreshCycle == absent &&
			inst -> mCycleAt (11).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (11).numberTCycles () == 5 &&
			inst -> mCycleDataAt (15)._dataCycle == 13 &&
			inst -> mCycleDataAt (15)._refreshCycle == absent &&
			inst -> mCyclePositionAt (16)._localCycle == 0 &&
			inst -> mCycleAt (16).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (19)._dataCycle == 18 &&
			inst -> mCycleDataAt (19)._refreshCycle == absent &&
			inst -> mCycleAt (20).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (22)._dataCycle == 22 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2, 3, 4 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 5 });
	ok &= check (indexedOK, "DDCB/FDCB shifts preserve two M1 fetches and 4,4,3,5,4,3 timing", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testBIT (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const BIT_0WithA bit0Register;
	const BIT_2WithB bit2Register;
	const BIT_5WithH bit5Register;
	const BIT_7WithL bit7Register;
	const Instruction* registers [] = { &bit0Register, &bit2Register, &bit5Register, &bit7Register };
	bool registersOK = true;
	for (const auto* inst : registers)
		registersOK &= inst -> clockCycles () == 8 && inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst -> mCyclesData ().memoryWrites ().empty () && inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (registersOK, "BIT register operations use two CB fetches without writes", o);

	// Zilog UM008011, page 245: BIT (HL) uses 4,4,4, with no write-back cycle.
	const BIT_0WithIndirectHL bit0HL;
	const BIT_1WithIndirectHL bit1HL;
	const BIT_6WithIndirectHL bit6HL;
	const BIT_7WithIndirectHL bit7HL;
	const Instruction* indirect [] = { &bit0HL, &bit1HL, &bit6HL, &bit7HL };
	bool indirectOK = true;
	for (const auto* inst : indirect)
		indirectOK &= inst -> clockCycles () == 12 && inst -> mCyclesData ().numberMCycles () == 3 &&
			inst -> mCyclePositionAt (8)._localCycle == 0 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (8).numberTCycles () == 4 &&
			inst -> mCycleDataAt (11)._dataCycle == 10 &&
			inst -> mCycleDataAt (11)._endCycle == 12 &&
			inst -> mCycleDataAt (11)._refreshCycle == absent &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
			inst -> mCyclesData ().memoryWrites ().empty () && inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (indirectOK, "BIT (HL) ends after its extended operand read", o);

	// Pages 247-249: indexed BIT uses 4,4,3,5,4, ending after the operand read.
	// The final operation byte is not M1; undocumented aliases add no transfer.
	const BIT_0WithIndirectIndexIX bit0IX;
	const U1BIT_1WithIndirectIndexIY bit1IY;
	const U2BIT_2WithIndirectIndexIX bit2IX;
	const U3BIT_3WithIndirectIndexIY bit3IY;
	const U4BIT_4WithIndirectIndexIX bit4IX;
	const U5BIT_5WithIndirectIndexIY bit5IY;
	const U6BIT_6WithIndirectIndexIX bit6IX;
	const U7BIT_7WithIndirectIndexIY bit7IY;
	const Instruction* indexed [] = {
		&bit0IX, &bit1IY, &bit2IX, &bit3IY, &bit4IX, &bit5IY, &bit6IX, &bit7IY };
	bool indexedOK = true;
	for (const auto* inst : indexed)
		indexedOK &= inst -> clockCycles () == 20 && inst -> mCyclesData ().numberMCycles () == 5 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleDataAt (10)._refreshCycle == absent &&
			inst -> mCycleAt (11).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (11).numberTCycles () == 5 &&
			inst -> mCycleDataAt (15)._dataCycle == 13 &&
			inst -> mCycleDataAt (15)._refreshCycle == absent &&
			inst -> mCyclePositionAt (16)._localCycle == 0 &&
			inst -> mCycleAt (16).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (16).numberTCycles () == 4 &&
			inst -> mCycleDataAt (19)._dataCycle == 18 &&
			inst -> mCycleDataAt (19)._endCycle == 20 &&
			inst -> mCycleDataAt (19)._refreshCycle == absent &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2, 3, 4 }) &&
			inst -> mCyclesData ().memoryWrites ().empty () && inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (indexedOK, "DDCB/FDCB BIT aliases preserve two M1 fetches and no writes", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testSETRES (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const SET_0WithA set0Register;
	const SET_7WithL set7Register;
	const RES_0WithB res0Register;
	const RES_7WithH res7Register;
	const Instruction* registers [] = { &set0Register, &set7Register, &res0Register, &res7Register };
	bool registersOK = true;
	for (const auto* inst : registers)
		registersOK &= inst -> clockCycles () == 8 && inst -> mCyclesData ().numberMCycles () == 2 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst -> mCyclesData ().memoryWrites ().empty () && inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (registersOK, "SET/RES registers use two fetches without memory writes", o);

	// Zilog UM008011, pages 253 and 260: (HL) uses 4,4,4,3.
	// The internal T-state belongs to the read; the write is a separate M-cycle.
	const SET_0WithIndirectHL set0HL;
	const SET_7WithIndirectHL set7HL;
	const RES_0WithIndirectHL res0HL;
	const RES_7WithIndirectHL res7HL;
	const Instruction* indirect [] = { &set0HL, &set7HL, &res0HL, &res7HL };
	bool indirectOK = true;
	for (const auto* inst : indirect)
		indirectOK &= inst -> clockCycles () == 15 && inst -> mCyclesData ().numberMCycles () == 4 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (8).numberTCycles () == 4 &&
			inst -> mCycleDataAt (11)._dataCycle == 10 &&
			inst -> mCycleDataAt (11)._refreshCycle == absent &&
			inst -> mCyclePositionAt (12)._localCycle == 0 &&
			inst -> mCycleAt (12).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (14)._dataCycle == 14 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 3 });
	ok &= check (indirectOK, "SET/RES (HL) preserve the extended read and final write", o);

	// Pages 255-260: indexed SET/RES use 4,4,3,5,4,3.
	// Only DD/FD and CB are M1 fetches; register copies need no extra transfer.
	const SET_0WithIndirectIndexIX set0IX;
	const SET_1WithIndirectIndexIY set1IY;
	const SET_2WithIndirectIndexIXCopyA set2IX;
	const SET_3WithIndirectIndexIYCopyH set3IY;
	const RES_4WithIndirectIndexIX res4IX;
	const RES_5WithIndirectIndexIY res5IY;
	const RES_6WithIndirectIndexIXCopyB res6IX;
	const RES_7WithIndirectIndexIYCopyL res7IY;
	const Instruction* indexed [] = {
		&set0IX, &set1IY, &set2IX, &set3IY, &res4IX, &res5IY, &res6IX, &res7IY };
	bool indexedOK = true;
	for (const auto* inst : indexed)
		indexedOK &= inst -> clockCycles () == 23 && inst -> mCyclesData ().numberMCycles () == 6 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (7)._refreshCycle == 6 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCycleDataAt (10)._refreshCycle == absent &&
			inst -> mCycleAt (11).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (11).numberTCycles () == 5 &&
			inst -> mCycleDataAt (15)._dataCycle == 13 &&
			inst -> mCycleDataAt (15)._refreshCycle == absent &&
			inst -> mCyclePositionAt (16)._localCycle == 0 &&
			inst -> mCycleAt (16).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleAt (16).numberTCycles () == 4 &&
			inst -> mCycleDataAt (19)._dataCycle == 18 &&
			inst -> mCycleDataAt (19)._refreshCycle == absent &&
			inst -> mCycleAt (20).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (22)._dataCycle == 22 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2, 3, 4 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 5 }) &&
			inst -> mCyclesData ().ioCycles ().empty ();
	ok &= check (indexedOK, "DDCB/FDCB SET/RES retain two M1 fetches and one operand write", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testSTACK (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const PUSH_AF pushAF;
	const PUSH_BC pushBC;
	const PUSH_DE pushDE;
	const PUSH_HL pushHL;
	const PUSH_IX pushIX;
	const PUSH_IY pushIY;
	const Instruction* pushes [] = { &pushAF, &pushBC, &pushDE, &pushHL, &pushIX, &pushIY };
	bool pushesOK = true;
	// Zilog UM008011, pages 115-118: PUSH uses 5,3,3, preceded by 4 for IX/IY.
	// The extra T-state extends the operation fetch rather than adding an M-cycle.
	// Stack semantics write the high byte first, then the low byte.
	for (size_t n = 0; n < 6; n++)
	{
		const Instruction& inst = *pushes [n];
		size_t offset = (n < 4) ? 0 : 4;
		pushesOK &= inst.clockCycles () == offset + 11 &&
			inst.mCyclesData ().numberMCycles () == ((n < 4) ? 3 : 4) &&
			inst.mCycleAt (offset).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCycleAt (offset).numberTCycles () == 5 &&
			inst.mCycleDataAt (offset + 4)._dataCycle == offset + 2 &&
			inst.mCycleDataAt (offset + 4)._refreshCycle == offset + 2 &&
			inst.mCyclePositionAt (offset + 5)._localCycle == 0 &&
			inst.mCycleAt (offset + 5).type () == MCycle::Type::_MEMORYWRITE &&
			inst.mCycleDataAt (offset + 7)._dataCycle == offset + 7 &&
			inst.mCycleDataAt (offset + 7)._refreshCycle == absent &&
			inst.mCyclePositionAt (offset + 8)._localCycle == 0 &&
			inst.mCycleAt (offset + 8).type () == MCycle::Type::_MEMORYWRITE &&
			inst.mCycleDataAt (offset + 10)._dataCycle == offset + 10 &&
			inst.mCyclesData ().memoryReads () == ((n < 4) ?
				std::vector <size_t> ({ 0 }) : std::vector <size_t> ({ 0, 1 })) &&
			inst.mCyclesData ().memoryWrites () == ((n < 4) ?
				std::vector <size_t> ({ 1, 2 }) : std::vector <size_t> ({ 2, 3 })) &&
			inst.mCyclesData ().internalCycles ().empty () && inst.mCyclesData ().ioCycles ().empty ();
		if (n >= 4)
			pushesOK &= inst.mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleAt (0).numberTCycles () == 4 && inst.mCycleDataAt (3)._refreshCycle == 2;
	}
	ok &= check (pushesOK, "PUSH retains the extended fetch and two ordered stack writes", o);

	const POP_AF popAF;
	const POP_BC popBC;
	const POP_DE popDE;
	const POP_HL popHL;
	const POP_IX popIX;
	const POP_IY popIY;
	const Instruction* pops [] = { &popAF, &popBC, &popDE, &popHL, &popIX, &popIY };
	bool popsOK = true;
	// Pages 119-123: POP uses 4,3,3, preceded by 4 for IX/IY.
	// Stack semantics read the low byte before the high byte, without a write-back.
	for (size_t n = 0; n < 6; n++)
	{
		const Instruction& inst = *pops [n];
		size_t offset = (n < 4) ? 0 : 4;
		popsOK &= inst.clockCycles () == offset + 10 &&
			inst.mCyclesData ().numberMCycles () == ((n < 4) ? 3 : 4) &&
			inst.mCycleAt (offset).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCycleAt (offset).numberTCycles () == 4 &&
			inst.mCycleDataAt (offset + 3)._refreshCycle == offset + 2 &&
			inst.mCyclePositionAt (offset + 4)._localCycle == 0 &&
			inst.mCycleAt (offset + 4).type () == MCycle::Type::_MEMORYREAD &&
			inst.mCycleDataAt (offset + 6)._dataCycle == offset + 6 &&
			inst.mCycleDataAt (offset + 6)._refreshCycle == absent &&
			inst.mCyclePositionAt (offset + 7)._localCycle == 0 &&
			inst.mCycleAt (offset + 7).type () == MCycle::Type::_MEMORYREAD &&
			inst.mCycleDataAt (offset + 9)._dataCycle == offset + 9 &&
			inst.mCyclesData ().memoryReads () == ((n < 4) ?
				std::vector <size_t> ({ 0, 1, 2 }) : std::vector <size_t> ({ 0, 1, 2, 3 })) &&
			inst.mCyclesData ().memoryWrites ().empty () &&
			inst.mCyclesData ().internalCycles ().empty () && inst.mCyclesData ().ioCycles ().empty ();
		if (n >= 4)
			popsOK &= inst.mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleAt (0).numberTCycles () == 4 && inst.mCycleDataAt (3)._refreshCycle == 2;
	}
	ok &= check (popsOK, "POP retains two ordered stack reads and separate IX/IY prefix fetches", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testJUMP (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const JP jp;
	const JP_NZ jpNZ;
	const JP_Z jpZ;
	const JP_NC jpNC;
	const JP_C jpC;
	const JP_PO jpPO;
	const JP_PE jpPE;
	const JP_P jpP;
	const JP_M jpM;
	const Instruction* absolute [] = { &jp, &jpNZ, &jpZ, &jpNC, &jpC, &jpPO, &jpPE, &jpP, &jpM };
	bool absoluteOK = true;
	// JP cc reads both address bytes even when the condition is false.
	for (const auto* inst : absolute)
		absoluteOK &= inst -> clockCycles () == 10 && inst -> numberMCyclesStructures () == 1 &&
			inst -> mCyclesData ().numberMCycles () == 3 &&
			inst -> mCycleAt (4).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (6)._dataCycle == 6 &&
			inst -> mCycleAt (7).type () == MCycle::Type::_MEMORYREAD &&
			inst -> mCycleDataAt (9)._dataCycle == 9 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
			inst -> mCyclesData ().memoryWrites ().empty ();
	ok &= check (absoluteOK, "JP and JP cc preserve both address reads and constant duration", o);

	const JP_IndirectHL jpHL;
	const JP_IndirectIX jpIX;
	const JP_IndirectIY jpIY;
	const Instruction* indirect [] = { &jpHL, &jpIX, &jpIY };
	bool indirectOK = true;
	// Parentheses identify a register-held target, not an operand read from memory.
	for (size_t n = 0; n < 3; n++)
	{
		const Instruction& inst = *indirect [n];
		indirectOK &= inst.clockCycles () == ((n == 0) ? 4 : 8) &&
			inst.mCyclesData ().numberMCycles () == ((n == 0) ? 1 : 2) &&
			inst.mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCycleAt (inst.clockCycles () - 1).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCyclesData ().memoryReads () == ((n == 0) ?
				std::vector <size_t> ({ 0 }) : std::vector <size_t> ({ 0, 1 })) &&
			inst.mCyclesData ().memoryWrites ().empty ();
	}
	ok &= check (indirectOK, "JP (HL/IX/IY) fetches without reading target memory", o);

	const JR jr;
	ok &= check (jr.clockCycles () == 12 && jr.numberMCyclesStructures () == 1 &&
		jr.mCyclesData ().numberMCycles () == 3 &&
		jr.mCycleDataAt (6)._dataCycle == 6 && jr.mCycleAt (7).type () == MCycle::Type::_INTERNAL &&
		jr.mCycleAt (7).numberTCycles () == 5 && jr.mCycleDataAt (11)._dataCycle == absent &&
		jr.mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
		jr.mCyclesData ().memoryWrites ().empty (), "JR retains its unconditional internal branch cycle", o);

	const JR_NZ jrNZ;
	const JR_Z jrZ;
	const JR_NC jrNC;
	const JR_C jrC;
	const DJNZ djnz;
	const Instruction* conditional [] = { &jrNZ, &jrZ, &jrNC, &jrC, &djnz };
	bool conditionalOK = true;
	// Zilog UM008011, pages 267-278: taken branches add five internal T-states.
	// Alternative 0 remains nominal; metadata does not select the execution path.
	for (size_t n = 0; n < 5; n++)
	{
		const Instruction& inst = *conditional [n];
		size_t fetchLength = (n < 4) ? 4 : 5;
		conditionalOK &= inst.clockCycles () == fetchLength + 3 && inst.numberMCyclesStructures () == 2;
		for (size_t alternative = 0; alternative < 2; alternative++)
		{
			const MCyclesData& data = inst.mCyclesData (alternative);
			conditionalOK &= data.numberTCycles () == fetchLength + 3 + alternative * 5 &&
				data.numberMCycles () == 2 + alternative &&
				inst.mCycleAt (0, alternative).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleAt (0, alternative).numberTCycles () == fetchLength &&
				inst.mCycleDataAt (fetchLength - 1, alternative)._refreshCycle == 2 &&
				inst.mCycleAt (fetchLength, alternative).type () == MCycle::Type::_MEMORYREAD &&
				inst.mCycleDataAt (fetchLength + 2, alternative)._dataCycle == fetchLength + 2 &&
				data.memoryReads () == std::vector <size_t> ({ 0, 1 }) && data.memoryWrites ().empty ();
		}
		conditionalOK &= inst.mCyclesData (0).internalCycles ().empty () &&
			inst.mCyclesData (1).internalCycles () == std::vector <size_t> ({ 2 }) &&
			inst.mCyclePositionAt (fetchLength + 3, 1)._localCycle == 0 &&
			inst.mCycleAt (fetchLength + 3, 1).type () == MCycle::Type::_INTERNAL &&
			inst.mCycleAt (fetchLength + 3, 1).numberTCycles () == 5 &&
			inst.mCycleDataAt (fetchLength + 7, 1)._dataCycle == absent &&
			inst.mCycleDataAt (fetchLength + 7, 1)._refreshCycle == absent;
	}
	ok &= check (conditionalOK, "JR cc and DJNZ preserve complete not-taken and taken alternatives", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testCALL (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const CALL call;
	const CALL_NZ callNZ;
	const CALL_Z callZ;
	const CALL_NC callNC;
	const CALL_C callC;
	const CALL_PO callPO;
	const CALL_PE callPE;
	const CALL_P callP;
	const CALL_M callM;
	const Instruction* calls [] = {
		&call, &callNZ, &callZ, &callNC, &callC, &callPO, &callPE, &callP, &callM };
	bool callsOK = true;
	// Zilog UM008011, pages 281-284: a taken CALL extends the high address read.
	// Both paths read the target, but only the taken path writes the return address.
	for (size_t n = 0; n < 9; n++)
	{
		const Instruction& inst = *calls [n];
		callsOK &= inst.clockCycles () == ((n == 0) ? 17 : 10) &&
			inst.numberMCyclesStructures () == ((n == 0) ? 1 : 2);
		for (size_t alternative = 0; alternative < inst.numberMCyclesStructures (); alternative++)
		{
			bool taken = n == 0 || alternative == 1;
			const MCyclesData& data = inst.mCyclesData (alternative);
			callsOK &= data.numberTCycles () == (taken ? 17 : 10) &&
				data.numberMCycles () == (taken ? 5 : 3) &&
				inst.mCycleAt (0, alternative).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleDataAt (3, alternative)._refreshCycle == 2 &&
				inst.mCycleAt (4, alternative).type () == MCycle::Type::_MEMORYREAD &&
				inst.mCycleDataAt (6, alternative)._dataCycle == 6 &&
				inst.mCycleAt (7, alternative).type () == MCycle::Type::_MEMORYREAD &&
				inst.mCycleAt (7, alternative).numberTCycles () == (taken ? 4 : 3) &&
				inst.mCycleDataAt (9, alternative)._dataCycle == 9 &&
				data.memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
				data.memoryWrites () == (taken ? std::vector <size_t> ({ 3, 4 }) : std::vector <size_t> ()) &&
				data.internalCycles ().empty () && data.ioCycles ().empty ();
			if (taken)
				callsOK &= inst.mCycleDataAt (10, alternative)._refreshCycle == absent &&
					inst.mCyclePositionAt (11, alternative)._localCycle == 0 &&
					inst.mCycleAt (11, alternative).type () == MCycle::Type::_MEMORYWRITE &&
					inst.mCycleDataAt (13, alternative)._dataCycle == 13 &&
					inst.mCycleAt (14, alternative).type () == MCycle::Type::_MEMORYWRITE &&
					inst.mCycleDataAt (16, alternative)._dataCycle == 16;
		}
	}
	ok &= check (callsOK, "CALL alternatives preserve target reads, extended read and conditional stack writes", o);

	const RST_00 rst00;
	const RST_08 rst08;
	const RST_10 rst10;
	const RST_18 rst18;
	const RST_20 rst20;
	const RST_28 rst28;
	const RST_30 rst30;
	const RST_38 rst38;
	const Instruction* restarts [] = { &rst00, &rst08, &rst10, &rst18, &rst20, &rst28, &rst30, &rst38 };
	bool restartsOK = true;
	// These are software RST opcodes, not interrupt-acknowledge sequences.
	for (const auto* inst : restarts)
		restartsOK &= inst -> clockCycles () == 11 && inst -> mCyclesData ().numberMCycles () == 3 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleAt (0).numberTCycles () == 5 &&
			inst -> mCycleDataAt (4)._refreshCycle == 2 &&
			inst -> mCycleAt (5).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (7)._dataCycle == 7 &&
			inst -> mCycleAt (8).type () == MCycle::Type::_MEMORYWRITE &&
			inst -> mCycleDataAt (10)._dataCycle == 10 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0 }) &&
			inst -> mCyclesData ().memoryWrites () == std::vector <size_t> ({ 1, 2 }) &&
			inst -> mCyclesData ().interruptAcknowledges ().empty ();
	ok &= check (restartsOK, "all RST vectors preserve 5,3,3 software instruction timing", o);

	const RET ret;
	const RET_I reti;
	const RET_N retn;
	const RET_NZ retNZ;
	const RET_Z retZ;
	const RET_NC retNC;
	const RET_C retC;
	const RET_PO retPO;
	const RET_PE retPE;
	const RET_P retP;
	const RET_M retM;
	const Instruction* returns [] = {
		&ret, &reti, &retn, &retNZ, &retZ, &retNC, &retC, &retPO, &retPE, &retP, &retM };
	bool returnsOK = true;
	// RET cc uses a five-T-state fetch on both paths; the taken path adds two reads.
	// RETI/RETN have two ordinary fetches and retain their existing interrupt effects.
	for (size_t n = 0; n < 11; n++)
	{
		const Instruction& inst = *returns [n];
		bool conditional = n >= 3;
		size_t readStart = conditional ? 5 : ((n == 0) ? 4 : 8);
		returnsOK &= inst.clockCycles () == (conditional ? 5 : readStart + 6) &&
			inst.numberMCyclesStructures () == (conditional ? 2 : 1);
		for (size_t alternative = 0; alternative < inst.numberMCyclesStructures (); alternative++)
		{
			bool taken = !conditional || alternative == 1;
			const MCyclesData& data = inst.mCyclesData (alternative);
			returnsOK &= data.numberTCycles () == readStart + (taken ? 6 : 0) &&
				data.numberMCycles () == ((readStart == 8) ? 2 : 1) + (taken ? 2 : 0) &&
				inst.mCycleAt (0, alternative).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleAt (0, alternative).numberTCycles () == (conditional ? 5 : 4) &&
				data.memoryWrites ().empty () && data.ioCycles ().empty () && data.interruptAcknowledges ().empty ();
			if (taken)
				returnsOK &= inst.mCyclePositionAt (readStart, alternative)._localCycle == 0 &&
					inst.mCycleAt (readStart, alternative).type () == MCycle::Type::_MEMORYREAD &&
					inst.mCycleDataAt (readStart + 2, alternative)._dataCycle == readStart + 2 &&
					inst.mCycleDataAt (readStart + 2, alternative)._refreshCycle == absent &&
					inst.mCycleAt (readStart + 3, alternative).type () == MCycle::Type::_MEMORYREAD &&
					inst.mCycleDataAt (readStart + 5, alternative)._dataCycle == readStart + 5 &&
					data.memoryReads () == ((readStart == 8) ?
						std::vector <size_t> ({ 0, 1, 2, 3 }) : std::vector <size_t> ({ 0, 1, 2 }));
			else
				returnsOK &= data.memoryReads () == std::vector <size_t> ({ 0 });
			if (readStart == 8)
				returnsOK &= inst.mCycleAt (4, alternative).type () == MCycle::Type::_OPCODEFETCH &&
					inst.mCycleDataAt (7, alternative)._refreshCycle == 6;
		}
	}
	ok &= check (returnsOK, "RET alternatives and RETI/RETN preserve conditional stack reads", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testEX (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const unsigned int w = MCHEmul::CPUCycle::_WRITE, i = MCHEmul::CPUCycle::_INTERNAL;
	const MCycle extendedWrite = _Z80_MW5 ();
	ok &= check (extendedWrite.type () == MCycle::Type::_MEMORYWRITE &&
		extendedWrite.tCycles () == MCHEmul::CycleStructure ({ w, w, w, i, i }) &&
		extendedWrite.dataCycle () == 2 && extendedWrite.contentionCycle () == 0 &&
		extendedWrite.refreshCycle () == absent, "MW5 retains one transfer before two internal T-states", o);

	const EX_AF exAF;
	const EX_DEWithHL exDEHL;
	const EX_X exX;
	const Instruction* registers [] = { &exAF, &exDEHL, &exX };
	bool registersOK = true;
	for (const auto* inst : registers)
		registersOK &= inst -> clockCycles () == 4 && inst -> mCyclesData ().numberMCycles () == 1 &&
			inst -> mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst -> mCycleDataAt (3)._dataCycle == 2 && inst -> mCycleDataAt (3)._refreshCycle == 2 &&
			inst -> mCyclesData ().memoryReads () == std::vector <size_t> ({ 0 }) &&
			inst -> mCyclesData ().memoryWrites ().empty ();
	ok &= check (registersOK, "register exchanges and EXX use only one opcode fetch", o);

	const EX_SPWithHL exSPHL;
	const EX_SPWithIX exSPIX;
	const EX_SPWithIY exSPIY;
	const Instruction* stack [] = { &exSPHL, &exSPIX, &exSPIY };
	bool stackOK = true;
	// Zilog UM008011, pages 127-129: EX (SP) uses 4,3,4,3,5 plus an IX/IY fetch.
	// The extended second read and final write remain single M-cycles.
	for (size_t n = 0; n < 3; n++)
	{
		const Instruction& inst = *stack [n];
		size_t offset = (n == 0) ? 0 : 4;
		stackOK &= inst.clockCycles () == offset + 19 &&
			inst.mCyclesData ().numberMCycles () == ((n == 0) ? 5 : 6) &&
			inst.mCycleAt (offset).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCycleDataAt (offset + 3)._refreshCycle == offset + 2 &&
			inst.mCycleAt (offset + 4).type () == MCycle::Type::_MEMORYREAD &&
			inst.mCycleDataAt (offset + 6)._dataCycle == offset + 6 &&
			inst.mCycleAt (offset + 7).type () == MCycle::Type::_MEMORYREAD &&
			inst.mCycleAt (offset + 7).numberTCycles () == 4 &&
			inst.mCycleDataAt (offset + 10)._dataCycle == offset + 9 &&
			inst.mCyclePositionAt (offset + 11)._localCycle == 0 &&
			inst.mCycleAt (offset + 11).type () == MCycle::Type::_MEMORYWRITE &&
			inst.mCycleDataAt (offset + 13)._dataCycle == offset + 13 &&
			inst.mCyclePositionAt (offset + 14)._localCycle == 0 &&
			inst.mCycleAt (offset + 14).type () == MCycle::Type::_MEMORYWRITE &&
			inst.mCycleAt (offset + 14).numberTCycles () == 5 &&
			inst.mCycleDataAt (offset + 18)._dataCycle == offset + 16 &&
			inst.mCycleDataAt (offset + 18)._endCycle == offset + 19 &&
			inst.mCycleDataAt (offset + 18)._refreshCycle == absent &&
			inst.mCyclesData ().memoryReads () == ((n == 0) ?
				std::vector <size_t> ({ 0, 1, 2 }) : std::vector <size_t> ({ 0, 1, 2, 3 })) &&
			inst.mCyclesData ().memoryWrites () == ((n == 0) ?
				std::vector <size_t> ({ 3, 4 }) : std::vector <size_t> ({ 4, 5 })) &&
			inst.mCyclesData ().internalCycles ().empty () && inst.mCyclesData ().ioCycles ().empty ();
		if (n > 0)
			stackOK &= inst.mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleDataAt (3)._refreshCycle == 2;
	}
	ok &= check (stackOK, "EX (SP),HL/IX/IY retain two reads and two writes with extended boundaries", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testBlockTransferCompare (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const LDI ldi;
	const LDD ldd;
	const LDIR ldir;
	const LDDR lddr;
	const CPI cpi;
	const CPD cpd;
	const CPIR cpir;
	const CPDR cpdr;
	const Instruction* instructions [] = { &ldi, &ldd, &ldir, &lddr, &cpi, &cpd, &cpir, &cpdr };
	bool transfersOK = true, comparisonsOK = true;
	// Zilog UM008011, pages 130-141: describe one iteration, not an entire repeat loop.
	// Equal 4,4,3,5 durations hide a write for LD and an internal M-cycle for CP.
	for (size_t n = 0; n < 8; n++)
	{
		const Instruction& inst = *instructions [n];
		bool move = n < 4, repeat = n % 4 >= 2;
		bool instructionOK = inst.clockCycles () == 16 && inst.numberMCyclesStructures () == (repeat ? 2 : 1);
		for (size_t alternative = 0; alternative < inst.numberMCyclesStructures (); alternative++)
		{
			const MCyclesData& data = inst.mCyclesData (alternative);
			instructionOK &= data.numberTCycles () == 16 + alternative * 5 &&
				data.numberMCycles () == 4 + alternative &&
				inst.mCycleAt (0, alternative).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleDataAt (3, alternative)._refreshCycle == 2 &&
				inst.mCycleAt (4, alternative).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleDataAt (7, alternative)._refreshCycle == 6 &&
				inst.mCyclePositionAt (8, alternative)._localCycle == 0 &&
				inst.mCycleAt (8, alternative).type () == MCycle::Type::_MEMORYREAD &&
				inst.mCycleDataAt (10, alternative)._dataCycle == 10 &&
				inst.mCycleDataAt (10, alternative)._refreshCycle == absent &&
				inst.mCyclePositionAt (11, alternative)._localCycle == 0 &&
				inst.mCycleAt (11, alternative).type () == (move ? MCycle::Type::_MEMORYWRITE : MCycle::Type::_INTERNAL) &&
				inst.mCycleAt (11, alternative).numberTCycles () == 5 &&
				inst.mCycleDataAt (15, alternative)._dataCycle == (move ? 13 : absent) &&
				inst.mCycleDataAt (15, alternative)._endCycle == 16 &&
				inst.mCycleDataAt (15, alternative)._refreshCycle == absent &&
				data.memoryReads () == std::vector <size_t> ({ 0, 1, 2 }) &&
				data.memoryWrites () == (move ? std::vector <size_t> ({ 3 }) : std::vector <size_t> ()) &&
				data.internalCycles ().size () == (move ? 0 : 1) + alternative &&
				data.ioCycles ().empty () && data.interruptAcknowledges ().empty ();
			// The repeat M-cycle performs no additional transfer or refresh.
			if (alternative == 1)
				instructionOK &= inst.mCyclePositionAt (16, 1)._localCycle == 0 &&
					inst.mCycleAt (16, 1).type () == MCycle::Type::_INTERNAL &&
					inst.mCycleAt (16, 1).numberTCycles () == 5 &&
					inst.mCycleDataAt (20, 1)._dataCycle == absent &&
					inst.mCycleDataAt (20, 1)._refreshCycle == absent &&
					inst.mCycleDataAt (20, 1)._endCycle == 21;
		}
		if (move)
			transfersOK &= instructionOK;
		else
			comparisonsOK &= instructionOK;
	}
	ok &= check (transfersOK, "LDI/LDD/LDIR/LDDR retain one read, one extended write and repeat alternatives", o);
	ok &= check (comparisonsOK, "CPI/CPD/CPIR/CPDR retain the internal comparison and separate repeat cycle", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testSimpleIO (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const IN_A inA;
	const OUT_A outA;
	const Instruction* immediate [] = { &inA, &outA };
	bool immediateOK = true;
	// Zilog UM008011, pages 295 and 306: immediate I/O uses 4,3,4.
	// Positions are nominal and zero-based; prepareIOAccess adds runtime contention.
	for (size_t n = 0; n < 2; n++)
	{
		const Instruction& inst = *immediate [n];
		immediateOK &= inst.clockCycles () == 11 && inst.mCyclesData ().numberMCycles () == 3 &&
			inst.mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCycleDataAt (3)._refreshCycle == 2 &&
			inst.mCycleAt (4).type () == MCycle::Type::_MEMORYREAD &&
			inst.mCycleDataAt (6)._dataCycle == 6 &&
			inst.mCyclePositionAt (7)._localCycle == 0 &&
			inst.mCycleAt (7).type () == ((n == 0) ? MCycle::Type::_IOREAD : MCycle::Type::_IOWRITE) &&
			inst.mCycleAt (7).numberTCycles () == 4 &&
			inst.mCycleDataAt (10)._contentionCycle == 7 && inst.mCycleDataAt (10)._dataCycle == 10 &&
			inst.mCycleDataAt (10)._refreshCycle == absent &&
			inst.mCyclesData ().ioCycles () == std::vector <size_t> ({ 2 }) &&
			inst.mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst.mCyclesData ().memoryWrites ().empty ();
	}
	ok &= check (immediateOK, "immediate IN/OUT preserve the nominal I/O start at 7 and transfer at 10", o);

	const IN_AFromC inAC;
	const IN_BFromC inBC;
	const IN_CFromC inCC;
	const IN_DFromC inDC;
	const IN_EFromC inEC;
	const IN_HFromC inHC;
	const IN_LFromC inLC;
	const IN_FFromC inFC;
	const OUT_AToC outAC;
	const OUT_BToC outBC;
	const OUT_CToC outCC;
	const OUT_DToC outDC;
	const OUT_EToC outEC;
	const OUT_HToC outHC;
	const OUT_LToC outLC;
	const OUT_0ToC out0C;
	const Instruction* registerIO [] = {
		&inAC, &inBC, &inCC, &inDC, &inEC, &inHC, &inLC, &inFC,
		&outAC, &outBC, &outCC, &outDC, &outEC, &outHC, &outLC, &out0C };
	bool registerIOOK = true;
	// Pages 296-297 and 307-308: ED forms use 4,4,4, including flags-only IN and OUT 0.
	// A port transfer is indexed as I/O, not as another memory read or write.
	for (size_t n = 0; n < 16; n++)
	{
		const Instruction& inst = *registerIO [n];
		registerIOOK &= inst.clockCycles () == 12 && inst.mCyclesData ().numberMCycles () == 3 &&
			inst.mCycleAt (0).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCycleDataAt (3)._refreshCycle == 2 &&
			inst.mCycleAt (4).type () == MCycle::Type::_OPCODEFETCH &&
			inst.mCycleDataAt (7)._refreshCycle == 6 &&
			inst.mCyclePositionAt (8)._localCycle == 0 &&
			inst.mCycleAt (8).type () == ((n < 8) ? MCycle::Type::_IOREAD : MCycle::Type::_IOWRITE) &&
			inst.mCycleAt (8).numberTCycles () == 4 &&
			inst.mCycleDataAt (11)._contentionCycle == 8 && inst.mCycleDataAt (11)._dataCycle == 11 &&
			inst.mCycleDataAt (11)._refreshCycle == absent &&
			inst.mCyclesData ().ioCycles () == std::vector <size_t> ({ 2 }) &&
			inst.mCyclesData ().memoryReads () == std::vector <size_t> ({ 0, 1 }) &&
			inst.mCyclesData ().memoryWrites ().empty ();
	}
	ok &= check (registerIOOK, "ED IN/OUT preserve the nominal I/O start at 8 and transfer at 11", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::testBlockIO (std::ostream& o)
{
	bool ok = true;
	const size_t absent = MCycle::_NOCYCLE;
	const INI ini;
	const IND ind;
	const INIR inir;
	const INDR indr;
	const OUTI outi;
	const OUTD outd;
	const OTIR otir;
	const OTDR otdr;
	const Instruction* instructions [] = { &ini, &ind, &inir, &indr, &outi, &outd, &otir, &otdr };
	bool inputsOK = true, outputsOK = true;
	// IN transfers the port byte before the memory write; OUT reads memory first.
	// The second fetch has five T-states, so the first data M-cycle starts at 9.
	for (size_t n = 0; n < 8; n++)
	{
		const Instruction& inst = *instructions [n];
		bool input = n < 4, repeat = n % 4 >= 2;
		size_t ioStart = input ? 9 : 12;
		bool instructionOK = inst.clockCycles () == 16 && inst.numberMCyclesStructures () == (repeat ? 2 : 1);
		for (size_t alternative = 0; alternative < inst.numberMCyclesStructures (); alternative++)
		{
			const MCyclesData& data = inst.mCyclesData (alternative);
			instructionOK &= data.numberTCycles () == 16 + alternative * 5 &&
				data.numberMCycles () == 4 + alternative &&
				inst.mCycleAt (0, alternative).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleDataAt (3, alternative)._refreshCycle == 2 &&
				inst.mCycleAt (4, alternative).type () == MCycle::Type::_OPCODEFETCH &&
				inst.mCycleAt (4, alternative).numberTCycles () == 5 &&
				inst.mCycleDataAt (8, alternative)._dataCycle == 6 &&
				inst.mCycleDataAt (8, alternative)._refreshCycle == 6 &&
				inst.mCyclePositionAt (9, alternative)._localCycle == 0 &&
				inst.mCycleAt (ioStart, alternative).type () == (input ? MCycle::Type::_IOREAD : MCycle::Type::_IOWRITE) &&
				inst.mCycleAt (ioStart, alternative).numberTCycles () == 4 &&
				inst.mCycleDataAt (ioStart + 3, alternative)._contentionCycle == ioStart &&
				inst.mCycleDataAt (ioStart + 3, alternative)._dataCycle == ioStart + 3 &&
				inst.mCycleDataAt (ioStart + 3, alternative)._refreshCycle == absent &&
				data.ioCycles () == (input ? std::vector <size_t> ({ 2 }) : std::vector <size_t> ({ 3 })) &&
				data.memoryReads () == (input ? std::vector <size_t> ({ 0, 1 }) : std::vector <size_t> ({ 0, 1, 2 })) &&
				data.memoryWrites () == (input ? std::vector <size_t> ({ 3 }) : std::vector <size_t> ()) &&
				data.internalCycles ().size () == alternative;
			if (input)
				instructionOK &= inst.mCyclePositionAt (13, alternative)._localCycle == 0 &&
					inst.mCycleAt (13, alternative).type () == MCycle::Type::_MEMORYWRITE &&
					inst.mCycleDataAt (15, alternative)._dataCycle == 15;
			else
				instructionOK &= inst.mCycleAt (9, alternative).type () == MCycle::Type::_MEMORYREAD &&
					inst.mCycleDataAt (11, alternative)._dataCycle == 11 &&
					inst.mCyclePositionAt (12, alternative)._localCycle == 0;
			// Repetition adds time after both transfers, without shifting the port phase.
			if (alternative == 1)
				instructionOK &= inst.mCyclePositionAt (16, 1)._localCycle == 0 &&
					inst.mCycleAt (16, 1).type () == MCycle::Type::_INTERNAL &&
					inst.mCycleAt (16, 1).numberTCycles () == 5 &&
					inst.mCycleDataAt (20, 1)._dataCycle == absent &&
					inst.mCycleDataAt (20, 1)._refreshCycle == absent &&
					inst.mCycleDataAt (20, 1)._endCycle == 21;
		}
		if (input)
			inputsOK &= instructionOK;
		else
			outputsOK &= instructionOK;
	}
	ok &= check (inputsOK, "INI/IND/INIR/INDR preserve port read at 12 before memory write at 15", o);
	ok &= check (outputsOK, "OUTI/OUTD/OTIR/OTDR preserve memory read at 11 before port write at 15", o);
	return (ok);
}

// ---
bool FZ80::MCycleTests::check (bool ok, const char* name, std::ostream& o)
{
	o << "MCycle: " << name << (ok ? " OK" : " FAILED") << std::endl;
	return (ok);
}
