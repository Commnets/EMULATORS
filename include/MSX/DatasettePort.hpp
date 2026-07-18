/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/07/2026 \n
 *	Description:	To emulate the behaviour of the Datasette Port.
 *				 	Usually the datasette Unit is connected here.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_DATASETTEPORT__
#define __MSX_DATASETTEPORT__

#include <CORE/incs.hpp>

namespace MSX
{
	/** This class represents the place a MSX Datasette connects. \n
		It is pretty much like the standard one, but assigned already a specific Id. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class DatasetteIOPort final : public MCHEmul::DatasetteIOPort
	{
		public:
		static const int _ID = 100;

		DatasetteIOPort ();

		virtual void linkToChips (const MCHEmul::Chips& c) override;

		private:
		MCHEmul::SoundChip* _sChip;
	};
}

#endif
  
// End of the file
/*@}*/
