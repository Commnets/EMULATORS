/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description:	To emulate the behaviour of the Datasette Port.
 *					It is a common device in most of the 80's computers e.g.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_DATASETTEPORT__
#define __MCHEMUL_DATASETTEPORT__

#include <CORE/IO.hpp>

namespace MCHEmul
{
	class DatasettePeripheral;

	/** This class represents the DatasettePort. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class DatasetteIOPort : public IODevice
	{
		public:
		// Different events.
		/** When read data from a datasette this event is sent. */
		static const unsigned int _READ				= 120;
		/** When this event is received a data is written in the peripheral connected. */
		static const unsigned int _WRITE			= 121;
		/** Managing the cassette the keys are simulated usind events. */
		static const unsigned int _NOKEYPRESSED		= 122;
		static const unsigned int _KEYPRESSED		= 123;
		/** With this events the motor can be activated or desactivated. */
		static const unsigned int _MOTORRUNNING		= 124;
		static const unsigned int _MOTORSTOPPED		= 125;

		DatasetteIOPort (int id, const Attributes& attrs = { })
			: IODevice (IODevice::Type::_INPUTOUTPUT, id, attrs),
			  _datasette (nullptr),
			  _lastCPUCycles (0)
							{ }

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
		// Implementation
		/** The CPU cycles when the simulate method was invoked the last time. */
		unsigned int _lastCPUCycles;
	};
}

#endif
  
// End of the file
/*@}*/
