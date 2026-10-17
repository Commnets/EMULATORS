#include "MCycleTests.hpp"
#include <FZ80/MCycle.hpp>
#include <FZ80/LD.hpp>

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
		!existing.hasMCycles () && existing.clockCycles () == 10,
		"legacy construction and existing instruction compatibility", o);

	MCyclesStructure sequence { _Z80_M1 (), _Z80_MR3 (), _Z80_MR3 () };
	const MCycleTestInstruction described (0x21, 3, 10, "LD HL,[$2]", sequence);
	sequence.clear ();
	const MCHEmul::CycleStructure expected { r, r, r, i, r, r, r, r, r, r };
	ok &= check (described.hasMCycles () && described.numberMCyclesStructures () == 1 &&
		described.clockCycles () == 10 && described.cycleStructure () == expected &&
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
bool FZ80::MCycleTests::check (bool ok, const char* name, std::ostream& o)
{
	o << "MCycle: " << name << (ok ? " OK" : " FAILED") << std::endl;
	return (ok);
}
