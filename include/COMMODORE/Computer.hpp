/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: Generic calss for all Commodore Computers.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_COMPUTER__
#define __COMMODORE_COMPUTER__

#include <CORE/incs.hpp>
#include <COMMODORE/TED/TED.hpp>
#include <COMMODORE/VICI/VICI.hpp>
#include <COMMODORE/VICII/VICII.hpp>
#include <COMMODORE/ACIA/ACIA.hpp>
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
		/** The TED. */
		bool existsTED () const
							{ return (_chips.find (TED::_ID) != _chips.end ()); }
		const TED* ted () const
							{ return (existsVICI () 
								? dynamic_cast <const TED*> 
								  ((* _chips.find (TED::_ID)).second)
								: nullptr); }
		TED* ted ()
							{ return (existsTED () 
								? dynamic_cast <TED*> 
								  ((* _chips.find (TED::_ID)).second)
								: nullptr); }

		// Not all these chips have to exist in any COMMODORE computer, so the answer can be nullptr.
		/** The VICI. */
		bool existsVICI () const
							{ return (_chips.find (VICI::_ID) != _chips.end ()); }
		const VICI* vicI () const
							{ return (existsVICI () 
								? dynamic_cast <const VICI*> 
								  ((* _chips.find (VICI::_ID)).second)
								: nullptr); }
		VICI* vicI ()
							{ return (existsVICI () 
								? dynamic_cast <VICI*> 
								  ((* _chips.find (VICI::_ID)).second)
								: nullptr); }

		/** The VICII. */
		bool existsVICII () const
							{ return (_chips.find (VICII::_ID) != _chips.end ()); }
		const VICII* vicII () const
							{ return (existsVICII () 
								? dynamic_cast <const VICII*> 
								  ((* _chips.find (VICII::_ID)).second)
								: nullptr); }
		VICII* vicII ()
							{ return (existsVICII () 
								? dynamic_cast <VICII*> 
								  ((* _chips.find (VICII::_ID)).second)
								: nullptr); }

		/** The ACIA. */
		bool existsACIA () const
							{ return (_chips.find (ACIA::_ID) != _chips.end ()); }
		const ACIA* acia () const
							{ return (existsACIA () 
								? dynamic_cast <const ACIA*> 
								  ((* _chips.find (ACIA::_ID)).second)
								: nullptr); }
		ACIA* acia ()
							{ return (existsACIA ()
								? dynamic_cast <ACIA*> 
								  ((* _chips.find (ACIA::_ID)).second)
								: nullptr); }

		/** The VIA. */
		bool existsVIA () const
							{ return (_chips.find (VIA::_ID) != _chips.end ()); }
		const VIA* via () const
							{ return (existsVIA () 
								? dynamic_cast <const VIA*> 
								  ((* _chips.find (VIA::_ID)).second)
								: nullptr); }
		VIA* via ()
							{ return (existsVIA ()
								? dynamic_cast <VIA*> 
								  ((* _chips.find (VIA::_ID)).second)
								: nullptr); }

		/** The CIA. */
		bool existsCIA () const
							{ return (_chips.find (CIA::_ID) != _chips.end ()); }
		const CIA* cia () const
							{ return (existsCIA () 
								? dynamic_cast <const CIA*> 
								  ((* _chips.find (CIA::_ID)).second)
								: nullptr); }
		CIA* cia ()
							{ return (existsCIA ()
								? dynamic_cast <CIA*> 
								  ((* _chips.find (CIA::_ID)).second)
								: nullptr); }
		
		/** The SID. */
		bool existsSID () const
							{ return (_chips.find (SID::_ID) != _chips.end ()); }
		const SID* sid () const
							{ return (existsSID () 
								? dynamic_cast <const SID*> 
								  ((*_chips.find (SID::_ID)).second)
								: nullptr); }
		SID* sid ()
							{ return (existsSID ()
								? dynamic_cast <SID*> 
								  ((*_chips.find (SID::_ID)).second)
								: nullptr); }

		/** The User IO Port. */
		bool existsUserIOPort () const
							{ return (_devices.find (UserIOPort::_ID) != _devices.end ());	}
		const UserIOPort* userIOPort () const
							{ return (existsUserIOPort () 
								? dynamic_cast <const UserIOPort*> 
								  ((*_chips.find (UserIOPort::_ID)).second)
								: nullptr); }
		UserIOPort* userIOPort ()
							{ return (existsUserIOPort () 
								? dynamic_cast <UserIOPort*> 
								  ((*_chips.find (UserIOPort::_ID)).second)
								: nullptr); }

		/** The Datasette Port. */
		bool existsDatasettePort () const
							{ return (_devices.find (DatasetteIOPort::_ID) != _devices.end ());	}
		const DatasetteIOPort* userDatasettePort () const
							{ return (existsDatasettePort () 
								? dynamic_cast <const DatasetteIOPort*> 
								  ((*_chips.find (DatasetteIOPort::_ID)).second)
								: nullptr); }
		DatasetteIOPort* userDatasettePort ()
							{ return (existsDatasettePort () 
								? dynamic_cast <DatasetteIOPort*> 
								  ((*_chips.find (DatasetteIOPort::_ID)).second)
								: nullptr); }
	};
}

#endif
  
// End of the file
/*@}*/
