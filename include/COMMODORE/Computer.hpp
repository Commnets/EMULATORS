/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: Generic calss for all Commodore Computers.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_COMPUTER__
#define __COMMODORE_COMPUTER__

#include <CORE/incs.hpp>
#include <COMMODORE/VICI/VICI.hpp>
#include <COMMODORE/VICII/VICII.hpp>
#include <COMMODORE/VIA/VIA.hpp>
#include <COMMODORE/CIA/CIA.hpp>
#include <COMMODORE/SID/SID.hpp>
#include <COMMODORE/UserPort.hpp>
#include <COMMODORE/DatasettePort.hpp>
#include <COMMODORE/1530Datasette.hpp>

namespace COMMODORE
{
	/** Just to recognize a Commdore Computer. */
	class Computer : public MCHEmul::Computer
	{
		public:
		Computer (MCHEmul::CPU* cpu, 
				  const MCHEmul::Chips& c, 
				  MCHEmul::Memory* m, 
				  const MCHEmul::IODevices& d, 
				  unsigned int cs, 
				  const MCHEmul::Buses& bs,
				  const MCHEmul::Wires& ws,
				  const MCHEmul::Attributes& attrs)
			: MCHEmul::Computer (cpu, c, m, d, cs, bs, ws, attrs)
							{ }

		// To get direct access to the most important COMMODORE chips...
		// Not all these chips have to exist in any COMMODORE computer, so the answer can be nullptr.
		/** The VICI. */
		bool existsVICI () const
							{ return (_chips.find (COMMODORE::VICI::_ID) != _chips.end ()); }
		const COMMODORE::VICI* vicI () const
							{ return (existsVICI () 
								? dynamic_cast <const COMMODORE::VICI*> 
								  ((* _chips.find (COMMODORE::VICI::_ID)).second)
								: nullptr); }
		COMMODORE::VICI* vicI ()
							{ return (existsVICI () 
								? dynamic_cast <COMMODORE::VICI*> 
								  ((* _chips.find (COMMODORE::VICI::_ID)).second)
								: nullptr); }

		/** The VICII. */
		bool existsVICII () const
							{ return (_chips.find (COMMODORE::VICII::_ID) != _chips.end ()); }
		const COMMODORE::VICII* vicII () const
							{ return (existsVICII () 
								? dynamic_cast <const COMMODORE::VICII*> 
								  ((* _chips.find (COMMODORE::VICII::_ID)).second)
								: nullptr); }
		COMMODORE::VICII* vicII ()
							{ return (existsVICII () 
								? dynamic_cast <COMMODORE::VICII*> 
								  ((* _chips.find (COMMODORE::VICII::_ID)).second)
								: nullptr); }

		/** The VIA. */
		bool existsVIA () const
							{ return (_chips.find (COMMODORE::VIA::_ID) != _chips.end ()); }
		const COMMODORE::VIA* via () const
							{ return (existsVIA () 
								? dynamic_cast <const COMMODORE::VIA*> 
								  ((* _chips.find (COMMODORE::VIA::_ID)).second)
								: nullptr); }
		COMMODORE::VIA* via ()
							{ return (existsVIA ()
								? dynamic_cast <COMMODORE::VIA*> 
								  ((* _chips.find (COMMODORE::VIA::_ID)).second)
								: nullptr); }

		/** The CIA. */
		bool existsCIA () const
							{ return (_chips.find (COMMODORE::CIA::_ID) != _chips.end ()); }
		const COMMODORE::CIA* cia () const
							{ return (existsCIA () 
								? dynamic_cast <const COMMODORE::CIA*> 
								  ((* _chips.find (COMMODORE::CIA::_ID)).second)
								: nullptr); }
		COMMODORE::CIA* cia ()
							{ return (existsCIA ()
								? dynamic_cast <COMMODORE::CIA*> 
								  ((* _chips.find (COMMODORE::CIA::_ID)).second)
								: nullptr); }
		
		/** The SID. */
		bool existsSID () const
							{ return (_chips.find (COMMODORE::SID::_ID) != _chips.end ()); }
		const COMMODORE::SID* sid () const
							{ return (existsSID () 
								? dynamic_cast <const COMMODORE::SID*> 
								  ((*_chips.find (COMMODORE::SID::_ID)).second)
								: nullptr); }
		COMMODORE::SID* sid ()
							{ return (existsSID ()
								? dynamic_cast <COMMODORE::SID*> 
								  ((*_chips.find (COMMODORE::SID::_ID)).second)
								: nullptr); }

		/** The User IO Port. */
		bool existsUserIOPort () const
							{ return (_devices.find (COMMODORE::UserIOPort::_ID) != _devices.end ());	}
		const COMMODORE::UserIOPort* userIOPort () const
							{ return (existsUserIOPort () 
								? dynamic_cast <const COMMODORE::UserIOPort*> 
								  ((*_chips.find (COMMODORE::UserIOPort::_ID)).second)
								: nullptr); }
		COMMODORE::UserIOPort* userIOPort ()
							{ return (existsUserIOPort () 
								? dynamic_cast <COMMODORE::UserIOPort*> 
								  ((*_chips.find (COMMODORE::UserIOPort::_ID)).second)
								: nullptr); }

		/** The Datasette Port. */
		bool existsDatasettePort () const
							{ return (_devices.find (COMMODORE::DatasetteIOPort::_ID) != _devices.end ());	}
		const COMMODORE::DatasetteIOPort* userDatasettePort () const
							{ return (existsDatasettePort () 
								? dynamic_cast <const COMMODORE::DatasetteIOPort*> 
								  ((*_chips.find (COMMODORE::DatasetteIOPort::_ID)).second)
								: nullptr); }
		COMMODORE::DatasetteIOPort* userDatasettePort ()
							{ return (existsDatasettePort () 
								? dynamic_cast <COMMODORE::DatasetteIOPort*> 
								  ((*_chips.find (COMMODORE::DatasetteIOPort::_ID)).second)
								: nullptr); }
	};
}

#endif
  
// End of the file
/*@}*/
