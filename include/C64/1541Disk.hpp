/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: 1541Disk.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/01/2026 \n
 *	Description: To emulate the behaviour of the 1541 Disk unit in a C64 (loading memory).
 *	Versions: 1.0 Initial
 */

#ifndef __C64_1541DISK__
#define __C64_1541DISK__

#include <COMMODORE/incs.hpp>
#include <C64/Memory.hpp>

namespace C64
{
	/** The disk working directly against the memory. \n
		The device number assigned by default is number 8, and the default ID = 103, but other can be created. \n
		Remember than in the Serial IO there might be many devices connected,
		but all of them must have both different ID and different device number. */
	class Disk1541Simulation final : public COMMODORE::Disk1540SeriesSimulation
	{
		public:
		static const int _DEFAULTID = 215;
		static const unsigned char _DEFAULTDEVICENUMBER = 8;
		static const std::vector <unsigned char> _POSSIBLEDEVICENUMBERS;

		/** To verify whether a device number is or not right. */
		static bool isDeviceNumberValid (unsigned char dN)
							{ return (std::find (_POSSIBLEDEVICENUMBERS.begin (),
												 _POSSIBLEDEVICENUMBERS.end (), dN) 
										!= _POSSIBLEDEVICENUMBERS.end ()); }

		Disk1541Simulation (int id = _DEFAULTID, unsigned char dN = _DEFAULTDEVICENUMBER);

		private:
		/** To load the info just only inthe RAM. */
		virtual void loadDataBlockInRAM (const MCHEmul::DataMemoryBlock& dB, MCHEmul::CPU* cpu) override
							{ static_cast <C64::Memory*> (cpu -> memoryRef ()) -> loadDataBlockInRAM (_data._data [actualBlock ()]); }

		private:
		/** The definition of the traps for this device. */
		static const Definition _DEFINITION;
	};
}

#endif
  
// End of the file
/*@}*/
