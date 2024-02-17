/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Czx80.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: All CPUs type ZX80 inherits from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_CZX80__
#define __FZ80_CZX80__

#include <CORE/incs.hpp>

namespace FZ80
{
	class CZ80;

	/** The Z80 m anages many ports.
		And the way they are understood will depend on the computer,
		although there is a defaul implementation per each. */
	class Z80Port
	{
		public:
		Z80Port (unsigned char id, const std::string& name)
			: _id (id), _name (name),
			  _cpu (nullptr)
							{ }

		unsigned char id () const
							{ return (_id); }
		const std::string& name () const
							{ return (_name); }

		/** To get & set the values.
			They must be overloaded, depending on the computer that uses it. */
		virtual MCHEmul::UByte value () const = 0;
		virtual void setValue (const MCHEmul::UByte& v) = 0;

		virtual void initialize ()
							{ setValue (0); }

		virtual MCHEmul::InfoStructure getInfoStructure () const;

		protected:
		unsigned char _id;
		std::string _name;

		// Implementation
		CZ80* _cpu;
	};

	// To simplify the management of a list of ports!...
	using Z80PortsList = std::vector <Z80Port*>;
	using Z80PortsMap = std::map <unsigned char, Z80Port*>;

	/** The basic port keeps its own value. */
	class Z80BasicPort final : public Z80Port
	{
		public:
		Z80BasicPort (unsigned char n, const std::string& name)
			: Z80Port (n, name),
			  _value (MCHEmul::UByte::_0)
							{ }

		virtual MCHEmul::UByte value () const override
							{ return (_value); }
		virtual void setValue (const MCHEmul::UByte& v) override
							{ _value = v; }
		
		private:
		MCHEmul::UByte _value;
	};

	/** The Chip CPU type ZX80 */
	class CZ80 : public MCHEmul::CPU
	{
		public:
		/** To identify the number and name of the registers. */
		static const size_t _AREGISTER	 = 0;
		// The F Register is the status...
		static const size_t _BREGISTER	 = 1;
		static const size_t _CREGISTER	 = 2;
		static const size_t _DREGISTER	 = 3;
		static const size_t _EREGISTER	 = 4;
		static const size_t _HREGISTER	 = 5;
		static const size_t _LREGISTER	 = 6;
		static const size_t _APREGISTER	 = 7;
		static const size_t _FPREGISTER	 = 8;
		static const size_t _BPREGISTER	 = 9;
		static const size_t _CPREGISTER	 = 10;
		static const size_t _DPREGISTER	 = 11;
		static const size_t _EPREGISTER	 = 12;
		static const size_t _HPREGISTER	 = 13;
		static const size_t _LPREGISTER	 = 14;
		static const size_t _IXHREGISTER = 15;
		static const size_t _IXLREGISTER = 16;
		static const size_t _IYHREGISTER = 17;
		static const size_t _IYLREGISTER = 18;
		static const size_t _IREGISTER   = 19;
		static const size_t _RREGISTER   = 20;

		/** To identify the position of the flags in the status register. */
		static const size_t _CARRYFLAG = 0;
		static const std::string _CARRYFLAGNAME;
		static const size_t _NEGATIVEFLAG = 1;
		static const std::string _NEGATIVEFLAGNAME;
		static const size_t _PARITYOVERFLOWFLAG = 2;
		static const std::string _PARITYOVERFLOWFLAGNAME;
		static const size_t _BIT3FLAG = 3;
		static const std::string _BIT3FLAGNAME;
		static const size_t _HALFCARRYFLAG = 4;
		static const std::string _HALFCARRYFLAGNAME;
		static const size_t _BIT5FLAG = 5;
		static const std::string _BIT5FLAGNAME;
		static const size_t _ZEROFLAG = 6;
		static const std::string _ZEROFLAGNAME;
		static const size_t _SIGNFLAG = 7;
		static const std::string _SIGNFLAGNAME;

		/** The different possibilities a Z80 instruction set has to understand its parameters. */
		enum class AddressMode
		{
			_INMEDIATE,				// The parameter is a 8 bit value
			_INMEDIATEEXTENDED,		// The parameter is a 16 bit value
			_ZEPOPAGEMODIFIED,		// The parameter is an address in the zero page
			_RELATIVE,				// The parameter is a number of steps in pc counter from the current position
			_EXTENDED,				// The paremeter is an address in the memory
			_INDEXED,				// The paremeter is an address from the registers HL, IX or IY
			_REGISTER,				// There is no explicit parameters. They are all implicit.
			_INDIRECTREGISTER,		// Used in stck movements
			_IMPLICIT,				// The parameter is implicit in the instruction
			_BIT,					// The parameter is the bit of a register
		};

		CZ80 (int id, const Z80PortsMap& pts = { },	
			const MCHEmul::Attributes& attrs = // By default...
				{  { "Code", "Z80" },
				   { "Manufacturer", "Zilog"},
				   { "Year", "1976" },
				   { "Speed Range", "2.5 - 10 MHz" } });

