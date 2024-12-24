/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: Instructions.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The list of instructions used by any F6500 family. \n
 *				 The no documented instructions have also been included. \n
 *				 Only the stable ones. \n
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_INSTRUCTIONS__
#define __F6500_INSTRUCTIONS__

#include <CORE/incs.hpp>
#include <F6500/C6510.hpp>

namespace F6500
{
	/** 
	  *	To simplify the way a new Instruction for this processor is implemented.
	  *	@param _C  : Code.
	  *	@param _M  : MemoryPositions occupied.
	  *	@param _CC : Clock cycles used. 
	  * @param _RCC: Structure of the internal cycles.
	  *				 Maintained empty to declare that they are not defined.
	  * @param _T  : The template to print the instruction.
	  *	@param _I  : Name of the intruction.
	  * @param _J  : Name of the parent class. The last parent class should be InstructionDefined.
	  */
	#define _INST6500_FROM(_C, _M, _CC, _RCC, _T, _I, _J) \
	class _I final : public _J \
	{ \
		public: \
		_I () : _J (_C, _M, _CC, _RCC, _T) { } \
		protected: \
		virtual bool executeImpl (bool& _FINISH) override; \
	};

	// Internal structure of the instructions...
	// In this case the structure is declared only from the perspective of the use of the address and data bus.
	/** For those instructions where all cycles are read cycles. */
	#define _6500R(X) MCHEmul::InstructionDefined::CycleStructure (X, MCHEmul::InstructionDefined::_CYCLEREAD)
	/** For those in which ones where only the first cycles are "read" and the last ones are "write". \
		X will be the total cycles, and Y the first _READ ones. */
	inline static MCHEmul::InstructionDefined::CycleStructure _6500RW (size_t X, size_t Y);
	/** Just for BRK. It is the only instruction with 3 consecutive _WRITE cycles .*/
	#define _6500BRK { \
		MCHEmul::InstructionDefined::_CYCLEREAD, \
		MCHEmul::InstructionDefined::_CYCLEREAD, \
		MCHEmul::InstructionDefined::_CYCLEWRITE, \
		MCHEmul::InstructionDefined::_CYCLEWRITE, \
		MCHEmul::InstructionDefined::_CYCLEWRITE, \
		MCHEmul::InstructionDefined::_CYCLEREAD, \
		MCHEmul::InstructionDefined::_CYCLEREAD }
	/** Just for JSR. */
	#define _6500JSR { \
		MCHEmul::InstructionDefined::_CYCLEREAD, \
		MCHEmul::InstructionDefined::_CYCLEREAD, \
		MCHEmul::InstructionDefined::_CYCLEWRITE, \
		MCHEmul::InstructionDefined::_CYCLEWRITE, \
		MCHEmul::InstructionDefined::_CYCLEWRITE, \
		MCHEmul::InstructionDefined::_CYCLEREAD }

	/** Most of 6500 instruction inherits from this class
		because it includes diffrent access for any addrss mode. \n
		Always little-endian format and no more than 2 bytes for and address are values taken. \n
		All 6500 instructions access to the data - bus in every cycle, even when they have to run some internal operation. \n
		Most of the access cycles are to read, and just a couple of operations access the bus to write. \n
		When this happen the write accesses are the last cycles of the total in almost any case, and the rest will be for "reading". \n
		The exception is in the instructions related with jumping. \n
		So as the default the type of cycle will be _CYCLEREAD for the first of them, and _CYCLEWRITE for the rest. \n
		@see: https://datasheet4u.com/datasheet-pdf/Synertek/SY6500/pdf.php?id=1306768 (last pages). */
	class Instruction : public MCHEmul::InstructionDefined
	{
		public:
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: MCHEmul::InstructionDefined (c, mp, cc, cS, t, false)
							{ /** Nothing else. */ }
	
		protected:
		/** To access the different registers. */
		const MCHEmul::Register& registerA () const
							{ return (cpu () -> internalRegister (C6510::_ACCUMULATOR)); }
		MCHEmul::Register& registerA ()
							{ return (cpu () -> internalRegister (C6510::_ACCUMULATOR)); }
		const MCHEmul::Register& registerX () const
							{ return (cpu () -> internalRegister (C6510::_XREGISTER)); }
		MCHEmul::Register& registerX ()
							{ return (cpu () -> internalRegister (C6510::_XREGISTER)); }
		const MCHEmul::Register& registerY () const
							{ return (cpu () -> internalRegister (C6510::_YREGISTER)); }
		MCHEmul::Register& registerY ()
							{ return (cpu () -> internalRegister (C6510::_YREGISTER)); }
		const MCHEmul::StatusRegister& statusRegister () const
							{ return (cpu () -> statusRegister ()); }
		MCHEmul::StatusRegister& statusRegister ()
							{ return (cpu () -> statusRegister ()); }

		// To interpret the parameters of the instruction as an address 
		/** 2 parameters representing an address. */
		inline const MCHEmul::Address& address_absolute () const;
		/** 1 parameter representing an address in the page 0. */
		inline const MCHEmul::Address& address_zeroPage () const;
		/** 2 parameters representing a base address. 
			The final one is got adding the X register to the base. */
		inline const MCHEmul::Address& address_absoluteX () const;
		/** 2 parameters representing a base address. 
			The final one is got adding the Y register to the base. */
		inline const MCHEmul::Address& address_absoluteY () const;
		/** 1 parameter representing a base address in page 0. 
			The final one is got adding the X register to the base. */
		inline const MCHEmul::Address& address_zeroPageX () const;
		/** 1 parameter representing a base address in page 0. 
			The final one is got adding the Y register to the base. */
		inline const MCHEmul::Address& address_zeroPageY () const;
		/** 1 parameter representing a base address in page 0. 
			The final one is got from the position in page 0 result of adding the X register to the base. */
		inline const MCHEmul::Address& address_indirectZeroPageX () const;
		/** 1 parameter representing a base address in page 0. 
			The final one is got from the position in page 0 result of adding the X register to position found in the base. */
		inline const MCHEmul::Address& address_indirectZeroPageY () const;
		/** 2 parameters representint a base addreess. 
			The final one is got at the position of that first base address. */
		inline const MCHEmul::Address& address_indirect () const;

		// To interpret the parameters of the instruction as a a value
		/** Just 1 parameter with the value. */
		inline const MCHEmul::UByte& value_inmediate () const;
		const MCHEmul::UByte& value_absolute () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_absolute ()) }))[0]); }
		const MCHEmul::UByte& value_zeroPage () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_zeroPage ()) }))[0]); }
		const MCHEmul::UByte& value_absoluteX () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_absoluteX ()) }))[0]); }
		const MCHEmul::UByte& value_absoluteY () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_absoluteY ()) }))[0]); }
		/** Just 1 parameter that can be negative value. It us used in jumps. */
		inline const MCHEmul::UByte& value_relative () const;
		const MCHEmul::UByte& value_zeroPageX () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_zeroPageX ()) }))[0]); }
		const MCHEmul::UByte& value_zeroPageY () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_zeroPageY ()) }))[0]); }
		const MCHEmul::UByte& value_indirectZeroPageX () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_indirectZeroPageX ()) }))[0]); }
		const MCHEmul::UByte& value_indirectZeroPageY () const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (address_indirectZeroPageY ()) }))[0]); }
	};

	// ---
	inline static MCHEmul::InstructionDefined::CycleStructure _6500RW (size_t X, size_t Y)
	{ 
		MCHEmul::InstructionDefined::CycleStructure write =
			MCHEmul::InstructionDefined::CycleStructure (X - Y, MCHEmul::InstructionDefined::_CYCLEWRITE);
		MCHEmul::InstructionDefined::CycleStructure result = 
			MCHEmul::InstructionDefined::CycleStructure (Y, MCHEmul::InstructionDefined::_CYCLEREAD);
		result.insert (result.end (), write.begin (), write.end ());
		
		return (result);
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_absolute () const
	{
		assert (parameters ().size () == 3);

		return (_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ parameters ()[1], parameters ()[2] }, false));
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_zeroPage () const
	{
		assert (parameters ().size () == 2);

		return _lastExecutionData._INOUTAddress = (MCHEmul::Address ({ parameters ()[1] }));
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_absoluteX () const
	{
		assert (parameters ().size () == 3);

		const MCHEmul::Register& x = registerX ();

		MCHEmul::Address iA ({ parameters ()[1], parameters ()[2] }, false);
		MCHEmul::Address fA = iA + (size_t) (x [0].value ());
		if (iA [0] != fA [0]) 
			_additionalCycles = 1; // Page jump in the address so one cycle more
		
		return (_lastExecutionData._INOUTAddress = fA);
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_absoluteY () const
	{
		assert (parameters ().size () == 3);

		const MCHEmul::Register& y = registerY ();

		MCHEmul::Address iA ({ parameters ()[1], parameters ()[2] }, false);
		MCHEmul::Address fA = iA + (size_t) (y [0].value ());
		if (iA [0] != fA [0]) 
			_additionalCycles = 1; // Page jump in the address so one cycle more
		
		return (_lastExecutionData._INOUTAddress = fA);
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_zeroPageX () const
	{
		assert (parameters ().size () == 2);

		// Take care because when adding the value of the register X the final address can not be in the page 0
		// and an adjustment will be required...
		MCHEmul::Address pg0A = 
			MCHEmul::Address ({ parameters ()[1] }) + (size_t) (registerX ()[0].value ());
		if (pg0A.value () > (unsigned int) 0x00ff) pg0A -= (size_t) 0x0100; // To avoid go out of the limits...
		return (_lastExecutionData._INOUTAddress = pg0A);
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_zeroPageY () const
	{
		assert (parameters ().size () == 2);

		return (_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ parameters ()[1] }) + 
				(size_t) (registerY ()[0].value ())); 
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_indirectZeroPageX () const
	{
		assert (parameters ().size () == 2);

		// Pre - indirect zero page addressing...
		// Take care because when adding the value of the register X the final address can not be in the page 0
		// and an adjustment will be required...
		MCHEmul::Address pg0A = 
			MCHEmul::Address ({ parameters ()[1] }) + (size_t) (registerX ()[0].value ());
		if (pg0A.value () > (unsigned int) 0x00ff) pg0A -= (size_t) 0x0100; // To avoid go out of the limits...
		return (_lastExecutionData._INOUTAddress = 
			MCHEmul::Address (memory () -> values (pg0A, 2), false)); 
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_indirectZeroPageY () const
	{
		assert (parameters ().size () == 2);

		const MCHEmul::Register& y = registerY ();

		// Post - indirect zero page addressing...
		MCHEmul::Address iA (memory () -> values (MCHEmul::Address ({ parameters ()[1] }), 2), false);
		MCHEmul::Address fA = iA + (size_t) (y [0].value ());
		if (iA [0] != fA [0]) 
			_additionalCycles = 1; // Page jump in the address so one cycle more

		return (_lastExecutionData._INOUTAddress = fA);
	}

	// ---
	inline const MCHEmul::Address& Instruction::address_indirect () const
	{
		assert (parameters ().size () == 3);

		return (_lastExecutionData._INOUTAddress = 
			MCHEmul::Address (memory () -> values 
				(MCHEmul::Address ({ parameters ()[1], parameters ()[2] }, false), 2), false));
	}

	// ---
	inline const MCHEmul::UByte& Instruction::value_inmediate () const
	{
		assert (parameters ().size () == 2);

		return (parameters ()[1]);
	}

	// ---
	inline const MCHEmul::UByte& Instruction::value_relative () const
	{
		assert (parameters ().size () == 2);

		return (parameters ()[1]); // The number can be interpreted as a negative number (used in jumps)...
	}

	// ADC
	/** ADC_General: To aggregate common steps in every ADC instruction. */
	class ADC_General : public Instruction
	{
		public:
		ADC_General (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS,
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool ADC_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();
	
		// Calculate the addition...
		unsigned char ft = st.bitStatus (C6500::_DECIMALFLAG) 
			? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
		MCHEmul::UInt r = MCHEmul::UInt (a.values ()[0], ft).
			add (MCHEmul::UInt (u, ft), st.bitStatus (C6500::_CARRYFLAG));
		a.set (r.bytes ()); // The carry register is taken into account in the addition...

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_OVERFLOWFLAG, r.overflow ());
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ());

		return (true);
	}

	_INST6500_FROM (0x69, 2, 2, _6500R(2),		"ADC#[#1]",				ADC_Inmediate, ADC_General);
	_INST6500_FROM (0x6d, 3, 4, _6500R(4),		"ADC[$2]",				ADC_Absolute, ADC_General);
	_INST6500_FROM (0x65, 2, 3, _6500R(3),		"ADC[$1]",				ADC_ZeroPage, ADC_General);
	_INST6500_FROM (0x61, 2, 6, _6500R(6),		"ADC([$1],X)",			ADC_ZeroPageIndirectX, ADC_General);
	_INST6500_FROM (0x71, 2, 5, _6500R(5),		"ADC([$1]),Y",			ADC_ZeroPageIndirectY, ADC_General);
	_INST6500_FROM (0x75, 2, 4, _6500R(4),		"ADC[$1],X",			ADC_ZeroPageX, ADC_General);
	_INST6500_FROM (0x7d, 3, 4, _6500R(4),		"ADC[$2],X",			ADC_AbsoluteX, ADC_General);
	_INST6500_FROM (0x79, 3, 4, _6500R(4),		"ADC[$2],Y",			ADC_AbsoluteY, ADC_General);

	// Non documented
	// ALR: AND + LSR
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** ALR_General: To aggregate common steps in every ALR instruction. */
	class ALR_General : public Instruction
	{
		public:
		ALR_General (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool ALR_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();
	
		// Makes the operation...
		MCHEmul::UByte r = (a.values ()[0] & u); // AND...
		bool c = r.shiftRightC (false, 1); // ...LSR
		a.set ({ r });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r [7]);
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x4b, 2, 2, _6500R(2),		"ALR#[#1]",				ALR_Inmediate, ALR_General);

	// Non documented
	// ANC: AND + Carry flag
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** ANC_General: To aggregate common steps in every ANC instruction. */
	class ANC_General : public Instruction
	{
		public:
		ANC_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (MCHEmul::UByte u);
	};

	// ---
	inline bool ANC_General::executeWith (MCHEmul::UByte u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();
	
		// Set the value...
		MCHEmul::UByte r = a.values ()[0] /** 1 byte long. */ & u; // AND...
		a.set ({ r });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r [7]);
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r [7] /** Carry always like sign. */);

		return (true);
	}

	_INST6500_FROM (0x0b /** 2b */, 2, 2, _6500R(2),	"ANC#[#1]",		ANC_Inmediate, ANC_General);

	// AND
	/** AND_General: To aggregate common steps in every AND instruction. */
	class AND_General : public Instruction
	{
		public:
		AND_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS,  
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool AND_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();
	
		// Set the value...
		// The register is always 1 byte long...
		MCHEmul::UByte r = a.values ()[0] & u;
		a.set ({ r });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r [7]);
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UByte::_0);

		return (true);
	}

	_INST6500_FROM (0x29, 2, 2, _6500R(2),		"AND#[#1]",				AND_Inmediate, AND_General);
	_INST6500_FROM (0x2d, 3, 4, _6500R(4),		"AND[$2]",				AND_Absolute, AND_General);
	_INST6500_FROM (0x25, 2, 3, _6500R(3),		"AND[$1]",				AND_ZeroPage, AND_General);
	_INST6500_FROM (0x21, 2, 6, _6500R(6),		"AND([$1],X)",			AND_ZeroPageIndirectX, AND_General);
	_INST6500_FROM (0x31, 2, 5, _6500R(5),		"AND([$1]),Y",			AND_ZeroPageIndirectY, AND_General);
	_INST6500_FROM (0x35, 2, 4, _6500R(4),		"AND[$1],X",			AND_ZeroPageX, AND_General);
	_INST6500_FROM (0x3d, 3, 4, _6500R(4),		"AND[$2],X",			AND_AbsoluteX, AND_General);
	_INST6500_FROM (0x39, 3, 4, _6500R(4),		"AND[$2],Y",			AND_AbsoluteY, AND_General);

	// Non documented
	// ARR: AND + ROR
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** ARR_General: To aggregate common steps in every ARR instruction. */
	class ARR_General : public Instruction
	{
		public:
		ARR_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool ARR_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();
	
		MCHEmul::UByte r = (a.values ()[0] & u); // AND...
		bool o = r [7] ^ r [6]; // The overflow flag is calculated different...
		r.rotateRightC (st.bitStatus (C6500::_CARRYFLAG), 1); // ...ROR
		a.set ({ r });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r [7]);
		st.setBitStatus (C6500::_OVERFLOWFLAG, o);
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r [6] /** after. */);

		return (true);
	}

	_INST6500_FROM (0x6b, 2, 2, _6500R(2),		"ARR#[#1]",				ARR_Inmediate, ARR_General);

	// ASL
	/** ASL_General: To aggregate common steps in every ASL instruction. */
	class ASL_General : public Instruction
	{
		public:
		ASL_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool ASL_General::executeOn (const MCHEmul::Address& a)
	{
		// Read the value, makes the operation and set it back!
		MCHEmul::UByte v = memory () -> value (a); // 1 byte long always
		bool c = v.shiftLeftC (false /** 0 is put into */, 1); // Keeps the status of the last bit to actualize later the carry flag
		memory () -> set (a, { v });
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, v [7]);
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x0e, 3, 6, _6500RW(6,4),	"ASL[$2]",				ASL_Absolute, ASL_General);
	_INST6500_FROM (0x06, 2, 5, _6500RW(5,3),	"ASL[$1]",				ASL_ZeroPage, ASL_General);
	_INST6500_FROM (0x0a, 1, 2, _6500R(2),		"ASL",					ASL_Accumulator, ASL_General);
	_INST6500_FROM (0x16, 2, 6, _6500RW(6,4),	"ASL[$1],X",			ASL_ZeroPageX, ASL_General);
	_INST6500_FROM (0x1e, 3, 7, _6500RW(7,5),	"ASL[$2],X",			ASL_AbsoluteX, ASL_General);

	/** BXX_General: To aggregate common steps in every Branch instruction. */
	class BXX_General : public Instruction
	{
		public:
		BXX_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline void executeBranch ();
	};

	// ---
	inline void BXX_General::executeBranch ()
	{
		// When the branch is executed an additional cycle is taken...
		_additionalCycles = 1;

		int jR = MCHEmul::UInt ({ value_relative () }).asInt (); // The value can be negative meaning back jump!

		if (jR == 0)
			return; // No need to continue...

		MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
		unsigned int oPC = pc.internalRepresentation (); // To check whether it crosees the page!
		if (jR > 0) pc.increment ((size_t) jR);
		else pc.decrement ((size_t) -jR); // Parameter to "decrement" always positive...

		// Does the program counter crosses the page?
		if ((oPC >> 8) != (pc.internalRepresentation () >> 8))
			_additionalCycles++;
	}

	// BCC 
	_INST6500_FROM (0x90, 2, 2, _6500R(2),		"BCC[&1]",				BCC, BXX_General);

	// BCS 
	_INST6500_FROM (0xb0, 2, 2, _6500R(2),		"BCS[&1]",				BCS, BXX_General);

	// BEQ
	_INST6500_FROM (0xf0, 2, 2, _6500R(2),		"BEQ[&1]",				BEQ, BXX_General);

	// BIT
	_INST6500_FROM (0x2c, 3, 4, _6500R(4),		"BIT[$2]",				BIT_Absolute, Instruction);
	_INST6500_FROM (0x24, 2, 3, _6500R(3),		"BIT[$1]",				BIT_ZeroPage, Instruction);

	// BMI
	_INST6500_FROM (0x30, 2, 2, _6500R(2),		"BMI[&1]",				BMI, BXX_General);

	// BNE
	_INST6500_FROM (0xd0, 2, 2, _6500R(2),		"BNE[&1]",				BNE, BXX_General);

	// BPL
	_INST6500_FROM (0x10, 2, 2, _6500R(2),		"BPL[&1]",				BPL, BXX_General);

	// BRK
	_INST6500_FROM (0x00, 1, 7, _6500BRK,		"BRK",					BRK, Instruction);

	// BVC
	_INST6500_FROM (0x50, 2, 2, _6500R(2),		"BVC[&1]",				BVC, BXX_General);

	// BVS
	_INST6500_FROM (0x70, 2, 2, _6500R(2),		"BVS[&1]",				BVS, BXX_General);

	// CLC
	_INST6500_FROM (0x18, 1, 2, _6500R(2),		"CLC",					CLC, Instruction);

	// CLD
	_INST6500_FROM (0xd8, 1, 2, _6500R(2),		"CLD",					CLD, Instruction);

	// CLI
	_INST6500_FROM (0x58, 1, 2, _6500R(2),		"CLI",					CLI, Instruction);

	// CLV
	_INST6500_FROM (0xb8, 1, 2, _6500R(2),		"CLV",					CLV, Instruction);

	// CMP
	/** CMP_General: To aggregate common steps in every CMP instruction. */
	class CMP_General : public Instruction
	{
		public:
		CMP_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool CMP_General::executeWith (const MCHEmul::UByte& u)
	{
		// To compare is like to substract...
		MCHEmul::UInt r = 
			MCHEmul::UInt (registerA ().values ()[0]) - 
			MCHEmul::UInt (u);  // Never longer that 1 byte, but the result could be negative...

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ()); // When the result is positive (a >= u)

		return (true);
	}

	_INST6500_FROM (0xc9, 2, 2, _6500R(2),		"CMP#[#1]",				CMP_Inmediate, CMP_General);
	_INST6500_FROM (0xcd, 3, 4, _6500R(4),		"CMP[$2]",				CMP_Absolute, CMP_General);
	_INST6500_FROM (0xc5, 2, 3, _6500R(3),		"CMP[$1]",				CMP_ZeroPage, CMP_General);
	_INST6500_FROM (0xc1, 2, 6, _6500R(6),		"CMP([$1],X)",			CMP_ZeroPageIndirectX, CMP_General);
	_INST6500_FROM (0xd1, 2, 5, _6500R(5),		"CMP([$1]),Y",			CMP_ZeroPageIndirectY, CMP_General);
	_INST6500_FROM (0xd5, 2, 4, _6500R(4),		"CMP[$1],X",			CMP_ZeroPageX, CMP_General);
	_INST6500_FROM (0xdd, 3, 4, _6500R(4),		"CMP[$2],X",			CMP_AbsoluteX, CMP_General);
	_INST6500_FROM (0xd9, 3, 4, _6500R(4),		"CMP[$2],Y",			CMP_AbsoluteY, CMP_General);

	// CPX
	/** CPX_General: To aggregate common steps in every CPX instruction. */
	class CPX_General : public Instruction
	{
		public:
		CPX_General (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool CPX_General::executeWith (const MCHEmul::UByte& u)
	{
		// To compare is like to substract...
		MCHEmul::UInt r = 
			MCHEmul::UInt (registerX ().values ()[0]) - 
			MCHEmul::UInt (u);  // Never longer that 1 byte, but the result could be negative...

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_ZEROFLAG, r [0] == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ()); // When the result is positive (a >= u)

		return (true);
	}

	_INST6500_FROM (0xe0, 2, 2, _6500R(2),		"CPX#[#1]",				CPX_Inmediate, CPX_General);
	_INST6500_FROM (0xec, 3, 4, _6500R(4),		"CPX[$2]",				CPX_Absolute, CPX_General);
	_INST6500_FROM (0xe4, 2, 3, _6500R(3),		"CPX[$1]",				CPX_ZeroPage, CPX_General);

	// CPY
	/** CPY_General: To aggregate common steps in every CPY instruction. */
	class CPY_General : public Instruction
	{
		public:
		CPY_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool CPY_General::executeWith (const MCHEmul::UByte& u)
	{
		// To compare is like to substract...
		MCHEmul::UInt r = 
			MCHEmul::UInt (registerY ().values ()[0]) -
			MCHEmul::UInt (u);  // Never longer that 1 byte, but the result could be negative...

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_ZEROFLAG, r [0] == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ()); // When the result is positive (a >= u)

		return (true);
	}

	_INST6500_FROM (0xc0, 2, 2, _6500R(2),		"CPY#[#1]",				CPY_Inmediate, CPY_General);
	_INST6500_FROM (0xcc, 3, 4, _6500R(4),		"CPY[$2]",				CPY_Absolute, CPY_General);
	_INST6500_FROM (0xc4, 2, 3, _6500R(3),		"CPY[$1]",				CPY_ZeroPage, CPY_General);

	// Non documented
	// DCP: DEC + CMP
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** DCP_General: To aggregate common steps in every DCP instruction. */
	class DCP_General : public Instruction
	{
		public:
		DCP_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool DCP_General::executeOn (const MCHEmul::Address& a)
	{
		// The memory location is reduced by 1...
		MCHEmul::UInt v = MCHEmul::UInt ((memory () -> value (a))) /** 1 byte long always. */ - MCHEmul::UInt::_1; // DEC...
		// A carry could be generated, but it will be ignored...
		memory () -> set (a, v.bytes ()[0]); // ...and stored back
		_lastExecutionData._INOUTData = v.bytes ();

		// To compare is like to substract...
		MCHEmul::UInt r = 
			MCHEmul::UInt (registerA ().values ()[0]) - v;  //...CMP

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ()); // When the result is positive (a >= u)

		return (true);
	}

	_INST6500_FROM (0xcf, 3, 6, _6500RW(6,4),	"DCP[$2]",				DCP_Absolute, DCP_General);
	_INST6500_FROM (0xc7, 2, 5, _6500RW(5,3),	"DCP[$1]",				DCP_ZeroPage, DCP_General);
	_INST6500_FROM (0xc3, 2, 8, _6500RW(8,6),	"DCP([$1],X)",			DCP_ZeroPageIndirectX, DCP_General);
	_INST6500_FROM (0xd3, 2, 8, _6500RW(8,6),	"DCP([$1]),Y",			DCP_ZeroPageIndirectY, DCP_General);
	_INST6500_FROM (0xd7, 2, 6, _6500RW(6,4),	"DCP[$1],X",			DCP_ZeroPageX, DCP_General);
	_INST6500_FROM (0xdf, 3, 7, _6500RW(7,5),	"DCP[$2],X",			DCP_AbsoluteX, DCP_General);
	_INST6500_FROM (0xdb, 3, 7, _6500RW(7,5),	"DCP[$2],Y",			DCP_AbsoluteY, DCP_General);

	// DEC
	/** DEC_General: To aggregate common steps in every DEC instruction. */
	class DEC_General : public Instruction
	{
		public:
		DEC_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool DEC_General::executeOn (const MCHEmul::Address& a)
	{
		// Read the value, makes the operation and sets it back...
		MCHEmul::UInt v = MCHEmul::UInt ((memory () -> value (a))) - MCHEmul::UInt::_1;
		// A carry could be generated, but it will be ignored...
		memory () -> set (a, v.bytes ()[0]); // 1 byte long always
		_lastExecutionData._INOUTData = v.bytes ();

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, v.negative ());
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UInt::_0);

		return (true);
	}

	_INST6500_FROM (0xce, 3, 6, _6500RW(6,4),	"DEC[$2]",				DEC_Absolute, DEC_General);
	_INST6500_FROM (0xc6, 2, 5, _6500RW(5,3),	"DEC[$1]",				DEC_ZeroPage, DEC_General);
	_INST6500_FROM (0xd6, 2, 6, _6500RW(6,4),	"DEC[$1],X",			DEC_ZeroPageX, DEC_General);
	_INST6500_FROM (0xde, 3, 7, _6500RW(7,5),	"DEC[$2],X",			DEC_AbsoluteX, DEC_General);

	// DEX
	_INST6500_FROM (0xca, 1, 2, _6500R(2),		"DEX",					DEX, Instruction);

	// DEY
	_INST6500_FROM (0x88, 1, 2, _6500R(2),		"DEY",					DEY, Instruction);

	// EOR
	/** EOR_General: To aggregate common steps in every EOR instruction. */
	class EOR_General : public Instruction
	{
		public:
		EOR_General (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool EOR_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();
	
		// Read the value, makes the operation and sets it back...
		MCHEmul::UByte r = a.values ()[0] ^ u;
		a.set ({ r });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r [7]);
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UByte::_0);

		return (true);
	}

	_INST6500_FROM (0x49, 2, 2, _6500R(2),		"EOR#[#1]",				EOR_Inmediate, EOR_General);
	_INST6500_FROM (0x4d, 3, 4, _6500R(4),		"EOR[$2]",				EOR_Absolute, EOR_General);
	_INST6500_FROM (0x45, 2, 3, _6500R(3),		"EOR[$1]",				EOR_ZeroPage, EOR_General);
	_INST6500_FROM (0x41, 2, 6, _6500R(6),		"EOR([$1],X)",			EOR_ZeroPageIndirectX, EOR_General);
	_INST6500_FROM (0x51, 2, 5, _6500R(5),		"EOR([$1]),Y",			EOR_ZeroPageIndirectY, EOR_General);
	_INST6500_FROM (0x55, 2, 4, _6500R(4),		"EOR[$1],X",			EOR_ZeroPageX, EOR_General);
	_INST6500_FROM (0x5d, 3, 4, _6500R(4),		"EOR[$2],X",			EOR_AbsoluteX, EOR_General);
	_INST6500_FROM (0x59, 3, 4, _6500R(4),		"EOR[$2],Y",			EOR_AbsoluteY, EOR_General);

	// INC
	/** INC_General: To aggregate common steps in every INC instruction. */
	class INC_General : public Instruction
	{
		public:
		INC_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool INC_General::executeOn (const MCHEmul::Address& a)
	{
		// Read the value, makes the operation and sets it back...
		MCHEmul::UInt v = MCHEmul::UInt ((memory () -> value (a))) + MCHEmul::UInt::_1;
		// A carry could be generated, but it will be ignored...
		memory () -> set (a, v.bytes ()[0]); // 1 byte long always
		_lastExecutionData._INOUTData = v.bytes ();

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, v.negative ());
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UInt::_0);

		return (true);
	}

	_INST6500_FROM (0xee, 3, 6, _6500RW(6,4),	"INC[$2]",				INC_Absolute, INC_General);
	_INST6500_FROM (0xe6, 2, 5, _6500RW(5,3),	"INC[$1]",				INC_ZeroPage, INC_General);
	_INST6500_FROM (0xf6, 2, 6, _6500RW(6,4),	"INC[$1],X",			INC_ZeroPageX, INC_General);
	_INST6500_FROM (0xfe, 3, 7, _6500RW(7,5),	"INC[$2],X",			INC_AbsoluteX, INC_General);

	// INX
	_INST6500_FROM (0xe8, 1, 2, _6500R(2),		"INX",					INX, Instruction);

	// INY
	_INST6500_FROM (0xc8, 1, 2, _6500R(2),		"INY",					INY, Instruction);

	// Non documented
	// ISC: INC + SBC
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** ISC_General: To aggregate common steps in every ISC instruction. */
	class ISC_General : public Instruction
	{
		public:
		ISC_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool ISC_General::executeOn (const MCHEmul::Address& a)
	{
		MCHEmul::Register& ac = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();

		// Read the value, makes the operation and sets it back...
		MCHEmul::UInt v = MCHEmul::UInt ((memory () -> value (a))) /** 1 bytelong. */ + MCHEmul::UInt::_1; // INC...
		// A carry could be generated, but it will be ignored...
		memory () -> set (a, v.bytes ()[0]);
		_lastExecutionData._INOUTData = v.bytes ();

		// ...and then the substract...
		unsigned char ft = st.bitStatus (C6500::_DECIMALFLAG) 
			? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
		MCHEmul::UInt r = MCHEmul::UInt (ac.values ()[0], ft).substract 
			(MCHEmul::UInt (v.bytes (), ft), st.bitStatus (C6500::_CARRYFLAG)); // ...SBC
		ac.set (r.bytes ());

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_OVERFLOWFLAG, r.overflow ());
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ());

		return (true);
	}

	_INST6500_FROM (0xef, 3, 6, _6500RW(6,4),	"ISC[$2]",				ISC_Absolute, ISC_General);
	_INST6500_FROM (0xe7, 2, 5, _6500RW(5,3),	"ISC[$1]",				ISC_ZeroPage, ISC_General);
	_INST6500_FROM (0xe3, 2, 8, _6500RW(8,6),	"ISC([$1],X)",			ISC_ZeroPageIndirectX, ISC_General);
	_INST6500_FROM (0xf3, 2, 8, _6500RW(8,6),	"ISC([$1]),Y",			ISC_ZeroPageIndirectY, ISC_General);
	_INST6500_FROM (0xf7, 2, 6, _6500RW(6,4),	"ISC[$1],X",			ISC_ZeroPageX, ISC_General);
	_INST6500_FROM (0xff, 3, 7, _6500RW(7,5),	"ISC[$2],X",			ISC_AbsoluteX, ISC_General);
	_INST6500_FROM (0xfb, 3, 7, _6500RW(7,5),	"ISC[$2],Y",			ISC_AbsoluteY, ISC_General);

	// Non documented
	// JAM
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	// Restart the cpu...
	_INST6500_FROM (0x02 /** 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 0x92, 0xb2, 0xd2, 0xf2 */, 1, 1, { }, "JAM", JAM, Instruction);

	// JMP
	_INST6500_FROM (0x4c, 3, 3, _6500R(3),		"JMP[%2]",				JMP_Absolute, Instruction);
	/** 6502 had a bug when jumping indirect. \n
		That bug happened when the low byte of the address where to find the final jumping address was 0xFF. \n
		In that case the high bytes was looked at the adress 0x00 of the same page (high bytes), 
		instead of the one at the next page. \n
		That bug was sorted out at the 65C02 version, that the one implemented here. */
	_INST6500_FROM (0x6c, 3, 5, _6500R(5),		"JMP([%2])",			JMP_Indirect, Instruction);

	// JSR
	_INST6500_FROM (0x20, 3, 6, _6500JSR,		"JSR[%2]",				JSR_Absolute, Instruction);

	// LDA
	/** LDA_General: To aggregate common steps in every LDA instruction. */
	class LDA_General : public Instruction
	{
		public:
		LDA_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool LDA_General::executeWith (const MCHEmul::UByte& u)
	{
		// Set the value...
		registerA ().set ({ u });

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, u [7]);
		st.setBitStatus (C6500::_ZEROFLAG, u == MCHEmul::UByte::_0);

		return (true);
	}

	_INST6500_FROM (0xa9, 2, 2, _6500R(2),		"LDA#[#1]",				LDA_Inmediate, LDA_General);
	_INST6500_FROM (0xad, 3, 4, _6500R(4),		"LDA[$2]",				LDA_Absolute, LDA_General);
	_INST6500_FROM (0xa5, 2, 3, _6500R(3),		"LDA[$1]",				LDA_ZeroPage, LDA_General);
	_INST6500_FROM (0xa1, 2, 6, _6500R(6),		"LDA([$1],X)",			LDA_ZeroPageIndirectX, LDA_General);
	_INST6500_FROM (0xb1, 2, 5, _6500R(5),		"LDA([$1]),Y",			LDA_ZeroPageIndirectY, LDA_General);
	_INST6500_FROM (0xb5, 2, 4, _6500R(4),		"LDA[$1],X",			LDA_ZeroPageX, LDA_General);
	_INST6500_FROM (0xbd, 3, 4, _6500R(4),		"LDA[$2],X",			LDA_AbsoluteX, LDA_General);
	_INST6500_FROM (0xb9, 3, 4, _6500R(4),		"LDA[$2],Y",			LDA_AbsoluteY, LDA_General);

	// Non documented
	// LAX: LDA + TAX
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** LAX_General: To aggregate common steps in every LAX instruction. */
	class LAX_General : public Instruction
	{
		public:
		LAX_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool LAX_General::executeOn (const MCHEmul::Address& a)
	{
		// The memory location is copied into both _ACCUMULATOR & _XREGISTER
		MCHEmul::UByte v = memory () -> value (a); // 1 byte long always...
		registerA ().set ({ v }); 
		registerX ().set ({ v });
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, v.bit (7));
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);

		return (true);
	}

	_INST6500_FROM (0xaf, 3, 6, _6500R(6),		"LAX[$2]",				LAX_Absolute, LAX_General);
	_INST6500_FROM (0xa7, 2, 5, _6500R(5),		"LAX[$1]",				LAX_ZeroPage, LAX_General);
	_INST6500_FROM (0xa3, 2, 8, _6500R(8),		"LAX([$1],X)",			LAX_ZeroPageIndirectX, LAX_General);
	_INST6500_FROM (0xb3, 2, 8, _6500R(8),		"LAX([$1]),Y",			LAX_ZeroPageIndirectY, LAX_General);
	_INST6500_FROM (0xb7, 2, 6, _6500R(6),		"LAX[$1],Y",			LAX_ZeroPageY, LAX_General);
	_INST6500_FROM (0xbf, 3, 7, _6500R(7),		"LAX[$2],Y",			LAX_AbsoluteY, LAX_General);

	// LDX
	/** LDX_General: To aggregate common steps in every LDX instruction. */
	class LDX_General : public Instruction
	{
		public:
		LDX_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool LDX_General::executeWith (const MCHEmul::UByte& u)
	{
		// Set the value...
		registerX ().set ({ u });

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, u [7]);
		st.setBitStatus (C6500::_ZEROFLAG, u == MCHEmul::UByte::_0);

		return (true);
	}

	_INST6500_FROM (0xa2, 2, 2, _6500R(2),		"LDX#[#1]",				LDX_Inmediate, LDX_General);
	_INST6500_FROM (0xae, 3, 4, _6500R(4),		"LDX[$2]",				LDX_Absolute, LDX_General);
	_INST6500_FROM (0xa6, 2, 3, _6500R(3),		"LDX[$1]",				LDX_ZeroPage, LDX_General);
	_INST6500_FROM (0xbe, 3, 4, _6500R(4),		"LDX[$2],Y",			LDX_AbsoluteY, LDX_General);
	_INST6500_FROM (0xb6, 2, 4, _6500R(4),		"LDX[$1],Y",			LDX_ZeroPageY, LDX_General);

	// LDY
	/** LDY_General: To aggregate common steps in every LDY instruction. */
	class LDY_General : public Instruction
	{
		public:
		LDY_General (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool LDY_General::executeWith (const MCHEmul::UByte& u)
	{
		// Set the value...
		registerY ().set ({ u });

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, u [7]);
		st.setBitStatus (C6500::_ZEROFLAG, u == MCHEmul::UByte::_0);

		return (true);
	}

	_INST6500_FROM (0xa0, 2, 2, _6500R(2),		"LDY#[#1]",				LDY_Inmediate, LDY_General);
	_INST6500_FROM (0xac, 3, 4, _6500R(4),		"LDY[$2]",				LDY_Absolute, LDY_General);
	_INST6500_FROM (0xa4, 2, 3, _6500R(3),		"LDY[$1]",				LDY_ZeroPage, LDY_General);
	_INST6500_FROM (0xb4, 2, 4, _6500R(4),		"LDY[$1],X",			LDY_ZeroPageX, LDY_General);
	_INST6500_FROM (0xbc, 3, 4, _6500R(4),		"LDY[$2],X",			LDY_AbsoluteX, LDY_General);

	// LSR
	/** LSR_General: To aggregate common steps in every LSR instruction. */
	class LSR_General : public Instruction
	{
		public:
		LSR_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool LSR_General::executeOn (const MCHEmul::Address& a)
	{
		// Read the value, makes the operation and set it back!
		MCHEmul::UByte v = memory () -> value (a); // 1 byte long always
		bool c = v.shiftRightC (false /** 0 is put into */, 1); // Keeps the status of the last bit to actualize later the carry flag
		memory () -> set (a, v);
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, v [7]);
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x4e, 3, 6, _6500RW(6,4),	"LSR[$2]",				LSR_Absolute, LSR_General);
	_INST6500_FROM (0x46, 2, 5, _6500RW(5,3),	"LSR[$1]",				LSR_ZeroPage, LSR_General);
	_INST6500_FROM (0x4a, 1, 2, _6500R(2),		"LSR",					LSR_Accumulator, LSR_General);
	_INST6500_FROM (0x56, 2, 6, _6500RW(6,4),	"LSR[$1],X",			LSR_ZeroPageX, LSR_General);
	_INST6500_FROM (0x5e, 3, 7, _6500RW(7,5),	"LSR[$2],X",			LSR_AbsoluteX, LSR_General);

	// NOP
	/** NOP_General. \n 
		There is only one official version of NOP, but many undocumented. */
	class NOP_General : public Instruction
	{
		public:
		NOP_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool NOP_General::executeWith (const MCHEmul::UByte& u)
	{
		// Nothing to do...

		return (true);
	}

	// The official and documented one...
	_INST6500_FROM (0xea, 1, 2, _6500R(2),		"NOP",					NOP, Instruction);
	// The rest are undocumented...
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	_INST6500_FROM (0x80 /** 0x82 (inestable), 0xc2 (insestable), 0xe2 (inestable) */, 2, 2, _6500R(2), "NOP#[#1]", NOP_Inmediate, NOP_General);
	_INST6500_FROM (0x0c, 3, 6, _6500R(6),		"NOP[$2]",				NOP_Absolute, NOP_General);
	_INST6500_FROM (0x04 /** 0x04, 0x44, 0x64 */, 2, 5, _6500R(5), "NOP[$1]", NOP_ZeroPage, NOP_General);
	_INST6500_FROM (0x14 /** 0x34, 0x54, 0x74, 0xd4, 0xf4 */, 2, 6, _6500R(6), "NOP[$1],X", NOP_ZeroPageX, NOP_General);
	_INST6500_FROM (0x1c /** 0x3c, 0x5c, 0x7x, 0xdc, 0xfc */, 3, 7, _6500R(7), "NOP[$2],X", NOP_AbsoluteX, NOP_General);

	// ORA
	/** ORA_General: To aggregate common steps in every ORA instruction. */
	class ORA_General : public Instruction
	{
		public:
		ORA_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool ORA_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();
	
		// Read the value, makes the operation and sets it back...
		MCHEmul::UByte r = a.values ()[0] | u;
		a.set ({ r });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r [7]);
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UByte::_0);

		return (true);
	}

	_INST6500_FROM (0x09, 2, 2, _6500R(2),		"ORA#[#1]",				ORA_Inmediate, ORA_General);
	_INST6500_FROM (0x0d, 3, 4, _6500R(4),		"ORA[$2]",				ORA_Absolute, ORA_General);
	_INST6500_FROM (0x05, 2, 3, _6500R(3),		"ORA[$1]",				ORA_ZeroPage, ORA_General);
	_INST6500_FROM (0x01, 2, 6, _6500R(6),		"ORA([$1],X)",			ORA_ZeroPageIndirectX, ORA_General);
	_INST6500_FROM (0x11, 2, 5, _6500R(5),		"ORA([$1]),Y",			ORA_ZeroPageIndirectY, ORA_General);
	_INST6500_FROM (0x15, 2, 4, _6500R(4),		"ORA[$1],X",			ORA_ZeroPageX, ORA_General);
	_INST6500_FROM (0x1d, 3, 4, _6500R(4),		"ORA[$2],X",			ORA_AbsoluteX, ORA_General);
	_INST6500_FROM (0x19, 3, 4, _6500R(4),		"ORA[$2],Y",			ORA_AbsoluteY, ORA_General);

	// PHA
	_INST6500_FROM (0x48, 1, 3, _6500RW(3,2),	"PHA",					PHA, Instruction);

	// PHP
	_INST6500_FROM (0x08, 1, 3, _6500RW(3,2),	"PHP",					PHP, Instruction);

	// PLA
	_INST6500_FROM (0x68, 1, 4, _6500R(4),		"PLA",					PLA, Instruction);

	// PLP
	_INST6500_FROM (0x28, 1, 4, _6500R(4),		"PLP",					PLP, Instruction);

	// Non documented
	// RLA: ROL + AND
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** RLA_General: To aggregate common steps in every RLA instruction. */
	class RLA_General : public Instruction
	{
		public:
		RLA_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool RLA_General::executeOn (const MCHEmul::Address& a)
	{
		MCHEmul::Register& ac = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();

		// The memory is affected...
		MCHEmul::UByte v = memory () -> value (a); // 1 byte long always...
		bool c = v.rotateLeftC (st.bitStatus (C6500::_CARRYFLAG), 1); // ROL...
		memory () -> set (a, v);
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// ...and also the accumulator...
		v = ac.values ()[0] & v; // ...AND
		ac.set ({ v });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, v [7]);
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x2f, 3, 6, _6500RW(6,4),	"RLA[$2]",				RLA_Absolute, RLA_General);
	_INST6500_FROM (0x27, 2, 5, _6500RW(5,3),	"RLA[$1]",				RLA_ZeroPage, RLA_General);
	_INST6500_FROM (0x23, 2, 8, _6500RW(8,6),	"RLA([$1],X)",			RLA_ZeroPageIndirectX, RLA_General);
	_INST6500_FROM (0x33, 2, 8, _6500RW(8,6),	"RLA([$1]),Y",			RLA_ZeroPageIndirectY, RLA_General);
	_INST6500_FROM (0x37, 2, 6, _6500RW(6,4),	"RLA[$1],X",			RLA_ZeroPageX, RLA_General);
	_INST6500_FROM (0x3f, 3, 7, _6500RW(7,5),	"RLA[$2],X",			RLA_AbsoluteX, RLA_General);
	_INST6500_FROM (0x3b, 3, 7, _6500RW(7,5),	"RLA[$2],Y",			RLA_AbsoluteY, RLA_General);

	// ROL
	/** ROL_General: To aggregate common steps in every ROL instruction. */
	class ROL_General : public Instruction
	{
		public:
		ROL_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool ROL_General::executeOn (const MCHEmul::Address& a)
	{
		MCHEmul::StatusRegister& st = statusRegister ();
		bool c = st.bitStatus (C6500::_CARRYFLAG); 

		// Read the value, makes the operation and set it back!
		MCHEmul::UByte v = memory () -> value (a); // 1 byte long always
		c = v.rotateLeftC (c /** The carry is put into. */, 1); // Keeps the status of the last bit to actualize later the carry flag
		memory () -> set (a, v);
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, v [7]);
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x2e, 3, 6, _6500RW(6,4),	"ROL[$2]",				ROL_Absolute, ROL_General);
	_INST6500_FROM (0x26, 2, 5, _6500RW(5,3),	"ROL[$1]",				ROL_ZeroPage, ROL_General);
	_INST6500_FROM (0x2a, 1, 2, _6500R(2),		"ROL",					ROL_Accumulator, ROL_General);
	_INST6500_FROM (0x36, 2, 6, _6500RW(6,4),	"ROL[$1],X",			ROL_ZeroPageX, ROL_General);
	_INST6500_FROM (0x3e, 3, 7, _6500RW(7,5),	"ROL[$2],X",			ROL_AbsoluteX, ROL_General);

	// ROR
	/** ROR_General: To aggregate common steps in every ROR instruction. */
	class ROR_General : public Instruction
	{
		public:
		ROR_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool ROR_General::executeOn (const MCHEmul::Address& a)
	{
		MCHEmul::StatusRegister& st = statusRegister ();
		bool c = st.bitStatus (C6500::_CARRYFLAG); 

		// Read the value, makes the operation and set it back!
		MCHEmul::UByte v = memory () -> value (a); // 1 byte long always
		c = v.rotateRightC (c /** The carry is put into. */, 1); // Keeps the status of the last bit to actualize later the carry flag
		memory () -> set (a, v);
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, v [7]);
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x6e, 3, 6, _6500RW(6,4),	"ROR[$2]",				ROR_Absolute, ROR_General);
	_INST6500_FROM (0x66, 2, 5, _6500RW(5,3),	"ROR[$1]",				ROR_ZeroPage, ROR_General);
	_INST6500_FROM (0x6a, 1, 2, _6500R(2),		"ROR",					ROR_Accumulator, ROR_General);
	_INST6500_FROM (0x76, 2, 6, _6500RW(6,4),	"ROR[$1],X",			ROR_ZeroPageX, ROR_General);
	_INST6500_FROM (0x7e, 3, 7, _6500RW(7,5),	"ROR[$2],X",			ROR_AbsoluteX, ROR_General);

	// Non documented
	// RRA: ROR + ADC
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** RRA_General: To aggregate common steps in every RRA instruction. */
	class RRA_General : public Instruction
	{
		public:
		RRA_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool RRA_General::executeOn (const MCHEmul::Address& a)
	{
		MCHEmul::Register& ac = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();

		// The operation affects the memory...
		MCHEmul::UByte v = memory () -> value (a); // Always 1 byte long...
		bool c = v.rotateRightC (st.bitStatus (C6500::_CARRYFLAG), 1); // ROR...
		memory () -> set (a, v);
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// ...and also the accumulator...
		unsigned char ft = st.bitStatus (C6500::_DECIMALFLAG) 
			? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
		MCHEmul::UInt r = MCHEmul::UInt (ac.values ()[0], ft).add (MCHEmul::UInt (v, ft), c); // ...ADC
		ac.set (r.bytes ());

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_OVERFLOWFLAG, r.overflow ());
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ());

		return (true);
	}

	_INST6500_FROM (0x6f, 3, 6, _6500RW(6,4),	"RRA[$2]",				RRA_Absolute, RRA_General);
	_INST6500_FROM (0x67, 2, 5, _6500RW(5,3),	"RRA[$1]",				RRA_ZeroPage, RRA_General);
	_INST6500_FROM (0x63, 2, 8, _6500RW(8,6),	"RRA([$1],X)",			RRA_ZeroPageIndirectX, RRA_General);
	_INST6500_FROM (0x73, 2, 8, _6500RW(8,6),	"RRA([$1]),Y",			RRA_ZeroPageIndirectY, RRA_General);
	_INST6500_FROM (0x77, 2, 6, _6500RW(6,4),	"RRA[$1],X",			RRA_ZeroPageX, RRA_General);
	_INST6500_FROM (0x7f, 3, 7, _6500RW(7,5),	"RRA[$2],X",			RRA_AbsoluteX, RRA_General);
	_INST6500_FROM (0x7b, 3, 7, _6500RW(7,5),	"RRA[$2],Y",			RRA_AbsoluteY, RRA_General);

	// RTI
	_INST6500_FROM (0x40, 1, 6, _6500R(6),		"RTI",					RTI, Instruction);

	// RTS
	_INST6500_FROM (0x60, 1, 6, _6500R(6),		"RTS",					RTS, Instruction);

	// Non documented
	// SAX: PHP + PHA + STX + AND + STA + PLA + PLP = _ACCUMULATOR & _XREGISTER...
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** SAX_General: To aggregate common steps in every SAX instruction. */
	class SAX_General : public Instruction
	{
		public:
		SAX_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool SAX_General::executeOn (const MCHEmul::Address& a)
	{
		// At the end the memory is stored with _ACCUMULATOR & _XREGISTER
		MCHEmul::UByte dt = registerA ().values ()[0] &
			registerX ().values ()[0];
		memory () -> set (a, dt); // Always 1 byte long...
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ dt });

		// ...with no impact in registers...

		return (true);
	}

	_INST6500_FROM (0x8f, 3, 4, _6500RW(4,3),	"SAX[$2]",				SAX_Absolute, SAX_General);
	_INST6500_FROM (0x87, 2, 3, _6500RW(3,2),	"SAX[$1]",				SAX_ZeroPage, SAX_General);
	_INST6500_FROM (0x97, 2, 6, _6500RW(6,5),	"SAX[$1],Y",			SAX_ZeroPageY, SAX_General);
	_INST6500_FROM (0x83, 2, 6, _6500RW(6,5),	"SAX([$1],X)",			SAX_ZeroPageIndirectX, SAX_General);

	// SBC
	/** SBC_General: To aggregate common steps in every SBC instruction. */
	class SBC_General : public Instruction
	{
		public:
		SBC_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool SBC_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();

		// Read the value, makes the operation and set it back!
		unsigned char ft = st.bitStatus (C6500::_DECIMALFLAG) 
			? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
		MCHEmul::UInt r = MCHEmul::UInt (a.values ()[0], ft).
			substract (MCHEmul::UInt (u, ft), st.bitStatus (C6500::_CARRYFLAG));
		a.set (r.bytes ()); // The carry is taken into account in the substraction

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, r.negative ());
		st.setBitStatus (C6500::_OVERFLOWFLAG, r.overflow ());
		st.setBitStatus (C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
		st.setBitStatus (C6500::_CARRYFLAG, r.carry ());

		return (true);
	}

	_INST6500_FROM (0xe9 /** 0xeb (undocumented) */, 2, 2, _6500R(2), "SBC#[#1]", SBC_Inmediate, SBC_General);
	_INST6500_FROM (0xed, 3, 4, _6500R(4),		"SBC[$2]",				SBC_Absolute, SBC_General);
	_INST6500_FROM (0xe5, 2, 3, _6500R(3),		"SBC[$1]",				SBC_ZeroPage, SBC_General);
	_INST6500_FROM (0xe1, 2, 6, _6500R(6),		"SBC([$1],X)",			SBC_ZeroPageIndirectX, SBC_General);
	_INST6500_FROM (0xf1, 2, 5, _6500R(5),		"SBC([$1]),Y",			SBC_ZeroPageIndirectY, SBC_General);
	_INST6500_FROM (0xf5, 2, 4, _6500R(4),		"SBC[$1],X",			SBC_ZeroPageX, SBC_General);
	_INST6500_FROM (0xfd, 3, 4, _6500R(4),		"SBC[$2],X",			SBC_AbsoluteX, SBC_General);
	_INST6500_FROM (0xf9, 3, 4, _6500R(4),		"SBC[$2],Y",			SBC_AbsoluteY, SBC_General);

	// Non documented
	// SBX: STA + TXA + AND + CMP + PHP + SEC + CLD + SBC + TAX + LDA + PLP = (_ACCUMULATOR & _XREGISTER) - DATA
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** SBX_General: To aggregate common steps in every SBX instruction. */
	class SBX_General : public Instruction
	{
		public:
		SBX_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (const MCHEmul::UByte& u);
	};

	// ---
	inline bool SBX_General::executeWith (const MCHEmul::UByte& u)
	{
		MCHEmul::Register& x = registerX ();
		MCHEmul::UInt v = MCHEmul::UInt (cpu () -> 
			internalRegister (C6510::_ACCUMULATOR).values ()[0] & x.values ()[0]) - MCHEmul::UInt ({ u });
		x.set (v.values ()); // But always 1 byte long...

		// Time of the status register...
		MCHEmul::StatusRegister& st = statusRegister ();
		st.setBitStatus (C6500::_NEGATIVEFLAG, v.negative ());
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UInt::_0);
		st.setBitStatus (C6500::_CARRYFLAG, v.carry ());

		return (true);
	}

	_INST6500_FROM (0xcb, 2, 2, _6500R(2),		"SBX#[#1]",				SBX_Inmediate, SBX_General);

	// SEC
	_INST6500_FROM (0x38, 1, 2, _6500R(2),		"SEC",					SEC, Instruction);

	// SED
	_INST6500_FROM (0xf8, 1, 2, _6500R(2),		"SED",					SED, Instruction);

	// SEI
	_INST6500_FROM (0x78, 1, 2, _6500R(2),		"SEI",					SEI, Instruction);

	// Non documented
	// SLO: ASL + ORA
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** SLO_General: To aggregate common steps in every SLO instruction. */
	class SLO_General : public Instruction
	{
		public:
		SLO_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool SLO_General::executeOn (const MCHEmul::Address& a)
	{
		MCHEmul::Register& ac = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();

		// The memory is affected...
		MCHEmul::UByte v = memory () -> value (a); 
		bool c = v.shiftLeftC (false /** 0 is put into */, 1); // ASL...
		memory () -> set (a, v);
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// ...and also the accumulator...
		v = ac.values ()[0] | v; // ...ORA
		ac.set ({ v });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, v [7]);
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x0f, 3, 6, _6500RW(6,4),	"SLO[$2]",				SLO_Absolute, SLO_General);
	_INST6500_FROM (0x07, 2, 5, _6500RW(5,3),	"SLO[$1]",				SLO_ZeroPage, SLO_General);
	_INST6500_FROM (0x03, 2, 8, _6500RW(8,6),	"SLO([$1],X)",			SLO_ZeroPageIndirectX, SLO_General);
	_INST6500_FROM (0x13, 2, 8, _6500RW(8,6),	"SLO([$1]),Y",			SLO_ZeroPageIndirectY, SLO_General);
	_INST6500_FROM (0x17, 2, 6, _6500RW(6,4),	"SLO[$1],X",			SLO_ZeroPageX, SLO_General);
	_INST6500_FROM (0x1f, 3, 7, _6500RW(7,5),	"SLO[$2],X",			SLO_AbsoluteX, SLO_General);
	_INST6500_FROM (0x1b, 3, 7, _6500RW(7,5),	"SLO[$2],Y",			SLO_AbsoluteY, SLO_General);

	// Non documented
	// SRE: LSR + EOR
	// https://www.esocop.org/docs/MOS6510UnintendedOpcodes-20152412.pdf
	/** SRE_General: To aggregate common steps in every SRE instruction. */
	class SRE_General : public Instruction
	{
		public:
		SRE_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool SRE_General::executeOn (const MCHEmul::Address& a)
	{
		MCHEmul::Register& ac = registerA ();
		MCHEmul::StatusRegister& st = statusRegister ();

		// The operation affects the memory...
		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.shiftRightC (false, 1); // LSR...
		memory () -> set (a, v);
		_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v });

		// ...and also the accumulator...
		v = ac.values ()[0] ^ v; // ...EOR
		ac.set ({ v });

		// Time of the status register...
		st.setBitStatus (C6500::_NEGATIVEFLAG, v [7]);
		st.setBitStatus (C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (C6500::_CARRYFLAG, c);

		return (true);
	}

	_INST6500_FROM (0x4f, 3, 6, _6500RW(6,4),	"SRE[$2]",				SRE_Absolute, SRE_General);
	_INST6500_FROM (0x47, 2, 5, _6500RW(5,3),	"SRE[$1]",				SRE_ZeroPage, SRE_General);
	_INST6500_FROM (0x43, 2, 8, _6500RW(8,6),	"SRE([$1],X)",			SRE_ZeroPageIndirectX, SRE_General);
	_INST6500_FROM (0x53, 2, 8, _6500RW(8,6),	"SRE([$1]),Y",			SRE_ZeroPageIndirectY, SRE_General);
	_INST6500_FROM (0x57, 2, 6, _6500RW(6,4),	"SRE[$1],X",			SRE_ZeroPageX, SRE_General);
	_INST6500_FROM (0x5f, 3, 7, _6500RW(7,5),	"SRE[$2],X",			SRE_AbsoluteX, SRE_General);
	_INST6500_FROM (0x5b, 3, 7, _6500RW(7,5),	"SRE[$2],Y",			SRE_AbsoluteY, SRE_General);

	// STA
	/** STA_General: To aggregate common steps in every STA instruction. */
	class STA_General : public Instruction
	{
		public:
		STA_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool STA_General::executeOn (const MCHEmul::Address& a)
	{
		// Set the value...
		memory () -> set (a, 
			_lastExecutionData._INOUTData = registerA ().values ()); // 1 byte-length

		return (true);
	}

	_INST6500_FROM (0x8d, 3, 4, _6500RW(4,3),	"STA[$2]",				STA_Absolute, STA_General);
	_INST6500_FROM (0x85, 2, 3, _6500RW(3,2),	"STA[$1]",				STA_ZeroPage, STA_General);
	_INST6500_FROM (0x81, 2, 6, _6500RW(6,5),	"STA([$1],X)",			STA_ZeroPageIndirectX, STA_General);
	_INST6500_FROM (0x91, 2, 6, _6500RW(6,5),	"STA([$1]),Y",			STA_ZeroPageIndirectY, STA_General);
	_INST6500_FROM (0x95, 2, 4, _6500RW(4,3),	"STA[$1],X",			STA_ZeroPageX, STA_General);
	_INST6500_FROM (0x9d, 3, 5, _6500RW(5,4),	"STA[$2],X",			STA_AbsoluteX, STA_General);
	_INST6500_FROM (0x99, 3, 5, _6500RW(5,4),	"STA[$2],Y",			STA_AbsoluteY, STA_General);

	// STX
	/** STX_General: To aggregate common steps in every STX instruction. */
	class STX_General : public Instruction
	{
		public:
		STX_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool STX_General::executeOn (const MCHEmul::Address& a)
	{
		// Set the value...
		memory () -> set (a, 
			_lastExecutionData._INOUTData = registerX ().values ()); // 1 byte-length

		return (true);
	}

	_INST6500_FROM (0x8e, 3, 4, _6500RW(4,3),	"STX[$2]",				STX_Absolute, STX_General);
	_INST6500_FROM (0x86, 2, 3, _6500RW(3,2),	"STX[$1]",				STX_ZeroPage, STX_General);
	_INST6500_FROM (0x96, 2, 4, _6500RW(4,3),	"STX[$1],Y",			STX_ZeroPageY, STX_General);

	// STY
	/** STY_General: To aggregate common steps in every STY instruction. */
	class STY_General : public Instruction
	{
		public:
		STY_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeOn (const MCHEmul::Address& a);
	};

	// ---
	inline bool STY_General::executeOn (const MCHEmul::Address& a)
	{
		// Set the value...
		memory () -> set (a, 
			_lastExecutionData._INOUTData = registerY ().values ()); // 1 byte-length

		return (true);
	}

	_INST6500_FROM (0x8c, 3, 4, _6500RW(4,3),	"STY[$2]",				STY_Absolute, STY_General);
	_INST6500_FROM (0x84, 2, 3, _6500RW(3,2),	"STY[$1]",				STY_ZeroPage, STY_General);
	_INST6500_FROM (0x94, 2, 4, _6500RW(4,3),	"STY[$1],X",			STY_ZeroPageX, STY_General);

	// TAX
	_INST6500_FROM (0xaa, 1, 2, _6500R(2),		"TAX",					TAX, Instruction);

	// TAY
	_INST6500_FROM (0xa8, 1, 2, _6500R(2),		"TAY",					TAY, Instruction);

	// TSX
	_INST6500_FROM (0xba, 1, 2, _6500R(2),		"TSX",					TSX, Instruction);

	// TXA
	_INST6500_FROM (0x8a, 1, 2, _6500R(2),		"TXA",					TXA, Instruction);

	// TXS
	_INST6500_FROM (0x9a, 1, 2, _6500R(2),		"TXS",					TXS, Instruction);

	// TYA
	_INST6500_FROM (0x98, 1, 2, _6500R(2),		"TYA",					TYA, Instruction);
}

#endif
  
// End of the file
/*@}*/
