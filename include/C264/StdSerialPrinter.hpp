/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: StdSerialPrinter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 06/03/2026 \n
 *	Description: To emulate the behaviour of the a standard serial printer in the C264.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_STANDARDSERIALPRINTER__
#define __C264_STANDARDSERIALPRINTER__

#include <COMMODORE/incs.hpp>

namespace C264
{
	/** The Standard Printer. By default it uses a default configuration. */
	class StandardSerialPrinterSimulation : public COMMODORE::SerialPrinterPeripheralSimulation
	{
		public:
		// Default values for the printer that can be changed when building instances...
		static const int _DEFAULTID = 205;
		static const unsigned char _DEFAULTDEVICENUMBER = (unsigned char) 0x04;
		static const std::vector <unsigned char> _POSSIBLEDEVICENUMBERS;

		/** To verify whether a device number is or not right. */
		static bool isDeviceNumberValid (unsigned char dN)
							{ return (std::find (_POSSIBLEDEVICENUMBERS.begin (),
												 _POSSIBLEDEVICENUMBERS.end (), dN) 
										!= _POSSIBLEDEVICENUMBERS.end ()); }

		StandardSerialPrinterSimulation (
			int id = _DEFAULTID, 
			unsigned char dN = _DEFAULTDEVICENUMBER, 
			MCHEmul::MatrixPrinterEmulation* mPE = 
				new COMMODORE::MPS801BasicMatrixPrinterEmulation);

		protected:
		/** The definition of the traps for this device. */
		static const Definition _DEFINITION;
	};
}

#endif
  
// End of the file
/*@}*/
