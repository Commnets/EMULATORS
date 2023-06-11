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
		Mainly all of them except COMMODORE16. */
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
		virtual bool getNextDataBit ();
		virtual void storeNextDataBit (bool s);

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
		MCHEmul::Clock _clock;
		
		mutable unsigned char _readWritePhase; // Phase in written or readding: header, content, end...
		// Counting which the info to write or read!
		mutable size_t _dataCounter; 
		mutable unsigned short _elementCounter;
	};
}

#endif
  
// End of the file
/*@}*/
