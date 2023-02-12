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

		/** Different events. */
		static const unsigned int _READ0			= 200;
		static const unsigned int _READ1			= 201;
		static const unsigned int _WRITE0			= 202;
		static const unsigned int _WRITE1			= 203;
		static const unsigned int _NOKEYPRESSED		= 204;
		static const unsigned int _KEYPRESSED		= 205;
		static const unsigned int _MOTORRUNNING		= 206;
		static const unsigned int _MOTORSTOPPED		= 207;

		DatasetteIOPort ();

		/** It verifies that the peripheral to add is compatible (= DatasettePeripheral). */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		protected:
		const DatasettePeripheral* datasette () const
							{ return (_datasette); }
		DatasettePeripheral* datasette ()
							{ return (_datasette); }

		protected:
		bool _lastValueRead;
		bool _lastMotorOff;
		bool _lastNoKeyPressed;

		// Implementation...
		DatasettePeripheral* _datasette;
	};
}

#endif
  
// End of the file
/*@}*/
