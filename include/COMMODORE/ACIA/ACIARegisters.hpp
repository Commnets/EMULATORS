/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: ACIARegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: ACIA Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_ACIAREGISTERS__
#define __COMMODORE_ACIAREGISTERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class ACIA;

	class ACIARegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend ACIA;

		static const int _ACIAREGS_SUBSET = 2000;

		ACIARegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x0f); }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues ();

		private:
		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
