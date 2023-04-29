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
#include <COMMODORE/SIDLibWrapper.hpp>

namespace COMMODORE
{
	class SID;

	class SIDRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend SID;

		static const int _SIDREGS_SUBSET = 1050;

		SIDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x20); }

		virtual void initialize () override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		/** To initialize the internal values. */
		void initializeInternalValues ();

		/** This method is to establish the wrapper used. */
		void setSIDLibWrapper (SIDLibWrapper* w)
							{ _sidWrapper = w; }

		/** Just to avoid unnecessary copies later. */
		mutable MCHEmul::UByte _lastValueRead;
		/** A reference to SID Lib Wrapper. */
		SIDLibWrapper* _sidWrapper;
	};
}

#endif
  
// End of the file
/*@}*/
