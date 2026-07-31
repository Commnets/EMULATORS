/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: To emulate the behaviour of the Datasette Unit connected to the ZXSpectrum.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_DATASETTE__
#define __ZXSPECTRUM_DATASETTE__

#include <SINCLAIR/incs.hpp>
#include <ZXSpectrum/Type.hpp>
#include <ZXSpectrum//FileIO.hpp>
#include <FZ80/CZ80.hpp>

namespace ZXSPECTRUM
{
	/** This type of datasette "injects" the infomation into the memory when it is simulated. 
		The way it is done, will depend on the type of structure prepared for that, 
		and also on the type of computer that is being simulated. \n
		No need of keys as the file that can be uploaded has only 1 data file inside. */
	class DatasetteInjection final : public MCHEmul::DatasettePeripheral
	{
		public:
		static const int _ID = 101;

		static const MCHEmul::Attributes _ATTRIBUTES;

		/** The commands accepted by this peripheral. \n
			They refer mainly to keys that can be pressed.
			Combinations are not possible. */
		static const int _KEYFOWARD = 1; // Move the block to read forward (until the maximum)
		static const int _KEYREWIND = 2; // Move the block to read backward (until the minimum)
		static const int _KEYEJECT  = 4; // To clean up the data loaded, or to simulate a new casette is inserted...

		/** The parameters are the point in the execution where the code has to be injected, and
			the point where to return once the injection has been done. */
		DatasetteInjection (Type t);

		/** The datasette admits TAP file format but also (not in all circunstances) the TZX format. */
		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** No data can be retrieved using this device. */
		virtual MCHEmul::FileData* retrieveData () const override
							{ return (nullptr); }

		virtual bool executeCommand (int cId, const MCHEmul::Strings& prms) override;
		virtual MCHEmul::Strings commandDescriptions () const override
							{ return (MCHEmul::Strings (
								{ "1:FORWARD", "2:REWIND", "4:EJECT (and clear data)" })); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		enum class TrapResult
		{
			_NOTAPPLIED,
			_BLOCKCONSUMED
		};

		/** To validate that the ROM code around the trap and its return point
			matches the standard 48K loading routine. */
		bool validLoadTrapContext (MCHEmul::CPU* cpu) const;

		/** Pure trap simulation. \n
			On exit: \n
			A	= calculated parity byte if parity checked, else 0. \n
			F	= flags resulting from the parity check, or carry reset on error. \n
			B	= 0xB0 when the parity byte has been checked. \n
			C	= 0x01 when a block has been processed. \n
			DE	= decremented by number of bytes loaded or verified. \n
			H	= calculated parity byte. \n
			L	= last byte loaded or verified, or the block flag if none. \n
			IX	= incremented by number of bytes loaded or verified. \n
			AF'	= 0x0145 after processing a non-empty request. \n
			R and the other registers are unchanged. \n
			The result distinguishes an unsupported trap context from a block
			that was consumed, including normal tape errors. */
		TrapResult simulateTrap (MCHEmul::CPU* cpu);

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugSimulation (MCHEmul::CPU* cpu);
		void debugStatus (const std::string& where, FZ80::CZ80* cpu);
		void debugTrapNotApplied (const std::string& reason);
		void debugNothingToRead ();
		// -----

		private:
		Type _type;

		// Implementation...
		const MCHEmul::Trap _loadTrap;
		size_t _blockRead;
	};
}

#endif
  
// End of the file
/*@}*/
