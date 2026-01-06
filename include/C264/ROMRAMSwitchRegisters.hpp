/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: ROMRAMSwitchRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/03/2024 \n
 *	Description: @see ROMRAMSwitch class.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_ROMRAMSWITCHREGISTERS__
#define __C264_ROMRAMSWITCHREGISTERS__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class ROMRAMSwitch;

	class ROMRAMSwitchRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend ROMRAMSwitch;

		static const int _ROMRAMSWITCHREGS_SUBSET = 2020;

		ROMRAMSwitchRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		// Mamaging the configuration...
		/** To know whether the ROM is or not active. */
		bool ROMActive () const
							{ return (_ROMActive); }
		/** To know whether the configuration changed. */
		bool configurationChanged () const
							{ return (_configurationChanged); } // This method put the v alue back to false in the variable
		bool peekConfigurationChanged () const
							{ return (_configurationChanged.peekValue ()); } // This one doesn't affect the value...

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  * CONFIGURATION		= Attribute: The configuration of the ROM latched. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (MCHEmul::ChipRegisters::readValue (p)); } // Return always the value stored in the position...

		void initializeInternalValues ();

		private:
		/** To indicate whether the ROM is or not active. */
		bool _ROMActive;

		// Implementation
		/** When the configuration has been changed, 
			as any of the registers has been accessed. */
		MCHEmul::OBool _configurationChanged;
	};
}

#endif
  
// End of the file
/*@}*/
