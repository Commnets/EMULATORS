#include <COMMODORE/1530Datasette.hpp>
#include <COMMODORE/FileIO.hpp>
#include <F6500/incs.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS, true /** Motor controlled internaly. */,
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{
	setClassName ("C2N1530");
}

// ---
COMMODORE::Datasette1530P::Datasette1530P (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS, true /** Motor controlled internally. */,
		{ { "Name", "Commodore 1530 Parallel (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{
	setClassName ("C2N1530");
}

// ---
MCHEmul::InfoStructure COMMODORE::Datasette1530Injection::Definition::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("BUFFER", _bufferAddr);
	result.add ("STATUS", _statusAddr);
	result.add ("VERIFYFLAG", _verifyFlagAddr);
	result.add ("STARTPRG", _startProgramAddr);
	result.add ("ENDPRG", _endProgramAddr);
	result.add ("KEYBOARDBUFFER", _keyboardBufferAddr);
	result.add ("KEYBOARDPENDING", _keyboardPendingAddr);

	MCHEmul::InfoStructure trps;
	for (size_t i = 0; i < _traps.size (); i++)
		trps.add (std::to_string (i), _traps [i].asString ());
	result.add ("Traps", std::move (trps));

	return (result);
}

// ---
COMMODORE::Datasette1530Injection::Datasette1530Injection (const COMMODORE::Datasette1530Injection::Definition& def)
	: MCHEmul::StandardDatasette (_ID,
		new MCHEmul::StandardDatasette::IONilSimulation, new MCHEmul::StandardDatasette::IOEmptyEncoder, true,
		{ { "Name", "Datasette Injection 1530 (CN2)" },
		  { "Manufacturer", "ICF to inject the code directly into the memory" } }),
	  _definition (def)
{ 
	setClassName ("C2N1530");
}

// ---
bool COMMODORE::Datasette1530Injection::simulate (MCHEmul::CPU* cpu)
{
	bool result = true;

	// What to do will depend on the simulation is...
	// Te definition can be in many different trap(
	// Detect first whether the simulation is or not in one of them and then execute the right thing...
	// There can not be more than 1 trap with the same name
	bool nF = false;
	for (COMMODORE::Datasette1530Injection::Traps::const_iterator i = _definition._traps.begin ();
		i != _definition._traps.end () && !nF; i++)
	{
		if (nF = (cpu -> programCounter ().internalRepresentation () == (*i)._addressIn.value ()))
		{
			// Execute the trap...
			result = executeTrap ((*i), cpu);

			cpu -> programCounter ().setAddress ((*i)._addressOut);
		}
	}

	return (result);
}

// ---
bool COMMODORE::Datasette1530Injection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <COMMODORE::T64FileData*> (dt) == nullptr &&
		dynamic_cast <COMMODORE::PRGFileData*> (dt) == nullptr)
		return (false); // These formats are the only ones accepted only...

	_data = dt -> asMemoryBlocks ();

	_dataCounter = 0;
	_elementCounter = 0;

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::Datasette1530Injection::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::StandardDatasette::getInfoStructure ());

	result.add ("Definition", std::move (_definition.getInfoStructure ()));

	return (result);
}

