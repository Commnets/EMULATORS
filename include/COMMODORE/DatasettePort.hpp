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

		DatasetteIOPort ();

		// Managing the different pins...
		/** D-4 READ. */
		bool pinD4 () const;
		/** E-5 WRTE. */
		void pinE5 (bool d);
		/** F-6 SENSE. */
		bool pintF6 () const;

		/** It verifies before adding it that whether the peripherial is somtehing compatible. */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;

		private:
		// Implementation...
		DatasettePeripheral* _datasette;
	};
}

#endif
  
// End of the file
/*@}*/
