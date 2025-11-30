#include <C264/1531Datasette.hpp>

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
