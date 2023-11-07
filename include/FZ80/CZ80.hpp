/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Czx80.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: All CPUs type ZX80 inherits from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_CZX80__
#define __FZ80_CZX80__

#include <CORE/incs.hpp>

namespace FZ80
{
	/** The Chip CPU type ZX80 */
	class CZ80 : public MCHEmul::CPU
	{
		public:
		/** To identify the number and name of the registers. */
		static const size_t _AREGISTER	 = 0;
		// The F Register is the status...
		static const size_t _BREGISTER	 = 2;
		static const size_t _CREGISTER	 = 3;
		static const size_t _DREGISTER	 = 4;
		static const size_t _EREGISTER	 = 5;
		static const size_t _HREGISTER	 = 6;
		static const size_t _LREGISTER	 = 7;
		static const size_t _APREGISTER	 = 10;
		static const size_t _FPREGISTER	 = 11;
		static const size_t _BPREGISTER	 = 11;
		static const size_t _CPREGISTER	 = 12;
		static const size_t _DPREGISTER	 = 13;
		static const size_t _EPREGISTER	 = 14;
		static const size_t _HPREGISTER	 = 15;
		static const size_t _LPREGISTER	 = 16;
		static const size_t _IXHREGISTER = 20;
		static const size_t _IXLREGISTER = 21;
		static const size_t _IYHREGISTER = 22;
		static const size_t _IYLREGISTER = 23;
		static const size_t _IREGISTER   = 24;
		static const size_t _RREGISTER   = 25;

		/** To identify the position of the flags in the status register. */
		static const size_t _CARRYFLAG = 0;
		static const std::string _CARRYFLAGNAME;
		static const size_t _NEGATIVEFLAG = 1;
		static const std::string _NEGATIVEFLAGNAME;
		static const size_t _PARITYOVERFLOWFLAG = 2;
		static const std::string _PARITYOVERFLOWFLAGNAME;
		static const size_t _HALFCARRYFLAG = 4;
		static const std::string _HALFCARRYFLAGNAME;
		static const size_t _ZEROFLAG = 6;
		static const std::string _ZEROFLAGNAME;
		static const size_t _SIGNFLAG = 7;
		static const std::string _SIGNFLAGNAME;

		CZ80 (const MCHEmul::CPUArchitecture& a = createArchitecture ());

		// Accesing the main registers...
		MCHEmul::Register& aRegister ()
							{ return (internalRegister (_AREGISTER)); }
		MCHEmul::Register& fRegister ()
							{ return (statusRegister ()); }
		MCHEmul::RefRegisters& afRegister ()
							{ return (_afRegister); }
		MCHEmul::Register& bRegister ()
							{ return (internalRegister (_BREGISTER)); }
		MCHEmul::Register& cRegister ()
							{ return (internalRegister (_CREGISTER)); }
		MCHEmul::RefRegisters& bcRegister ()
							{ return (_bcRegister); }
		MCHEmul::Register& dRegister ()
							{ return (internalRegister (_DREGISTER)); }
		MCHEmul::Register& eRegister ()
							{ return (internalRegister (_EREGISTER)); }
		MCHEmul::RefRegisters& deRegister ()
							{ return (_deRegister); }
		MCHEmul::Register& hRegister ()
							{ return (internalRegister (_HREGISTER)); }
		MCHEmul::Register& lRegister ()
							{ return (internalRegister (_LREGISTER)); }
		MCHEmul::RefRegisters& hlRegister ()
							{ return (_hlRegister); }

		// Accesing the alternative registers...
		MCHEmul::Register& apRegister ()
							{ return (internalRegister (_APREGISTER)); }
		MCHEmul::Register& fpRegister ()
							{ return (internalRegister (_FPREGISTER)); }
		MCHEmul::RefRegisters& afpRegister ()
							{ return (_afpRegister); }
		MCHEmul::Register& bpRegister ()
							{ return (internalRegister (_BPREGISTER)); }
		MCHEmul::Register& cpRegister ()
							{ return (internalRegister (_CPREGISTER)); }
		MCHEmul::RefRegisters& bcpRegister ()
							{ return (_bcpRegister); }
		MCHEmul::Register& dpRegister ()
							{ return (internalRegister (_DPREGISTER)); }
		MCHEmul::Register& epRegister ()
							{ return (internalRegister (_EPREGISTER)); }
		MCHEmul::RefRegisters& depRegister ()
							{ return (_depRegister); }
		MCHEmul::Register& hpRegister ()
							{ return (internalRegister (_HPREGISTER)); }
		MCHEmul::Register& lpRegister ()
							{ return (internalRegister (_LPREGISTER)); }
		MCHEmul::RefRegisters& hlpRegister ()
							{ return (_hlpRegister); }

		// Accesing the index registers...
		MCHEmul::Register& ixhRegister ()
							{ return (internalRegister (_IXHREGISTER)); }
		MCHEmul::Register& ixlRegister ()
							{ return (internalRegister (_IXLREGISTER)); }
		MCHEmul::RefRegisters& ixRegister ()
							{ return (_ixRegister); }
		MCHEmul::Register& iyhRegister ()
							{ return (internalRegister (_IYHREGISTER)); }
		MCHEmul::Register& iylRegister ()
							{ return (internalRegister (_IYLREGISTER)); }
		MCHEmul::RefRegisters& iyRegister ()
							{ return (_iyRegister); }

		// Accesing other registers used only in some computers...
		MCHEmul::Register& iRegister ()
							{ return (internalRegister (_IREGISTER)); }
		MCHEmul::Register& rRegister ()
							{ return (internalRegister (_RREGISTER)); }

		protected:
		MCHEmul::RefRegisters _afRegister;
		MCHEmul::RefRegisters _bcRegister;
		MCHEmul::RefRegisters _deRegister;
		MCHEmul::RefRegisters _hlRegister;
		MCHEmul::RefRegisters _afpRegister;
		MCHEmul::RefRegisters _bcpRegister;
		MCHEmul::RefRegisters _depRegister;
		MCHEmul::RefRegisters _hlpRegister;
		MCHEmul::RefRegisters _ixRegister;
		MCHEmul::RefRegisters _iyRegister;

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
