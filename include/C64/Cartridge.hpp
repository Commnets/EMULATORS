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
	class MemoryView;

	/** Ultramax Cartridge is not still supported. */
	class Cartridge final : public COMMODORE::ExpansionPeripheral
	{
		public:
		static const int _ID = 200;

		Cartridge ();

		void configureMemoryStructure (bool ROML, bool ROMH1, bool ROMH2);

		/** Only the PINS 8 and 9 are used
			No other functionlaity has been implemented so far. */
		virtual bool PIN_UP (unsigned char nP) const;

		/** When data os connected attending to the type of cartridge (info in the data received)
			the additional subset of information is created. */
		virtual bool connectData (MCHEmul::FileData* dt) override;

		virtual bool initialize () override
							{ return (true); }
		/** When finish, the additional memory created has to taken off from the memory and destroyed. */
		virtual bool finalize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/** To dump the data of the cartridge into the memory. \n
			The configuartion of the memory is changed. */
		void dumpDataInto (C64::Memory* m, MCHEmul::MemoryView* mV);

		private:
		bool dataDumped () const
							{ bool r = _dataDumped; _dataDumped = false; return (r); }

		/** To clean up the memory used. */
		void cleanUpAdditionalSubsets ();

		private:
		// Implementation
		mutable bool _dataDumped;

		/** When inserting a cartridge the structure of the memory is changed,
			and additional subsets of memory are added. \n
			Other way around when the cartridge is unplugged. */
		C64::Memory* _memoryRef;
		MCHEmul::MemoryView* _memoryView;
		MCHEmul::PhysicalStorages _storages;
		MCHEmul::PhysicalStorageSubsets _subsets;
		/** The physical storage. */
		static const int _EXPANSIONROMBASE			= 2000; //The different chips of memory are created using this base...
	};
}

#endif
  
// End of the file
/*@}*/
