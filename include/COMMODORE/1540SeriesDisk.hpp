/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: 1540SeriesDisk.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/01/2026 \n
 *	Description: To emulate the behaviour of the 1540 Disk Series Unit (1540,1541,...) \n
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_1540SERIESDISK__
#define __COMMODORE_1540SERIESDISK__

#include <CORE/incs.hpp>
#include <COMMODORE/FileIO.hpp>
#include <COMMODORE/SerialIOPeripherals.hpp>

namespace COMMODORE
{
	/** To emulate the behaviour of the 1540 Series Disk Unit (1540,1541,...). \n
		It is mainly used to load data into the memory of the computer. */
	class Disk1540SeriesSimulation : public SerialIOPeripheralSimulation
	{
		public:
		Disk1540SeriesSimulation (int id, unsigned char dN, 
			const SerialIOPeripheralSimulation::Definition& dt);

		virtual bool initialize () override
							{ return (true); }
		virtual bool finalize () override
							{ return (true); }

		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** No data can be retrieved using this device. */
		virtual MCHEmul::FileData* retrieveData () const override
							{ return (nullptr); }

		/** 
		  * The info included is the one of the parent, plus: \n
		  * DATA	= InfoStructuer: The data kept in the disk. Just the first 256.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual unsigned char sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		virtual unsigned char receiveByte (MCHEmul::CPU* cpu, MCHEmul::UByte& b) override;

		/** To load the program into the memory. \n
			In some COMMODORE computers (like C64) the whole RAM memory is not directly accesible. */
		virtual void loadDataBlockInRAM (const MCHEmul::DataMemoryBlock& dB, MCHEmul::CPU* cpu)
							{ cpu -> memoryRef () -> put (_data._data [actualBlock ()]); }

		/** To get the actual block depending on the actual track and actual sector. \n
			Bear in mind that not all tracks have the same number of sectors! */
		size_t actualBlock () const;

		protected:
		MCHEmul::ExtendedDataMemoryBlocks _data;

		// Immplementation
		/** Where the disk is now. */
		mutable size_t _actualTrack; 
		mutable size_t _actualSector;
	};
}

#endif
  
// End of the file
/*@}*/
