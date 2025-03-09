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

namespace ZXSPECTRUM
{
	/** With the io activities running "linked" with the CPU. */
	class Datasette final : public MCHEmul::BasicDatasette
	{
		public:
		static const int _ID = 100;

		Datasette (unsigned int rS);
	};

	/** With the io activities running "in paralell" with the CPU. */
	class DatasetteP final : public MCHEmul::BasicDatasetteP
	{
		public:
		static const int _ID = 1000;

		DatasetteP (unsigned int rS);
	};

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

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual bool connectData (MCHEmul::FileData* dt) override;

		private:
		/** Pure trap simulation. \n
			On exit: \n
			A	= calculated parity byte if parity checked, else 0 (CHECKME). \n
			F	= if parity checked, all flags are modified
				  else carry only is modified (FIXME).
			B	= 0xB0 (success) or 0x00 (failure). \n
			C	= 0x01 (confirmed), 0x21, 0xFE or 0xDE (CHECKME). ºn
			DE	= decremented by number of bytes loaded or verified. \n
			H	= calculated parity byte or undefined. \n
			L	= last byte read, or 1 if none. \n
			IX	= incremented by number of bytes loaded or verified. \n
			A'	= unchanged on error + no flag byte, else 0x01. \n
			F'	= 0x01      on error + no flag byte, else 0x45. \n
			R	= no point in altering it :-). \n
			Other registers unchanged. **/
		bool simulateTrap (MCHEmul::CPU* cpu);
		bool simulateTrap_old (MCHEmul::CPU* cpu);

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugStatus (const std::string& where, FZ80::CZ80* cpu);
		void debugSimulation (MCHEmul::CPU* cpu);
		void debugErrorTrap ();
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
