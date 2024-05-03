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
#include <FZ80/INTInterrupt.hpp>

namespace FZ80
{
	class CZ80;

	/** The Z80 manages many ports (up to 256). \n
		And the way they are "understood" will depend on the computer,
		and even to the devices connected to them. */
	class Z80Port : public MCHEmul::Observer, public MCHEmul::InfoClass
	{
		public:
		friend CZ80;

		/** Always needed an id, a and name and an optional set of attributes. */
		Z80Port (unsigned char id, const std::string& name, 
					const MCHEmul::Attributes& attrs = { })
			: MCHEmul::InfoClass ("Z80Port"),
			  _id (id), _name (name),
			  _cpu (nullptr),
			  _attributes (attrs)
							{ }

		unsigned char id () const
							{ return (_id); }
		const std::string& name () const
							{ return (_name); }
		const CZ80* cpu () const
							{ return (_cpu); }
		CZ80* cpu ()
							{ return (_cpu); }
		
		const MCHEmul::Attributes& attributes () const
							{ return (_attributes); }
		inline const std::string& attribute (const std::string& atrN) const;

		/** To get & set the values.
			They must be overloaded, depending on the computer that uses it. 
			As the port can be connected to many id ports, the port id is also received itself. */
		virtual MCHEmul::UByte value (unsigned char id) const = 0;
		virtual MCHEmul::UByte peekValue (unsigned char id) const
							{ return (value (id)); } // Same than previous but to avoid modify the status... equal by defautl
		virtual void setValue (unsigned char id, const MCHEmul::UByte& v) = 0;

		virtual void initialize ()
							{ setValue (0, 0); }

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		unsigned char _id;
		std::string _name;
		MCHEmul::Attributes _attributes;

		// Implementation
		/** It is adjusted at CPU initialization. */
		CZ80* _cpu;
	};

	// ---
	inline const std::string& Z80Port::attribute (const std::string& aN) const
	{ 
		MCHEmul::Attributes::const_iterator i = _attributes.find (aN); 
		
		return ((i == _attributes.end ()) 
			? MCHEmul::AttributedNotDefined : (*i).second); 
	}

	// To simplify the management of a set of ports!...
	// A very simple list of ports...
	using Z80PortsPlainList = std::vector <Z80Port*>;
	// Because the same id can be used for several port behaviours!
	using Z80PortsMap = std::multimap <unsigned char, Z80Port*>; 
	// In the same position there could be a set of ports used...
	using Z80PortsList = std::vector <Z80PortsPlainList>;

	/** The Chip CPU type ZX80. \n
		Ports in this type of CPY are quite important! */
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

		/** NOTE: \n
			Take care with the ports map. \n
			The same port behaviour with different id are allowed,
			and also many ports under the same id are allowed. */
		CZ80 (int id, const Z80PortsMap& pts = { },	// Can be assigned later, so they are not mandatory at this point...
			const MCHEmul::Attributes& attrs = // By default...
				{  { "Code", "Z80" },
				   { "Manufacturer", "Zilog"},
				   { "Year", "1976" },
				   { "Speed Range", "2.5 - 10 MHz" } });

		virtual ~CZ80 ()
						{ deletePorts (); }

		unsigned char INTmode () const
						{ return (static_cast <const INTInterrupt*> (interrupt (INTInterrupt::_ID)) -> INTMode ()); }
		void setINTMode (unsigned char iM)
						{ static_cast <INTInterrupt*> (interrupt (INTInterrupt::_ID)) -> setINTMode (iM); }

		/** In the case of the interrupt type 0, what the processor does is to execute the code of the instruction
			that is in the data bus. */
		/** The type of interrupt type 1 uses this address and it is constant. */
		MCHEmul::Address INT1VectorAddress () const
							{ return (MCHEmul::Address ({ 0x38, 0x00 }, false /** Little - endian */)); }
		/** In the case of the interrupt type 2, the address to jump to is deducted from the values in the dataBus and register I. */
		MCHEmul::Address INT2VectorAddress () const
							{ return (MCHEmul::Address 
								({ lastINOUTData ()[0], iRegister ().values () [0] }, false /** Little - endian */)); }
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
		MCHEmul::RefRegisters& irRegister ()
							{ return (_irRegister); }
		const MCHEmul::RefRegisters& irRegister () const
							{ return (_irRegister); }

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
		/** Get / Set ports. */
		const Z80PortsPlainList& port (unsigned char p) const // The list can be empty, but it won't ever be null!
							{ return (_portsRaw [(size_t) p]); }
		Z80PortsPlainList& port (unsigned char p) // The list returned can be empty, but it won't ever be null!
							{ return (_portsRaw [(size_t) p]); }
		const Z80PortsMap& ports () const
							{ return (_ports); }
		/** NOTE: \n
			To replace the ports assigned. \n
			Take care, all previous ones (if existed) will be deleted, 
			so if some of the ports received as parameter existed already attached to the cpu,
			a crash would be generated later when used! */
		void setPorts (const Z80PortsMap& pts)
							{ assignPorts (pts); }
		/** NOTE: \n
			Just to add a couple of ports. \n
			If a port existed already at the same location than now is being tried, 
			it wouldn't be added to avoid having the same port at the same id twice or more!. */
		void addPorts (const Z80PortsMap& pts);

		/** The way that the port is read, can be changed depending on the computer. \n
			When the value of a port is read, just buy default the value of the first port associated to that number is returned. */
		MCHEmul::UByte portValue (unsigned char p) const
							{ return ((*(_portsRaw [(size_t) p]).begin ()) -> value (p)); }
		void setPortValue (unsigned char p, const MCHEmul::UByte& v)
							{ for (const auto& i : _portsRaw [(size_t) p]) i -> setValue (p, v); }

		virtual bool initialize () override;

		/** When a interrupt is requested, 
			and if it possible, the halt situation is unblocked (_haltUnblocked = true) */
		virtual void requestInterrupt (int id, unsigned int nC, MCHEmul::Chip* src = nullptr, int cR = -1) override;

		// Managing the HALT execution...
		bool haltActive () const
							{ return (_haltActive); }
		void setHaltActive (bool a) // What ever the previous situation is, the status is modified...
							{ _haltActive = a; }

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void deletePorts ();
		void assignPorts (const Z80PortsMap& pm);

		protected:
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
		MCHEmul::RefRegisters _irRegister;

		/** The ports value. */
		Z80PortsMap _ports;

		// The flipflop register in Z80 help to control the status of the interrupts!
		bool _IFF1;
		bool _IFF2;

		// To manage the special situation when HALT is executed.
		/** When HALT instruction is executed, this variable is activated,
			and when an interrupt comes later, HALT has to finish. */
		bool _haltActive;

		/** The list of the ports used in the Z80. */
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
