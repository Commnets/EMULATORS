/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C1C2SelectorRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/03/2024 \n
 *	Description: @see C1C2Selector class.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C1C2SELECTORREGISTERS__
#define __C264_C1C2SELECTORREGISTERS__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C1C2Selector;

	class C1C2SelectorRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend C1C2Selector;

		static const int _C1C2SELECTORREGS_SUBSET = 2030;

		C1C2SelectorRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
			: MCHEmul::ChipRegisters (_C1C2SELECTORREGS_SUBSET, ps, pp, a, s),
			  _configurationChanged (false)
							{ initializeInternalValues (); }

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		// Mamaging the configuration...
		/** To know the configuration of the memory. */
		unsigned char memoryConfiguration () const
							{ return (_memoryConfiguration); }
		/** To know whether the configuration changed. */
		bool configurationChanged () const
							{ return (_configurationChanged); } // This method put the v alue back to false in the variable
		bool peekConfigurationChanged () const
							{ return (_configurationChanged.peekValue ()); } // This one doesn't affect the value...

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  * MEMORYCFG = Attribute: The configuration of the memory.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (MCHEmul::ChipRegisters::readValue (p)); } // Return always the value stored in the position...

		void initializeInternalValues ();

		private:
		/** To indicate the configuration of the memory. */
		unsigned char _memoryConfiguration;

		// Implementation
		/** When the configuration has been changed, 
			as any of the registers has been accessed. */
		MCHEmul::OBool _configurationChanged;
	};
}

#endif
  
// End of the file
/*@}*/
