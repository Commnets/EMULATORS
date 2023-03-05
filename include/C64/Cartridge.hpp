/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Cartridge.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 27/01/2023 \n
 *	Description: To emulate the behaviour of a cartridge in the C64 computer
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CARTRIDGE__
#define __C64_CARTRIDGE__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class Memory;

	/** Ultramax Cartridge is not still supported. */
	class Cartridge final : public COMMODORE::ExpansionPeripheral
	{
		public:
		static const int _ID = 200;

		Cartridge ();

		/** Only the PINS 8 and 9 are used
			No other functionlaity has been implemented so far. */
		virtual bool PIN_UP (unsigned char nP) const;

		virtual bool connectData (MCHEmul::FileData* dt) override;

		virtual bool initialize () override
							{ return (true); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/** To dump the data of the cartridge into the memory. \n
			There might be up to 3 different zones of memory possible, that will be filled up
			depending on the configuration of the cartridge. */
		void dumpDataInto (MCHEmul::PhysicalStorageSubset* eL,
						   MCHEmul::PhysicalStorageSubset* eH1,
						   MCHEmul::PhysicalStorageSubset* eH2);

		private:
		bool dataDumped () const
							{ bool r = _dataDumped; _dataDumped = false; return (r); }

		private:
		// Implementation
		mutable bool _dataDumped;
	};
}

#endif
  
// End of the file
/*@}*/