		~CZ80 ();

		unsigned char INTmode () const
						{ return (_INTMode); }
		void setINTMode (unsigned char iM)
						{ _INTMode = iM; }

		/** In the case of the interrupt type 0, what the processor does is to execute the code of the instruction
			that is in the data bus. */
		/** The type of interrupt type 1 uses this address and it is constant. */
		MCHEmul::Address INT1VectorAddress () const
							{ return (MCHEmul::Address ({ 0x38, 0xff }, false /** Little - endian */)); }
		/** In the case of the interrupt type 2, the address to jump to is deducted from the values in the dataBus and register I. */
		MCHEmul::Address INT2VectorAddress () const
							{ return (MCHEmul::Address 
								({ _lastInstruction -> lastINOUTData ()[0], iRegister ().values () [0] }, false /** Little - endian */)); }
		MCHEmul::Address NMIVectorAddress () const
							{ return (MCHEmul::Address ({ 0x66, 0x00 }, false /** Little - endian */)); }

		// Accesing the main registers...
		MCHEmul::Register& aRegister ()
							{ return (internalRegister (_AREGISTER)); }
		const MCHEmul::Register& aRegister () const
							{ return (internalRegister (_AREGISTER)); }
		MCHEmul::Register& fRegister ()
							{ return (statusRegister ()); }
		const MCHEmul::Register& fRegister () const
							{ return (statusRegister ()); }
		MCHEmul::RefRegisters& afRegister ()
							{ return (_afRegister); }
		const MCHEmul::RefRegisters& afRegister () const
							{ return (_afRegister); }

		MCHEmul::Register& bRegister ()
							{ return (internalRegister (_BREGISTER)); }
		const MCHEmul::Register& bRegister () const
							{ return (internalRegister (_BREGISTER)); }
		MCHEmul::Register& cRegister ()
							{ return (internalRegister (_CREGISTER)); }
		const MCHEmul::Register& cRegister () const
							{ return (internalRegister (_CREGISTER)); }
		MCHEmul::RefRegisters& bcRegister ()
							{ return (_bcRegister); }
		const MCHEmul::RefRegisters& bcRegister () const
							{ return (_bcRegister); }

		MCHEmul::Register& dRegister ()
							{ return (internalRegister (_DREGISTER)); }
		const MCHEmul::Register& dRegister () const
							{ return (internalRegister (_DREGISTER)); }
		MCHEmul::Register& eRegister ()
							{ return (internalRegister (_EREGISTER)); }
		const MCHEmul::Register& eRegister () const
							{ return (internalRegister (_EREGISTER)); }
		MCHEmul::RefRegisters& deRegister ()
							{ return (_deRegister); }
		const MCHEmul::RefRegisters& deRegister () const
							{ return (_deRegister); }
		
		MCHEmul::Register& hRegister ()
							{ return (internalRegister (_HREGISTER)); }
		const MCHEmul::Register& hRegister () const
							{ return (internalRegister (_HREGISTER)); }
		MCHEmul::Register& lRegister ()
							{ return (internalRegister (_LREGISTER)); }
		const MCHEmul::Register& lRegister () const
							{ return (internalRegister (_LREGISTER)); }
		MCHEmul::RefRegisters& hlRegister ()
							{ return (_hlRegister); }
		const MCHEmul::RefRegisters& hlRegister () const
							{ return (_hlRegister); }

		// Accesing the alternative registers...
		MCHEmul::Register& apRegister ()
							{ return (internalRegister (_APREGISTER)); }
		const MCHEmul::Register& apRegister () const
							{ return (internalRegister (_APREGISTER)); }
		MCHEmul::Register& fpRegister ()
							{ return (internalRegister (_FPREGISTER)); }
		const MCHEmul::Register& fpRegister () const
							{ return (internalRegister (_FPREGISTER)); }
		MCHEmul::RefRegisters& afpRegister ()
							{ return (_afpRegister); }
		const MCHEmul::RefRegisters& afpRegister () const
							{ return (_afpRegister); }

		MCHEmul::Register& bpRegister ()
							{ return (internalRegister (_BPREGISTER)); }
		const MCHEmul::Register& bpRegister () const
							{ return (internalRegister (_BPREGISTER)); }
		MCHEmul::Register& cpRegister ()
							{ return (internalRegister (_CPREGISTER)); }
		const MCHEmul::Register& cpRegister () const
							{ return (internalRegister (_CPREGISTER)); }
		MCHEmul::RefRegisters& bcpRegister ()
							{ return (_bcpRegister); }
		const MCHEmul::RefRegisters& bcpRegister () const
							{ return (_bcpRegister); }

