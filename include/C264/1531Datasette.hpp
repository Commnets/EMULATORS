/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: 1531Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 22/11/2025 \n
 *	Description: To emulate the behaviour of the 1531 Datasette Unit in a C264.
 *				 That is a bit different that the one in other C64 models.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_1531DATASETTE__
#define __C264_1531DATASETTE__

#include <COMMODORE/incs.hpp>
#include <C264/Memory.hpp>

namespace C264
{
	/** In the Commodore 16/Plus4 the way the standard datsette behaves is a bit different.
		The duration of the cycles is half the pulse. \n
		So the time is up level and the time in low level are kep separatelly.
		So the length of the file will be "double" the one in other computers. */
	class Datasette1531 : public COMMODORE::Datasette1530
	{
		public:
		class TAPFileFormatImplementation final : 
			public COMMODORE::Datasette1530::TAPFileFormatImplementation
		{
			public:
			TAPFileFormatImplementation (unsigned int cR)
				: COMMODORE::Datasette1530::TAPFileFormatImplementation (cR),
				  _valueToFromDatasette (true)
							{ }

			/** The time is reached <0> when the number of cycles to read has been reached.
				The value returned in that case is the one expected. 
				It alternates between 0 and 1, starting always in 1. */
			virtual std::tuple <bool, bool, bool> timeToReadValue (unsigned int cC) override
							{ return ((clockValue () >= _cyclesToRead) 
								? std::tuple <bool, bool, bool> ({ true, _valueToFromDatasette, true }) 
								: std::tuple <bool, bool, bool> ({ false, false, false })); }
			/** Once the value has been read, the value expected is the opposite. 
				Apart of that the lock is reset. */
			virtual void whenValueRead (unsigned int cC, const MCHEmul::UByte& v) override;

			/** In this case, the value written can be either 1 or 0 but has to be the one expected! 
				In the 1531 the transitions between 0-1 and between 1 and 0 are kept. */
			virtual std::tuple <bool, MCHEmul::UByte> timeToWriteValue (unsigned int cC, 
				bool vc, unsigned int ccvc, bool vs) override
							{ return ((vc && (cC == ccvc) && (_valueToFromDatasette == vs))
								? std::tuple <bool, MCHEmul::UByte> 
									{ true, MCHEmul::UByte ((unsigned char) (clockValue () >> 3)) }
								: std::tuple <bool, MCHEmul::UByte> { false, MCHEmul::UByte::_0 }); }
			/** After writting, the clock is reset and also the value to expect is the opposite. */
			virtual void whenValueWritten (unsigned int cC, const MCHEmul::UByte& v);

			private:
			/** In this case also the internal variable has to be initialized. */
			virtual void initialize 
				(MCHEmul::StandardDatasette::Status st) override;

			/** The value to be moved into/from the datasette. \n
				It is changed every time a value is read. */
			bool _valueToFromDatasette;
		};

		Datasette1531 (unsigned int cR, TAPFileFormatImplementation* dI);

		/** To connect the type of data TAP. 
			On this case it is verified that the parameter given is right for this type of computer too! */
		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** To create an empty file of type TAP. 
			In this case some additional parameters must be provided, 
			The default ones are not enough!. */
		virtual MCHEmul::FileData* emptyData () const override
							{ return (new COMMODORE::TAPFileData 
								("C16-TAPE-RAW", 2 /** Always 2 in this case. */, 
								 COMMODORE::TAPFileData::ComputerVersion::_C16)); }
		/** To get the data kept into the system as something to be saved. 
			The only type of information supported is TAP. */
		virtual MCHEmul::FileData* retrieveData () const override;
	};

	/** In the C16, the trap executed when LOAD/SAVE is executed is a bit different 
		from the one in the C64 and VIC20. \n
		So this class just redefines that method to manage that difference. \n
		The difference are not only (obviously) at the point the trap is executed but also inthe actions taken. */
	class Datasette1531Injection : public COMMODORE::Datasette1530Injection
	{
		public:
		Datasette1531Injection (const Definition& dt)
			: COMMODORE::Datasette1530Injection (dt)
							{ }

		private:
		/** In this case the implementation of the trap is a bit differenet 
			than in the case of the Commodore64 and VIC20. \n */
		virtual bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu) override;
	};
}

#endif
  
// End of the file
/*@}*/
