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
	/** With the io activities running "linked" with the CPU. */
	class Datasette final : public MCHEmul::StandardDatasette
	{
		public:

		/** In this implementation every bit in every byte represents a signal in the EAR (bit 6 port 0xfe).
			Every x TStates (cyles defined as parameter at construction time or modfied later) a bit is read
			The last byte has only y bits to be read. 
			From that point onwars everything read is 0 for a number of miliseconds.
			All these parameters are received as parameter at construction time, but they can be redefined. */
		class DirectRecordingImplementation final : 
			public MCHEmul::StandardDatasette::Implementation
		{
			public:
			DirectRecordingImplementation (unsigned int cR, unsigned char nbl)
				: MCHEmul::StandardDatasette::Implementation (),
				  _cyclesToAction (cR), _bitsLastByte (nbl),
				  _byteInfo (MCHEmul::UByte::_0), _bitManaged (7)
							{ }

			/** Every block the cycles needed before reading the next element could vary. */
			void setParameters (unsigned int cR, unsigned char nbl)
							{ _cyclesToAction = cR; _bitsLastByte = nbl; }

			virtual std::tuple <bool, bool, bool> timeToReadValue (unsigned int cC) override;
			virtual void whenValueRead (unsigned int cC, const MCHEmul::UByte& v) override
							{ _byteInfo = v; _bitManaged = 7; }
			// The parameters to sample might vary...
			virtual void whenReadingNewBlock (const MCHEmul::DataMemoryBlock& dB) override;

			virtual std::tuple <bool, MCHEmul::UByte> timeToWriteValue (unsigned int cC, 
				bool vc, unsigned int ccvc, bool vs) override;
			virtual void whenValueWritten (unsigned int cC, const MCHEmul::UByte& v)
							{ _byteInfo = MCHEmul::UByte::_0; _bitManaged = 0; }

			virtual void initialize (MCHEmul::StandardDatasette::Status st) override;

			private:
			unsigned int _cyclesToAction;
			size_t _bitsLastByte;

			// Implementation
			/** The byte being shifted now. At the beginning is 0. */
			MCHEmul::UByte _byteInfo;
			/** The bit read/written within the previous byte.
				In the last one, this value cannot be bigger that _bitsLastByte. */
			size_t _bitManaged;
		};

		static const int _ID = 100;

		Datasette (unsigned int rS);

		/** The only type admitted in TZX, but only when all blocks as sampling info. */
		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** By default it creates an empty TZX type of file.
			The only block that will be manage is the one reflects exactly the length of the sounds. */
		virtual MCHEmul::FileData* emptyData () const override
							{ return (new TZXFileData); }
		/** The data is returned in the format that was provided either with connectData or with emptyData.
			The type of data managed in this way is TZX only. */
		virtual MCHEmul::FileData* retrieveData () const override;

		private:
		/** The original data, before it is converted into 
			the info requested by this peripheral. */
		MCHEmul::ExtendedDataMemoryBlocks _originalData;
	};

	/** This type of datasette "injects" the infomation into the memory when it is simulated. 
		The way it is done, will depend on the type of structure prepared for that, 
		and also on the type of computer that is being simulated. \n
		No need of keys as the file that can be uploaded has only 1 data file inside. */
	class DatasetteInjection final : public MCHEmul::DatasettePeripheral
	{
		public:
		static const int _ID = 101;

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

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugSimulation (MCHEmul::CPU* cpu);
		void debugStatus (const std::string& where, FZ80::CZ80* cpu);
		void debugErrorTrap ();
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
