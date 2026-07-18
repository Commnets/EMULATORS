/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/07/2026 \n
 *	Description: To emulate the behaviour of the Datasette Unit in a MSX machine.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_DATASETTE__
#define __MSX_DATASETTE__

#include <CORE/incs.hpp>
#include <MSX/FileIO.hpp>

namespace MSX
{
	/** This class emulates a datasette in a MSX system. \n
		It injects the data into the memory trapping several positions in KERNEL. \n
		The routines trapped are the same whether the system is a MSX1.0 or a MSX2.0 or	other. \n
		MSX system was designed to suppord always backward compatibility. */
	class DatasetteInjection final : public MCHEmul::StandardDatasette
	{
		public:
		static const int _ID = 101;

		static const MCHEmul::Attributes _ATTRIBUTES;

		DatasetteInjection ();

		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** No data can be retrieved using this device. */
		virtual MCHEmul::FileData* retrieveData () const override
							{ return (nullptr); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** Invoked from simulate. */
		bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu);
		// All of them invoked from executeTrap.
		// The trap functions are always the same whatever the MSX system version is.
		/** It starts the cassette motor and analyzes 
			the incoming audio until it synchronizes with the read pilot tone */
		bool executeTAPIONTrap	(MCHEmul::CPU* cpu);
		/** Reads the tape frequencies to decode and return a single byte in register A. */
		bool executeTAPINTrap	(MCHEmul::CPU* cpu);
		/** Immediately stops the reading process and turns off the cassette motor. */
		bool executeTAPIOFTrap	(MCHEmul::CPU* cpu);
		/** Start the motor in record mode and generate the 
			synchronization pilot tone on the tape for a few seconds. */
		bool executeTAPOONTrap	(MCHEmul::CPU* cpu);
		/** Writes the byte in register A to the tape as a series of frequencies. */
		bool executeTAPOUTTrap	(MCHEmul::CPU* cpu);
		/** It stops the recording process, 
			generates the final closing bit, and turns off the cassette motor. */
		bool executeTAPOOFrap	(MCHEmul::CPU* cpu);

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugStatus (const std::string& where, MCHEmul::CPU* cpu);
		void debugErrorTrap ();
		void debugNothingToRead ();
		// -----

		private:
		// Use these following ids to identify the two traps that this class
		// understood by default...
		static const int _TAPIONTRAP	= 0;
		static const int _TAPINTRAP		= 1;
		static const int _TAPIOFTRAP	= 2;
		static const int _TAPOONTRAP	= 3;
		static const int _TAPOUTRAP		= 4;
		static const int _TAPOOFTRAP	= 5;

		const MCHEmul::Traps _traps;
	};
}

#endif
  
// End of the file
/*@}*/
