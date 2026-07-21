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
		It injects the data trapping the six standard cassette BIOS entry points. \n
		The entry points are the same in every MSX generation because later systems
		keep backwards compatibility with the original BIOS interface. */
	class DatasetteInjection final : public MCHEmul::DatasettePeripheral
	{
		public:
		static const int _ID = 101;

		static const MCHEmul::Attributes _ATTRIBUTES;

		DatasetteInjection ();

		virtual bool initialize () override;

		virtual bool connectData (MCHEmul::FileData* dt) override;
		virtual MCHEmul::FileData* emptyData () const override
							{ return (new MSX::CASFileData); }
		virtual MCHEmul::FileData* retrieveData () const override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The attributes and infostructures of the parent class, plus: \n
		  * MODE		= Attribute: Current cassette trap mode. \n
		  * BLOCKS		= Attribute: Number of CAS blocks. \n
		  * BLOCK		= Attribute: Current or next CAS block. \n
		  * OFFSET		= Attribute: Current byte offset. \n
		  * TRAPS		= InfoStructure: Standard BIOS traps handled.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** Invoked from simulate. Its result indicates whether the trap was handled,
			not whether the BIOS cassette operation succeeded. */
		bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu);
		// All of them invoked from executeTrap.
		/** It starts the cassette motor and synchronizes with the next CAS marker. */
		bool executeTAPIONTrap	(MCHEmul::CPU* cpu);
		/** Reads and returns a single byte in register A. */
		bool executeTAPINTrap	(MCHEmul::CPU* cpu);
		/** Immediately stops the reading process and turns off the cassette motor. */
		bool executeTAPIOFTrap	(MCHEmul::CPU* cpu);
		/** Starts the motor in record mode and creates a new CAS block. */
		bool executeTAPOONTrap	(MCHEmul::CPU* cpu);
		/** Writes the byte in register A to the current CAS block. */
		bool executeTAPOUTTrap	(MCHEmul::CPU* cpu);
		/** Stops the recording process and turns off the cassette motor. */
		bool executeTAPOOFTrap	(MCHEmul::CPU* cpu);

		/** Sets Carry according to the BIOS cassette success convention. */
		void setCassetteError (MCHEmul::CPU* cpu, bool error);

		// -----
		// Different debug methods to simplify the internal code
		// and to make simpler the modification in case it is needed...
		/** Debug special situations...
			Take care using these instructions: _deepDebugFile could be == nullptr. */
		void debugStatus (const std::string& where, MCHEmul::CPU* cpu);
		void debugErrorTrap ();
		void debugNothingToRead ();
		// -----

		private:
		static const int _TAPIONTRAP	= 0;
		static const int _TAPINTRAP		= 1;
		static const int _TAPIOFTRAP	= 2;
		static const int _TAPOONTRAP	= 3;
		static const int _TAPOUTRAP		= 4;
		static const int _TAPOOFTRAP	= 5;

		enum class Mode
		{
			_IDLE = 0,
			_READING,
			_WRITING
		};

		const MCHEmul::Traps _traps;

		Mode _mode;
		size_t _blockCounter;
		size_t _byteCounter;
	};
}

#endif
  
// End of the file
/*@}*/
