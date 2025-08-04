/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 03/01/2024 \n
 *	Description: The DatasetteIOPOrt within the VIC20 is connected with some chips and memory!
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_DATASETTEPORT__
#define __VIC20_DATASETTEPORT__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA1;
	class VIA2;

	/** This class represents the DatasettePort. */
	class DatasetteIOPort final : public COMMODORE::DatasetteIOPort
	{
		public:
		DatasetteIOPort ()
			: COMMODORE::DatasetteIOPort (),
			  _via1 (nullptr), _via2 (nullptr)
							{ }

		/** The DatasettePort is connected with the VIA1 and the VIA2: \n
			In the VIA1:
			-----------
			It is controlled whether a button (that moves the motor) in the datasette is pressed,
			and when this happens the PB6 is reset. \n
			When the motor moves the CA2 line is moved to down (putting a value of 111 in their configuration),
			a change happens in this line and then a notification to the datsetteport for the motor to move happens too. \n
			In the VIA2:
			-----------
			Changes in the CA1 line indicates that the sound line has changed from 0 to 1 or viceversa. \n
			And The PB3 changes (from 0 to 1 or viceversa) send a notification to the datasetteport. */
		virtual void linkToChips (const MCHEmul::Chips& c) override;

		private:
		VIA1* _via1;
		VIA2* _via2;
	};
}

#endif
  
// End of the file
/*@}*/
