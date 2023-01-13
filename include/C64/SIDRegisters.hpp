/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: SIDRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: C64 SID Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SIDREGISTERS__
#define __C64_SIDREGISTERS__

#include <CORE/incs.hpp>

namespace C64
{
	class SID;

	class SIDRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend SID;

		SIDRegisters (int id, MCHEmul::PhysicalStorage* pS);

		virtual size_t numberRegisters () const override
							{ return (0x20); }

		virtual void initialize () override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;

		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		void initializeInternalValues ();

		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