// ---
bool COMMODORE::Datasette1530Injection::executeTrap 
	(const COMMODORE::Datasette1530Injection::Trap& t, MCHEmul::CPU* cpu)
{
	bool result = true;

	// The CPU has to be from the F6500 family!
	assert (dynamic_cast <F6500::C6500*> (cpu) != nullptr);

	switch (t._id)
	{
		// When the loader is lloking for a header...
		case _FINDHEADERTRAP:
		{
			int e = 0;

			// Gets with the buffer is really located...(
			// using and indirect address mode...
			// This info will be used many times later...
			MCHEmul::Address ctteBuffer (cpu -> memoryRef () -> values (_definition._bufferAddr, 2).reverse ());

			// If there is no data, makes no sense to progress...
			if (_data._data.empty ())
				e = 1;
			else
			{
				// Or if there is no any type of data of the type
				// that this traps needs...
				while (_data._data [_dataCounter].attribute ("TYPE") != "1" /** File record normal = 1 */
					&& _dataCounter < _data._data.size ())
					_dataCounter++;
				if (_dataCounter > _data._data.size ())
				{
					_dataCounter = 0; // Start back next time...

					e = 1;

					break;
				}
			}

			// Only if something usefull was found, the method continues...
			if (e == 0)
			{
				// Storing the header of the info found in the casette buffer...
				MCHEmul::DataMemoryBlock& dtM = _data._data [_dataCounter];
				cpu -> memoryRef () -> set (ctteBuffer, 0x01 /** Machine type default value. */);
				cpu -> memoryRef () -> set (ctteBuffer + 1, MCHEmul::UBytes (dtM.startAddress ().bytes (), 
					false /** little - endian. */).bytes ());
				cpu -> memoryRef () -> set (ctteBuffer + 3, MCHEmul::UBytes (dtM.endAddress ().bytes (), false).bytes ());
				for (size_t i = 0; i < 16 /** No more than 16 bytes. */; i++)
					cpu -> memoryRef () -> set (ctteBuffer + 5 + i, 
						(i < dtM.name ().size ()) ? dtM.name ()[i] : MCHEmul::UByte::_0);
			}
			else
				cpu -> memoryRef () -> set (ctteBuffer, 0x05 /** End of casette. */);

			// Cleans up everthing...
			cpu -> memoryRef () -> set (_definition._statusAddr, MCHEmul::UByte::_0);
			cpu -> memoryRef () -> set (_definition._verifyFlagAddr, MCHEmul::UByte::_0);
			if (_definition._irqTmpAddr != MCHEmul::Address ())
				cpu -> memoryRef () -> set (_definition._irqTmpAddr, 
					{ (unsigned char) (_definition._irqVal & 0x00ff), 
					  (unsigned char) ((_definition._irqVal & 0xff00) >> 8) } /** Already in little - endian. */);

			// Before finishing, check whether STOP key was or not pressed
			// before this trap wwas reached...
			// If it was, the carry flag of the cpu is set to true, otherwise it is false...
			cpu -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, false);
			for (unsigned char i = cpu -> memoryRef () ->
				value (_definition._keyboardPendingAddr).value (); i > 0; i--)
			{
				if (cpu -> memoryRef () -> value (_definition._keyboardBufferAddr + i - 1) == 0x03)
				{
					cpu -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, true);

					break;
				}
			}

			// Finally the ZERO flag is set to true...
			// It is how the original kernal routine worked!
			cpu -> statusRegister ().setBitStatus (F6500::C6500::_ZEROFLAG, true);
		}

		break;

		// When there is data about to be loaded...
		case _RECEIVEDATATRAP:
		{
			MCHEmul::Address start (cpu -> memoryRef () -> values (_definition._startProgramAddr, 2).reverse ());
			MCHEmul::Address end (cpu -> memoryRef () -> values (_definition._endProgramAddr, 2).reverse ());

			switch (static_cast <F6500::C6500*> (cpu) -> xRegister ().values () [0].value ())
			{
				case 0x0e:
					cpu -> memoryRef () -> set (_data._data [_dataCounter]);
					break;

				default:
					assert (false); // Funtion not supported!
					break;
			}

			if (_definition._irqTmpAddr != MCHEmul::Address ())
				cpu -> memoryRef () -> set (_definition._irqTmpAddr, 
					{ (unsigned char) (_definition._irqVal & 0x00ff), 
					  (unsigned char) ((_definition._irqVal & 0xff00) >> 8) } /** Already in little - endian. */);

			cpu -> memoryRef () -> set (_definition._statusAddr, 
				cpu -> memoryRef () -> value (_definition._statusAddr) | 0x40 /** EOF. */);

			cpu -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, false);
			cpu -> statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, false);
		}

		break;

		default:
			assert (false); // It shouldn't be here anyway...just in case!
			break;
	}

	return (result);
}
