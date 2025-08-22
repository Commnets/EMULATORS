/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/06/2024 \n
 *	Description: To emulate the behaviour of the Datasette Unit connected to the ZX81.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_DATASETTE__
#define __ZX81_DATASETTE__

#include <SINCLAIR/incs.hpp>
#include <ZX81/Type.hpp>

namespace ZX81
{
	/** This type of datasette "injects" the infomation into the memory when it is simulated. 
		The way it is done, will depend on the type of structure prepared for that, 
		and also on the type of computer that is being simulated. \n
		No need of keys as the file that can be uploaded has only 1 data file inside. */
	class DatasetteInjection final : public MCHEmul::DatasettePeripheral
	{
		public:
		static const int _ID = 101;

		/** The parameters are the point in the execution where the code has to be injected, and
			the point where to return once the injection has been done. */
		DatasetteInjection (Type t);

		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** No data can be retrieved using this device. */
		virtual MCHEmul::FileData* retrieveData () const override
							{ return (nullptr); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugSimulation (MCHEmul::CPU* cpu);
		// -----

		private:
		Type _type;

		// Implementation...
		const MCHEmul::Trap _loadTrap;
	};
}

#endif
  
// End of the file
/*@}*/
