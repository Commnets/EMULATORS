/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: 1530Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the 1530 Datasette Unit.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_1530DATASETTE__
#define __COMMODORE_1530DATASETTE__

#include <CORE/incs.hpp>
#include <COMMODORE/DatasettePeripherals.hpp>

namespace COMMODORE
{
	/** This unit was used in many COMMODORE models. \n
		Mainly all of them except COMMODORE16. \n
		The datasette moves between 300 and 311 bits per second, 
		depending on the speed of the computer connected. \n
		Every bit is broken in 4 cycles of information. \n
		Every two cycles makes a dipole, so a bit is always make up of 2 dipoles. \n
		A leader bit is made up of 4 0 cyles. \n
		A 0 bit is made up of 2 (1st dipole) 0 cycles and 2 (2nd dipole) 1 cycles. \n
		A 1 bit is made up of 2 (1st dipole) 1 cycles and 2 (2nd dipole) 0 cycles. \n
		A work market bit is make up of a word marker dipole and and one dipole 1. \n
		Every cycle lasts different depending whether it represents a leader bit (174 us), 
		a 0 data bit (169 us), a 1 data bit (247 us) or a word market bit (332 us).
		https://archive.org/details/COMPUTEs_VIC-20_and_Commodore_64_Tool_Kit_Kernal_1985_COMPUTE_Publications_a (page 275).
		*/
	class Datasette1530 final : public DatasettePeripheral
	{
		public:
		static const int _ID = 100;

		/** The commands accepted by this peripheral. \n
			They refer mainly to keys that can be pressed. \n
			Notice that combinations are possible. */
		static const int _KEYFOWARD = 1;
		static const int _KEYREWIND = 2;
		static const int _KEYSTOP   = 4;
		static const int _KEYPLAY   = 8;
		static const int _KEYRECORD = 16;
		static const int _KEYEJECT	= 32; // To clean up the data loaded, or to simulate a new castte is inserted...
		/** The key EJECT has no value. */

		Datasette1530 ();

		virtual bool initialize () override;

		virtual bool executeCommand (int id, const MCHEmul::Strings& prms) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual bool connectData (MCHEmul::FileData* dt) override;

		/**
		  *	The name of the fields are: \n
		  * The ones in the parent class. \n
		  *	ATTRS	= InfoStructure: Attributes defining the Peripheral. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** According with the phase the things to store will be one or another. \n
			The can be overloaded to simulate different types of storage, if needed. \n
			But by default they implemented the commodore standard. */
		inline bool getNextDataBit ();
		inline void storeNextDataBit (bool s);

		private:
		/** The different status that this peripheral can be in. \n
			At creating this datasette is stopped. */
		enum class Status
		{
			_STOPPED = 0,
			_READING = 1,
			_SAVING = 2
		};

		// Immplementation
		mutable Status _status;
		
		// Counting which the info to write or read!
		mutable size_t _dataCounter; 
		mutable unsigned short _elementCounter;

		// Managing the clockcycles...
		bool _firstCycleSimulation;
		unsigned int _lastCPUCycles;
		unsigned int _clockCycles;
	};

	// ---
	inline bool Datasette1530::getNextDataBit ()
	{
		return (_data._data [_dataCounter].bytes ()[_elementCounter++] == MCHEmul::UByte::_1);
	}

	// ---
	inline void Datasette1530::storeNextDataBit (bool s)
	{
		_data._data [_dataCounter].addByte 
			(s ? MCHEmul::UByte::_1 /** cycle 1. */ : MCHEmul::UByte::_0 /** cycle 0. */);

		_elementCounter++;
	}
}

#endif
  
// End of the file
/*@}*/
