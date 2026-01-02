#include <C264/1531Datasette.hpp>

// ---
void C264::Datasette1531::TAPFileFormatImplementation::whenValueRead 
	(unsigned int cC, const MCHEmul::UByte& v)
{
	COMMODORE::Datasette1530::TAPFileFormatImplementation::whenValueRead (cC, v);

	// Change the value to be read from the datasette next time...
	_valueToFromDatasette = !_valueToFromDatasette;
}

// ---
void C264::Datasette1531::TAPFileFormatImplementation::whenValueWritten 
	(unsigned int cC, const MCHEmul::UByte& v)
{ 
	COMMODORE::Datasette1530::TAPFileFormatImplementation::whenValueWritten (cC, v);

	// Change the expected value to be written next time in the datasette...
	_valueToFromDatasette = !_valueToFromDatasette;
}

// ---
void C264::Datasette1531::TAPFileFormatImplementation::initialize 
	(MCHEmul::StandardDatasette::Status st)
{ 
	COMMODORE::Datasette1530::TAPFileFormatImplementation::initialize (st); 

	// The value to play with from the beginning is always true!...
	_valueToFromDatasette = true; 
}

// ---
C264::Datasette1531::Datasette1531 (unsigned int cR, 
		C264::Datasette1531::TAPFileFormatImplementation* dI)
	: COMMODORE::Datasette1530 (cR, dI)
{
	// The name of the class is changed...
	setClassName ("C2N1531");
}

// ---
bool C264::Datasette1531::connectData (MCHEmul::FileData* dt)
{
	// The verification in this case is a bit stronger!
	// The contect of the TAP format needs to be created thinking in the COMMODORE16 version!
	if (dynamic_cast <COMMODORE::TAPFileData*> (dt) == nullptr ||
		static_cast <COMMODORE::TAPFileData*> (dt) -> _computerVersion 
			!= COMMODORE::TAPFileData::ComputerVersion::_C16)
		return (false);

	return (COMMODORE::Datasette1530::connectData (dt));
}

// ---
MCHEmul::FileData* C264::Datasette1531::retrieveData () const
{
	COMMODORE::TAPFileData* result = 
		static_cast <COMMODORE::TAPFileData*> (COMMODORE::Datasette1530::retrieveData ());
	assert (result != nullptr); // It cannot happen, but just in case...

	// There are some attributes different than the original ones...
	// Signature:
	result -> _signature = std::string ("C16-TAPE-RAW");
	// The version of the file (in C16 systems the version used is 2)
	result -> _version = 2;
	// ...and also the Computer version:
	result -> _computerVersion = COMMODORE::TAPFileData::ComputerVersion::_C16;

	// ...and returns it!
	return (result);
}

// ---
bool C264::Datasette1531Injection::executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu)
{
	bool result = true;

	// The CPU has to be from the F6500 family!
	assert (dynamic_cast <F6500::C6500*> (cpu) != nullptr);

	_IFDEBUG debugStatus (t.asString (), cpu);

	switch (t._id)
	{
		// When the loader is looking for a header...
		case _FINDHEADERTRAP:
			{
				_IFDEBUG debugHeaderFileFound (cpu);

				int e = 0;

				MCHEmul::Address ctteBuffer = _definition._bufferAddr;
				// The base address of the cassette is also reflected in the memory...
				cpu -> memoryRef () -> put (MCHEmul::Address ({ 0xb6, 0x00 }, false), 
					{ 0x33, 0x03 } /** The address 0x0333, that is the address of the buffer. */);

				// If there is no data, makes no sense to progress...
				// ..ot the data counter is at the end of the datasette file...
				if (_data._data.empty () ||
					_dataCounter > (_data._data.size () - 1))
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

						e = 2;

						break;
					}
				}

				// Only if something usefull was found, the method continues...
				if (e == 0)
				{
					// Storing the header of the info found in the cassette buffer...
					MCHEmul::DataMemoryBlock& dtM = _data._data [_dataCounter];
					cpu -> memoryRef () -> put (MCHEmul::Address ({ 0xf8, 0x00 }, false), 0x01 /** Machine type default value. */);
					cpu -> memoryRef () -> put (ctteBuffer, MCHEmul::UBytes (dtM.startAddress ().bytes (), 
						false /** little - endian. */).bytes ());
					cpu -> memoryRef () -> put (ctteBuffer + 2, MCHEmul::UBytes (dtM.endAddress ().bytes (), false).bytes ());
					for (size_t i = 0; i < 16 /** No more than 16 bytes. */; i++)
						cpu -> memoryRef () -> put (ctteBuffer + 4 + i, 
							(i < dtM.name ().size ()) ? dtM.name ()[i] : MCHEmul::UByte::_0);
				}
				else
				if (e == 1)
				{
					cpu -> memoryRef () -> put (MCHEmul::Address ({ 0xf8, 0x00 }, false), 0x05 /** End of casette. */);

					_IFDEBUG debugNothingToRead ();
				}
				else
					_IFDEBUG debugErrorTrap ();

				// Cleans up everthing...
				cpu -> memoryRef () -> put (_definition._statusAddr, MCHEmul::UByte::_0);
				cpu -> memoryRef () -> put (_definition._verifyFlagAddr, MCHEmul::UByte::_0);

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
				_IFDEBUG debugDataFileFound (cpu);

				MCHEmul::Address start (cpu -> memoryRef () -> 
					values (_definition._startProgramAddr, 2).reverse ());
				MCHEmul::Address end (cpu -> memoryRef () -> 
					values (_definition._endProgramAddr, 2).reverse ());

				loadDataBlockInRAM (_data._data [_dataCounter], cpu);

				cpu -> memoryRef () -> put (_definition._statusAddr, 
					cpu -> memoryRef () -> value (_definition._statusAddr) | 0x40 /** EOF. */);
			}

			break;

		default:
			{
				_LOG ("Trap type not supported yet:" + 
					std::to_string (t._id));
				assert (false); // Just for compiling with debug on...
			}

			break;
	}

	return (result);
}