		MCHEmul::Register& dpRegister ()
							{ return (internalRegister (_DPREGISTER)); }
		const MCHEmul::Register& dpRegister () const
							{ return (internalRegister (_DPREGISTER)); }
		MCHEmul::Register& epRegister ()
							{ return (internalRegister (_EPREGISTER)); }
		const MCHEmul::Register& epRegister () const
							{ return (internalRegister (_EPREGISTER)); }
		MCHEmul::RefRegisters& depRegister ()
							{ return (_depRegister); }
		const MCHEmul::RefRegisters& depRegister () const
							{ return (_depRegister); }

		MCHEmul::Register& hpRegister ()
							{ return (internalRegister (_HPREGISTER)); }
		const MCHEmul::Register& hpRegister () const
							{ return (internalRegister (_HPREGISTER)); }
		MCHEmul::Register& lpRegister ()
							{ return (internalRegister (_LPREGISTER)); }
		const MCHEmul::Register& lpRegister () const
							{ return (internalRegister (_LPREGISTER)); }
		MCHEmul::RefRegisters& hlpRegister ()
							{ return (_hlpRegister); }
		const MCHEmul::RefRegisters& hlpRegister () const
							{ return (_hlpRegister); }

		// Accesing the index registers...
		MCHEmul::Register& ixhRegister ()
							{ return (internalRegister (_IXHREGISTER)); }
		const MCHEmul::Register& ixhRegister () const
							{ return (internalRegister (_IXHREGISTER)); }
		MCHEmul::Register& ixlRegister ()
							{ return (internalRegister (_IXLREGISTER)); }
		const MCHEmul::Register& ixlRegister () const
							{ return (internalRegister (_IXLREGISTER)); }
		MCHEmul::RefRegisters& ixRegister ()
							{ return (_ixRegister); }
		const MCHEmul::RefRegisters& ixRegister () const
							{ return (_ixRegister); }

		MCHEmul::Register& iyhRegister ()
							{ return (internalRegister (_IYHREGISTER)); }
		const MCHEmul::Register& iyhRegister () const
							{ return (internalRegister (_IYHREGISTER)); }
		MCHEmul::Register& iylRegister ()
							{ return (internalRegister (_IYLREGISTER)); }
		const MCHEmul::Register& iylRegister () const
							{ return (internalRegister (_IYLREGISTER)); }
		MCHEmul::RefRegisters& iyRegister ()
							{ return (_iyRegister); }
		const MCHEmul::RefRegisters& iyRegister () const
							{ return (_iyRegister); }

		// Accesing other registers used only in some computers...
		MCHEmul::Register& iRegister ()
							{ return (internalRegister (_IREGISTER)); }
		const MCHEmul::Register& iRegister () const
							{ return (internalRegister (_IREGISTER)); }
		MCHEmul::Register& rRegister ()
							{ return (internalRegister (_RREGISTER)); }
		const MCHEmul::Register& rRegister () const
							{ return (internalRegister (_RREGISTER)); }

		// Managing the status of the flipflop registers...
		bool IFF1 () const
							{ return (_IFF1); }
		void setIFF1 (bool v)
							{ _IFF1 = v; }
		bool IFF2 () const
							{ return (_IFF2); }
		void setIFF2 (bool v)
							{ _IFF2 = v; }

		// Managing the ports...
		/** The way that the port is read, can be changed depending on the computer. \n
			i.e in ZX Spectrum, when reading port $FE the value of the register A is taken into account. */
		MCHEmul::UByte port (unsigned char p) const
							{ return (_portsRaw [(size_t) p] -> value ()); }
		void setPort (unsigned char p, const MCHEmul::UByte& v)
							{ _portsRaw [(size_t) p] -> setValue (v); }

		virtual bool initialize () override;

		protected:
		/** The type of interruption. */
		unsigned char _INTMode;

		/** The registers that are made up of two. */
		MCHEmul::RefRegisters _afRegister;  // A and F
		MCHEmul::RefRegisters _bcRegister;
		MCHEmul::RefRegisters _deRegister;
		MCHEmul::RefRegisters _hlRegister;
		MCHEmul::RefRegisters _afpRegister; // A and F shadow
		MCHEmul::RefRegisters _bcpRegister;
		MCHEmul::RefRegisters _depRegister;
		MCHEmul::RefRegisters _hlpRegister;
		MCHEmul::RefRegisters _ixRegister;
		MCHEmul::RefRegisters _iyRegister;

		/** The ports value. */
		Z80PortsMap _ports;

		// The flipflop register in Z80 help to control the status of the interrupts!
		bool _IFF1;
		bool _IFF2;

		private:
		Z80PortsList _portsRaw;

		private:
		// Implementation
		static MCHEmul::CPUArchitecture createArchitecture ();
		static MCHEmul::Registers createInternalRegisters ();
		static MCHEmul::StatusRegister createStatusRegister ();
		static MCHEmul::Instructions createInstructions ();
	};
}

#endif
  
// End of the file
/*@}*/
