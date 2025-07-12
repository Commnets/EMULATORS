/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: 1530Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the 1530 Datasette Unit.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_1530DATASETTE__
#define __COMMODORE_1530DATASETTE__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** This unit was used in many COMMODORE models. \n
		Mainly all of them except COMMODORE16. \n
		The datasette moves between 300 and 311 bits per second in average, 
		depending on the speed of the computer connected. \n
		Every bit is broken in 4 datasette cycles (DCycle) of information. \n
		Every two datasette cycles makes a "dipole", so a bit is always make up of 2 "dipoles". \n
		A leader bit is made up of 4 0 type DCycles = 16 DCycles \n
		A 0 bit is made up of 2 (1st dipole) 0 DCycles and 2 (2nd dipole) 1 DCycles. \n
		A 1 bit is made up of 2 (1st dipole) 1 DCycles and 2 (2nd dipole) 0 DCycles. \n
		A work marker bit is make up of a word marker dipole and and one dipole 1. \n
		Every cycle lasts different depending whether it represents a leader bit (174 us), 
		a 0 data bit (169 us), a 1 data bit (247 us) or a word market bit (332 us).
		https://archive.org/details/COMPUTEs_VIC-20_and_Commodore_64_Tool_Kit_Kernal_1985_COMPUTE_Publications_a (page 275).
		The way 0 and 1 are kept in the datasette is changing the frequency of the signal stored.
		It means changinf the time the CPU takes before storing the next piece of info!
		*/
	class Datasette1530 : public MCHEmul::StandardDatasette
	{
		public:
		/** The implementation (@see TAPFileData class). 
			The paremeter received is the number of cycles before the system is ready for the next read activity. \n
			And that number of cycles are used (comparing to the current cycles in the methods redefined. */
		class TAPFileFormatImplementation final : 
			public MCHEmul::StandardDatasette::Implementation
		{
			public:
			TAPFileFormatImplementation (unsigned int cR)
				: MCHEmul::StandardDatasette::Implementation (),
				  _cyclesToRead (cR)
							{ }

			/** v is set to 1, when the time to read is true,
				pointing out that a falling edge has been detected. */
			virtual bool timeToReadValue (bool &v) const override
							{ return (v = (clockValue () >= _cyclesToRead)); }
			/** The value read from the file is translated into number of cycles until the next 
				falling edge comes, that is the signal sent to the computer. */
			virtual void whenValueRead (const MCHEmul::UByte& v) override;

			/** The value sent to the file is always related with the number of cycles
				since this method was invoked last time. */
			virtual MCHEmul::UByte valueToSaveForBit (bool v) const override
							{ return (clockValue () >> 3); }

			private:
			/** The number of cycles that the datasxette need to read a value. \n
				In the TAP file data every data byte represents (with a formula) 
				the number of cycles to wait until the next down edged of every signal that is 
				exactly when a "1" is sent to the computer. */
			unsigned int _cyclesToRead;
		};

		static const int _ID = 100;

		/** The number of cycles to next read operation is received as paraneter. \n
			Usually that number of seconds is not constant but changed as the simulation progreeses. */
		Datasette1530 (unsigned int cR);

		/** To connect the type of data TAP. */
		virtual bool connectData (MCHEmul::FileData* dt) override;
	};

	/** This unit is just to jump over the routines aimed for this task in the kernel. 
		The info needed to "trap" the right position will depend on the final computer emulated. */
	class Datasette1530Injection : public MCHEmul::StandardDatasette
	{
		public:
		struct Definition final
		{
			public:
			MCHEmul::Address _bufferAddr;			// Where the tape buffer starts...
			MCHEmul::Address _statusAddr;			// to keep tract of the status of the datasette...
			MCHEmul::Address _verifyFlagAddr;		// The same loading routines in CBM kernal can be used for loading or verifying...
			MCHEmul::Address _irqTmpAddr;	
			unsigned short _irqVal;
			MCHEmul::Address _startProgramAddr;		// Init of the RAM where program is loaded...
			MCHEmul::Address _endProgramAddr;		// End of the RAM where program is loaded...
			MCHEmul::Address _keyboardBufferAddr;	// Where the keyboard buffer starts (to simulate later "autorun")...
			MCHEmul::Address _keyboardPendingAddr;	// Number of keys ending to be read in the keyboard buffer...
			const MCHEmul::Traps _traps;			// The traps...

			MCHEmul::InfoStructure getInfoStructure () const;
		};

		static const int _ID = 101;

		// Use these following ids to identify the two traps that this class
		// understood by default...
		static const int _FINDHEADERTRAP = 0;
		static const int _RECEIVEDATATRAP = 1;

		Datasette1530Injection (const Definition& dt);

		virtual bool connectData (MCHEmul::FileData* dt) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** Invoked from simulate. It can be overloaded, 
			but a couple of basic traps are understood: "Header" & "Retrieve".
			Define them when construct the object because the address In/Out can vary per type of COMMODORE machine. */
		virtual bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu);

		/** To load the program into the memory. \n
			In some COMMODORE computers (like C64) the whole RAM memory is not directly accesible. */
		virtual void loadDataBlockInRAM (const MCHEmul::DataMemoryBlock& dB, MCHEmul::CPU* cpu)
							{ cpu -> memoryRef () -> put (_data._data [_dataCounter]); }

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugStatus (const std::string& where, MCHEmul::CPU* cpu);
		void debugHeaderFileFound (MCHEmul::CPU* cpu);
		void debugDataFileFound (MCHEmul::CPU* cpu);
		void debugErrorTrap ();
		void debugNothingToRead ();
		// -----

		protected:
		Definition _definition;
	};
}

#endif
  
// End of the file
/*@}*/
