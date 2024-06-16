/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: 1530Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the 1530 Datasette Unit.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_1530DATASETTE__
#define __COMMODORE_1530DATASETTE__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** This unit was used in many COMMODORE models. \n
		Mainly all of them except COMMODORE16. \n
		The datasette moves between 300 and 311 bits per second, 
		depending on the speed of the computer connected. \n
		Every bit is broken in 4 cycles of information. \n
		Every two cycles makes a dipole, so a bit is always make up of 2 dipoles. \n
		A leader bit is made up of 4 0 cyles. \n
		A 0 bit is made up of 2 (1st dipole) 0 cycles and 2 (2nd dipole) 1 cycles. \n
		A 1 bit is made up of 2 (1st dipole) 1 cycles and 2 (2nd dipole) 0 cycles. \n
		A work market bit is make up of a word marker dipole and and one dipole 1. \n
		Every cycle lasts different depending whether it represents a leader bit (174 us), 
		a 0 data bit (169 us), a 1 data bit (247 us) or a word market bit (332 us).
		https://archive.org/details/COMPUTEs_VIC-20_and_Commodore_64_Tool_Kit_Kernal_1985_COMPUTE_Publications_a (page 275).
		However the one managing that stuff is the ROM emulation, the duty of this class is to have the right 
		speed of the motor at construction time.
		*/
	class Datasette1530 final : public MCHEmul::BasicDatasette
	{
		public:
		static const int _ID = 100;

		/** The running speed is still a parameter. */
		Datasette1530 (unsigned int rS);
	};

	/** This is identical to the previous one, 
		but with the motor running in parallel. The internal name of the lass is the same. */
	class Datasette1530P final : public MCHEmul::BasicDatasetteP
	{
		public:
		static const int _ID = 1000;

		/** The "speed of the motor" is still a parameter. 
			It is really the time in millisecond between two io actions in the datasette. */
		Datasette1530P (unsigned int mS);
	};
}

#endif
  
// End of the file
/*@}*/
