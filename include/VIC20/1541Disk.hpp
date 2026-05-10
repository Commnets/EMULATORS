/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: 1541Disk.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 08/05/2026 \n
 *	Description: To emulate the behaviour of the 1541 Disk unit in a VIC20 (loading memory).
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_1541DISK__
#define __VIC20_1541DISK__

#include <COMMODORE/incs.hpp>
#include <VIC20/Memory.hpp>

namespace VIC20
{
	/** The disk working directly against the memory. \n
		The device number assigned by default is number 8, and the default ID = 215, but other can be created. \n
		Remember than in the Serial IO there might be many devices connected,
		but all of them must have both different ID and different device number. */
	class Disk1541Simulation final : public COMMODORE::Disk1540SeriesSimulation
	{
		public:
		static const int _DEFAULTID = 215;
		static const unsigned char _DEFAULTDEVICENUMBER = 8;
		/** There are 4 different possibilities. 
			There were several versions of the Disk1541: 
			Disk1540 (VIC20), Disk1541 (C64), Disk1571 (double side C128&C64), Disk1570 (C128&C64) and Disk1581 (C128&C64).\n
			It was possible to seelct the number of the unit using switched in the back part of the unit, 
			but just only in 1571 and 1581. However that possibility has been included in the configuration. */
		static const std::vector <unsigned char> _POSSIBLEDEVICENUMBERS;

		/** To verify whether a device number is or not right. */
		static bool isDeviceNumberValid (unsigned char dN)
							{ return (std::find (_POSSIBLEDEVICENUMBERS.begin (),
												 _POSSIBLEDEVICENUMBERS.end (), dN) 
										!= _POSSIBLEDEVICENUMBERS.end ()); }

		Disk1541Simulation (
			const MCHEmul::ASCIIToCodeConverter* cvs, 
			int id = _DEFAULTID, unsigned char dN = _DEFAULTDEVICENUMBER);

		private:
		/** The definition of the traps for this device. */
		static const Definition _DEFINITION;
	};
}

#endif
  
// End of the file
/*@}*/
