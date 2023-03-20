/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SIDRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: SID Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SIDREGISTERS__
#define __COMMODORE_SIDREGISTERS__

#include <CORE/incs.hpp>
#include <resid/sid.h>

namespace COMMODORE
{
	class SID;

	class SIDRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend SID;

		static const int _SIDREGS_SUBSET = 1001;

		SIDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
				: MCHEmul::ChipRegisters (_SIDREGS_SUBSET, ps, pp, a, s),
				  _resid_sid (nullptr)
							{ setClassName ("SIDRegisters"); }

		virtual size_t numberRegisters () const override
							{ return (0x20); }

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override
							{ MCHEmul::PhysicalStorageSubset::setValue (p, v);
							  if (_resid_sid != nullptr) // Can happen when emulation starts...
								  _resid_sid -> write ((RESID::reg8) (p % 0x20), (RESID::reg8) v.value ()); }
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (_lastValueRead = 
								(_resid_sid == nullptr) // It shouldn't happen ever but...
									? MCHEmul::PhysicalStorage::_DEFAULTVALUE
									: MCHEmul::UByte ((unsigned char) _resid_sid -> read ((RESID::reg8) (p % 0x20)))); }

		/** This methis is invoked from the SID emulation to set a reference to the RESID SID emulation. */
		void setRESID_SID (RESID::SID* sid)
							{ _resid_sid = sid; }

		/** Just to avoid unnecessary copies later. */
		mutable MCHEmul::UByte _lastValueRead;
		/** A reference to the RESID SID emulation has to be considered. */
		RESID::SID* _resid_sid;
	};
}

#endif
  
// End of the file
/*@}*/
