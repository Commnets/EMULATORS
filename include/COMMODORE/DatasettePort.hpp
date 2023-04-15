/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description:	To emulate the behaviour of the Datasette Port.
 *				 	Usually the 1530 Unit (or 1531) is connected here.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_DATASETTEPORT__
#define __COMMODORE_DATASETTEPORT__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class DatasettePeripheral;

	/** This class represents the UserIOPort. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class DatasetteIOPort : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 100;

		// Different events.
		/** When read data from a datasette this event is sent. */
		static const unsigned int _READ				= 250;
		/** When this event is received a data is written in the peripheral connected. */
		static const unsigned int _WRITE			= 251;
		/** Managing the cassette the keys are simulated usind events. */
		static const unsigned int _NOKEYPRESSED		= 254;
		static const unsigned int _KEYPRESSED		= 255;
		/** With this events the motor can be activated or desactivated. */
		static const unsigned int _MOTORRUNNING		= 256;
		static const unsigned int _MOTORSTOPPED		= 257;

		DatasetteIOPort ();

		/** To get the casette connect if any. */
		const DatasettePeripheral* datasette () const
							{ return (_datasette); }
		DatasettePeripheral* datasette ()
							{ return (_datasette); }

		/** It verifies that the peripheral to add is compatible (= DatasettePeripheral). */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		protected:
		/** The casette connected. */
		DatasettePeripheral* _datasette;
	};
}

#endif
  
// End of the file
/*@}*/
