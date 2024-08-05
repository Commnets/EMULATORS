/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Cartridge.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
		enum class Type
		{
			_NOTDEFINED = -1,
			_GENERIC = 0,
			_ACTIONREPLAY = 1,
			_KCS = 2,
			_FINALCARTRIDGEIII = 3,
			_SIMONSBASIC = 4,
			_OCEANTYPE1 = 5,
			_EXPERTCARTRIDGE = 6,
			_FUNPLAY = 7,
			_SUPERGAMES = 8,
			_ATOMICPOWER = 9,
			_EPYXFASTLOAD = 10,
			_WESTERMAMNLEARNING = 11,
			_REXUTILITY = 12,
			_FINALCARTRIDGEI = 13,
			_MAGICFORMEL = 14,
			_C64GAMESYSTEM = 15,
			_WARPSPEED = 16,
			_DINAMIC = 17,
			_ZAXXON = 18,
			_MASGICDESK = 19,
			_SUPERSNAPSHOT = 20,
			_COMAL80 = 21,
			_STRUCTUREDBASIC = 22,
			_ROSS = 23,
			_DELAEP64 = 24,
			_DELAEP7x8 = 25,
			_DELAEP256 = 26,
			_REXEP256 = 27,
			_MIKROASSEMBLER = 28,
			_FINALCARTRIDGEPLUS = 29,
			_ACTIONREPLAY4 = 30,
			_STARDOS = 31,
			_EASYFLASH = 32,
			_EASYFLASHBANK4 = 33,
			_CAPTURE = 34,
			_ACTIONREPLAY3 = 35,
			_RETROREPLAY = 36,
			_MMC64 = 37,
			_MMCREPLAY = 38,
			_IDE64 = 39,
			_SUPERSNAPSHOT4 = 40,
			_IEEE488 = 41,
			_GAMEKILLER = 42,
			_PROPHET64 = 43,
			_EXOS = 44,
			_FREEZEFRAME = 45,
			_FREEZEMACHINE = 46,
			_SNAPSHOT64 = 47,
			_SUPEREXPLODE5 = 48,
			_MAGICVOICE = 49,
			_ACTIONREPLAY2 = 50,
			_MACH5 = 51,
			_DIASHOWMAKER = 52,
			_PAGEFOX = 53,
			_KIGSOFT = 54,
			_SILVERROCK128CARTRIDGE = 55,
			_FORMEL64 = 56,
			_RGCD = 57,
			_RRNETMK3 = 58,
			_EASYCALC = 59,
			_GMOD2 = 60,
			_MAXBASIC = 61,
			_GMOD3 = 62,
			_ZIPPCODE48 = 63,
			_BLACKBOXV8 = 64,
			_BLACKBOXV3 = 65,
			_BLACKBOXV4 = 66,
			_REXRAMFLOPPY = 67,
			_BISPLUS = 68,
			_SDBOX = 69,
			_MULTIMAX = 70,
			_BLACKBOXV9 = 71,
			_KENELHOSTADAPTER = 72,
			_RAMLINK = 73,
			_HERO = 74,
			_IEEEFLASH64 = 75,
			_TURTLEGPRAPHICSII = 76,
			_FREEZEFRAMEMK2 = 77,
			_PARTNET64 = 78
		};

		static const int _ID = 203;

		Cartridge ();

		void configureMemoryStructure (bool romL, bool romH1, bool romH2CPU, bool romH2VIC);

		/** Only the PINS 8 and 9 are used
			No other functionlaity has been implemented so far. */
		virtual bool PIN_UP (unsigned char nP) const override;

		/** To know the type of cartridge, if any. */
		Type type () const
							{ return (data ()._data.empty () 
								? Type::_NOTDEFINED 
								: (Type) (std::atoi (((*_data._attributes.find ("TYPE")).second).c_str ()))); }
		/** To know whether it is or not a ULTIMAX cartridge. */
		bool ultimax () const
							{ return (_ultimax); }

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
		void dumpDataInto (C64::Memory* m, MCHEmul::MemoryView* cV, MCHEmul::MemoryView* vV);

		private:
		bool dataDumped () const
							{ bool r = _dataDumped; _dataDumped = false; return (r); }

		/** To clean up the memory used. */
		void cleanUpAdditionalSubsets ();

		private:
		/** When the cartridge is ultimax. */
		bool _ultimax;

		// Implementation
		mutable bool _dataDumped;

		/** When inserting a cartridge the structure of the memory is changed,
			and additional subsets of memory are added. \n
			Other way around when the cartridge is unplugged. */
		C64::Memory* _memoryRef;
		MCHEmul::MemoryView* _memoryCPUView;
		MCHEmul::MemoryView* _memoryVICIIView;
		MCHEmul::PhysicalStorages _storages;
		MCHEmul::PhysicalStorageSubsets _cpuSubsets;
		MCHEmul::PhysicalStorageSubsets _viciiSubsets;
		/** The physical storage. */
		static const int _EXPANSIONROMBASE			= 4000; // The different chips of memory are created using this base...
		static const int _EXPANSIONROMBASE_SUBSET	= 4000; // For the CPU View...
		static const int _EXPANSIONROMBASEI_SUBSET	= 5000; // For the VICII view...
	};
}

#endif
  
// End of the file
/*@}*/
