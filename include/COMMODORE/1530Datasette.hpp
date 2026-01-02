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
#include <COMMODORE/FileIO.hpp>

namespace COMMODORE
{
	/** This unit was used in many COMMODORE models. \n
		Mainly all of them including COMMODORE16 (which name in that case is 1531 and has differences in the connector). \n
		The datasette moves between 300 and 311 bits per second in average, 
		depending on the speed of the computer connected (PAL/NTSC). \n
		Every bit is broken in 4 datasette cycles (DCycle) of information. \n
		Every two datasette cycles makes a "dipole", so a bit of info (that is not a logical bit) in the datasette 
		is always make up of 2 "dipoles". \n
		A leader bit is made up of 4 0 type DCycles = 16 DCycles \n
		A 0 bit is made up of 2 (1st dipole) 0 DCycles and 2 (2nd dipole) 1 DCycles. \n
		A 1 bit is made up of 2 (1st dipole) 1 DCycles and 2 (2nd dipole) 0 DCycles. \n
		A work marker bit is make up of a word marker dipole and and one dipole 1. \n
		Every cycle lasts different depending whether it represents a leader bit (174 us), 
		a 0 data bit (169 us), a 1 data bit (247 us) or a word market bit (332 us).
		https://archive.org/details/COMPUTEs_VIC-20_and_Commodore_64_Tool_Kit_Kernal_1985_COMPUTE_Publications_a (page 275).
		The way 0 and 1 are kept in the datasette is changing the frequency of the signal stored.
		It means changinf the time the CPU takes before storing the next piece of info! */
	class Datasette1530 : public MCHEmul::StandardDatasette
	{
		public:
		/** The implementation (@see TAPFileData class). 
			The paremeter received is the number of cycles before the system is ready for the next read activity. \n
			And that number of cycles are used (comparing to the current cycles in the methods redefined. \n
			By default the TAP File Format is written and read as follows: 
			-> When reading a value from the file, that value is converted into number of cycles to wait until 
			   the next falling edge comes (that is when a "1" is sent to the computer. And always a 1). \n
			-> When writing a value to the file, the number of cycles waited since the last write operation
			   is converted into a byte value to be stored in the file. \n
			So the format detect and keeps the low-to-high transitions, and the time between them. */
		class TAPFileFormatImplementation : // It is not final because the C16 is a bit different! 
			public MCHEmul::StandardDatasette::Implementation
		{
			public:
			TAPFileFormatImplementation (unsigned int cR)
				: MCHEmul::StandardDatasette::Implementation (),
				  _cyclesToRead (cR)
							{ }

			/** The time is reached <0> when the number of cycles to read has been reached.
				The value returned is always true <1> because this datasette sends 
				1 anytime a change in the polarity is detected. 
				<2> At that moment if will ne also the time to read a new value. */
			virtual std::tuple <bool, bool, bool> timeToReadValue (unsigned int cC) override
							{ return ((clockValue () >= _cyclesToRead) 
								? std::tuple <bool, bool, bool> ({ true, true, true }) 
								: std::tuple <bool, bool, bool> ({ false, false, false })); }
			/** The value read from the file is translated into number of cycles until the next 
				falling edge comes, that is the signal sent to the computer. */
			virtual void whenValueRead (unsigned int cC, const MCHEmul::UByte& v) override;

			/** The information has to tell us that is the time, and the value to write has to be 1.
				because it will represent a complete cycle. 0 - 1 - 0 or 1 - 0 - 1 */
			virtual std::tuple <bool, MCHEmul::UByte> timeToWriteValue (unsigned int cC, 
				bool vc, unsigned int ccvc, bool vs) override
							{ return ((vc && (cC == ccvc) && vs)
								? std::tuple <bool, MCHEmul::UByte> 
									{ true, MCHEmul::UByte ((unsigned char) (clockValue () >> 3)) }
								: std::tuple <bool, MCHEmul::UByte> { false, MCHEmul::UByte::_0 }); }
			/** The only thing to do is to reset the internal counter. */
			virtual void whenValueWritten (unsigned int cC, const MCHEmul::UByte& v)
							{ resetClock (); }

			protected:
			/** The number of cycles that the datasette need to read a value. \n
				In the TAP file data every data byte represents (with a formula) 
				the number of cycles to wait until the next down edged of every signal that is 
				exactly when a "1" is sent to the computer. */
			unsigned int _cyclesToRead;
		};

		static const int _ID = 100;

		/** The number of cycles to next read operation is received as paraneter. \n
			Usually that number of seconds is not constant but changed as the simulation progreeses. */
		Datasette1530 (unsigned int cR, TAPFileFormatImplementation* dI);

		/** To connect the type of data TAP. */
		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** To create an empty file of type TAP. 
			The one by default is C64 type (@see TAPFileData class). 
			The information is created for a C64 by default (it is who this format was created for!). */
		virtual MCHEmul::FileData* emptyData () const override
							{ return (new TAPFileData); }
		/** To get the data kept into the system as something to be saved. 
			The only type of information supported is TAP. */
		virtual MCHEmul::FileData* retrieveData () const override;
	};

	/** This unit is just to jump over the routines aimed for this task in the kernel. 
		The info needed to "trap" the right position will depend on the final computer emulated. \n
		The most important method is executeTrap. The method is execute when a position of the trap is found. \n
		The code of that methd has to be aligned with the KERNEL using it. \n
		The default implementation is alligned with a Commodore 64 and a Commodore VIC20 kernel. */
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
		/** No data can be retrieved using this device. */
		virtual MCHEmul::FileData* retrieveData () const override
							{ return (nullptr); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** Invoked from simulate. It can be overloaded, 
			but a couple of basic traps are understood: "Header" & "Retrieve".
			Define them when construct the object because the address In/Out can vary per type of COMMODORE machine. \n
			However this default implementation oas highlighted above is ready for a Commodore 64 and a Commodore VIC20 */
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
