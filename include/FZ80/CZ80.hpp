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
