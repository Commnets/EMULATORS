#include <COMMODORE/SerialIOPeripherals.hpp>
#include <F6500/incs.hpp>

// ---
MCHEmul::InfoStructure COMMODORE::SerialIOPeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("DEVICENUMBER", _deviceNumber);

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::SerialIOPeripheralSimulation::Definition::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("LATTABLE", MCHEmul::removeAll0 
		(_LATTABLE.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0')));
	result.add ("FATTABLE", MCHEmul::removeAll0 
		(_FATTABLE.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0')));
	result.add ("SATTABLE", MCHEmul::removeAll0 
		(_SATTABLE.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0')));
	result.add ("DFLTN",	MCHEmul::removeAll0 
		(_DFLTN.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0')));
	result.add ("DFLTO",	MCHEmul::removeAll0 
		(_DFLTO.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0')));

	MCHEmul::InfoStructure trps;
	for (size_t i = 0; i < _traps.size (); i++)
		trps.add (std::to_string (i), _traps [i].asString ());
	result.add ("TRAPS", std::move (trps));

	return (result);
}

// ---
COMMODORE::SerialIOPeripheralSimulation::SerialIOPeripheralSimulation (int id, unsigned char dN,
		const COMMODORE::SerialIOPeripheralSimulation::Definition& def, const MCHEmul::Attributes& attrs)
	: COMMODORE::SerialIOPeripheral (id, dN, attrs),
	  _definition (def),
	  _status (COMMODORE::SerialIOPeripheralSimulation::Status::_NOTLISTENING),
	  _statusOpen (COMMODORE::SerialIOPeripheralSimulation::StatusOpen::_CLOSE),
	  _lastCPUCycles (0)
{ 
	setClassName ("SerialIOPeripheralSimulation");
}

// ---
bool COMMODORE::SerialIOPeripheralSimulation::simulate (MCHEmul::CPU* cpu)
{
	// What to do will depend on the simulation is...
	// Te definition can be in many different traps
	// Detect first whether the simulation is or not in one of them and then execute the right thing...
	// There can not be more than 1 trap with the same name
	bool nF = false;
	for (MCHEmul::Traps::const_iterator i = _definition._traps.begin ();
		i != _definition._traps.end () && !nF; i++)
	{
		if (nF = (cpu -> programCounter ().internalRepresentation () == (*i)._addressIn.value () && 
			(*i).verifyFingerTipAgainst (cpu -> memoryRef ()))) // The fingertip has to be also "found"...
		{
			// Empty any pending set action...
			// Because a trap is breaking the normal flow of the code...
			// so no set commands pendings can be left!
			MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

			// Execute the trap...
			// ...and if it was executed, change the program counter
			if (executeTrap ((*i), cpu))
				cpu -> programCounter ().setAddress ((*i)._addressOut);
		}
	}

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::SerialIOPeripheralSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (COMMODORE::SerialIOPeripheral::getInfoStructure ());

	result.add ("SerialIOSimulationDefinition", std::move (_definition.getInfoStructure ()));

	return (result);

}

// ---
bool COMMODORE::SerialIOPeripheralSimulation::executeTrap 
	(const MCHEmul::Trap& t, MCHEmul::CPU* cpu)
{
	// The CPU has to be from the F6500 family!
	assert (dynamic_cast <F6500::C6500*> (cpu) != nullptr);

	_IFDEBUG debugStatus (t.asString (), cpu);

	/** Whether the trap was or not executed. 
		Initially it is not. */
	bool result = false;
	/** The result when executed. */
	unsigned char st = 0;
	switch (t._id)
	{
		case _LISTEN1TRAP:
		case _LISTEN2TRAP:
			{
				result = executeListenTrap (cpu, st);
			}

			break;

		case _SENDBYTETRAP:
			{
				result = executeSendByteTrap (cpu, st);
			}

			break;

		case _RECEIVEBYTETRAP:
			{
				result = executeReceiveByteTrap (cpu, st);
			}

			break;

		case _READYTRAP:
			{
				result = executeReadyTrap (cpu, st);
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

	// If everything ok, sets the result
	// If a trap was executed actuallt but it didin't need to change the status, 
	// the value written will be 0 (the default one)
	if (result)
		cpu -> memoryRef () -> set (_definition._stAddress, 
			cpu -> memoryRef () -> value (_definition._stAddress) | st);

	return (result);
}

// ---
bool COMMODORE::SerialIOPeripheralSimulation::executeListenTrap (MCHEmul::CPU* cpu, unsigned char& st)
{
	// The commands that can be sent to the device...
	static const unsigned char _LISTEN		= 0x02;
	static const unsigned char _UNLISTEN	= 0x03; 
	static const unsigned char _TALK		= 0x04;
	static const unsigned char _UNTALK		= 0x05;
	static const unsigned char _SECONDARY	= 0x06;
	static const unsigned char _CLOSE		= 0x0e;
	static const unsigned char _OPEN		= 0x0f;

	// The LS Nibble is the logical channel...
	unsigned char iecdata = 
		cpu -> memoryRef () -> value (MCHEmul::Address ({ 0x95, 0x00 }, false)).value ();
	unsigned char cmd = (iecdata >> 4) & 0x0f; // Command = Most Significant Nibble...
	unsigned char attr = iecdata & 0x0f; // Device Number affected or 
										 // Secondary Command (what means there was a previous instruction) = Less Significant Nibble...

	// If the status of the device is not listening...
	// ...and the command received is not for the this specific device (_deviceNumber) to take attention
	// throught out the command _LISTER or _TALK, the command is not processed, 
	// and the token needs to go to the next serial device connected (up to 32)....
	if (_status == COMMODORE::SerialIOPeripheralSimulation::Status::_NOTLISTENING && 
		((cmd != _LISTEN && cmd != _TALK) ||
		 (cmd == _LISTEN || cmd == _TALK) && attr != deviceNumber ()))
		return (false); // The command was not for this element...

	// Depending on the command the action will be different...
	switch (cmd)
	{
		case _LISTEN:
			{
				_status = COMMODORE::SerialIOPeripheralSimulation::Status::_LISTENING;

				st = listen (cpu, attr);

				_IFDEBUG _deepDebugFile -> 
					writeSimpleLine ("Device  " + std::to_string ((int) attr) + " listening");
			}

			break;

		case _UNLISTEN:
			{
				_status = COMMODORE::SerialIOPeripheralSimulation::Status::_NOTLISTENING;

				st = unlisten (cpu, attr);

				_IFDEBUG _deepDebugFile -> 
					writeSimpleLine ("Device  " + std::to_string ((int) attr) + " not listening anymore");
			}

			break;

		case _TALK:
			{
				_status = COMMODORE::SerialIOPeripheralSimulation::Status::_TALKING;

				st = talk (cpu, attr);

				_IFDEBUG _deepDebugFile -> 
					writeSimpleLine ("Device  " + std::to_string ((int) attr) + " talking");
			}

			break;

		case _UNTALK:
			{
				_status = COMMODORE::SerialIOPeripheralSimulation::Status::_NOTLISTENING;

				st = untalk (cpu, attr);

				_IFDEBUG _deepDebugFile -> 
					writeSimpleLine ("Device  " + std::to_string ((int) attr) + " not talking anymore");
			}

			break;

		case _SECONDARY:
			{
				if (_status == COMMODORE::SerialIOPeripheralSimulation::Status::_LISTENING ||
					_status == COMMODORE::SerialIOPeripheralSimulation::Status::_TALKING)
					st = openChannel (cpu, attr);
				else
					_LOG ("Status " + std::to_string ((int) _status) + " not supported for SECONDARY command");

				_IFDEBUG _deepDebugFile -> 
					writeSimpleLine ("Device  " + std::to_string ((int) attr) + " not talking anymore");
			}

			break;

		case _CLOSE:
			{
				st = closeChannel (cpu, attr);

				_IFDEBUG _deepDebugFile -> 
					writeSimpleLine ("Device  " + std::to_string ((int) attr) + " closed");
			}

			break;

		case _OPEN:
			{
				st = openChannel (cpu, attr);

				_IFDEBUG _deepDebugFile -> 
					writeSimpleLine ("Device  " + std::to_string ((int) attr) + " opened");
			}

			break;

		default:
			{
				_LOG ("Serial Command " + std::to_string ((int) cmd) + " not supported");
			}

			break;
	}

	cpu -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, false);
	cpu -> statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, false);

	return (true);
}

// ---
bool COMMODORE::SerialIOPeripheralSimulation::executeSendByteTrap (MCHEmul::CPU* cpu, unsigned char& st)
{
	if (_status != COMMODORE::SerialIOPeripheralSimulation::Status::_LISTENING &&
		_status != COMMODORE::SerialIOPeripheralSimulation::Status::_TALKING)
		return (false); // The trap is not for this element!

	st = sendByte (cpu,
		cpu -> memoryRef () -> value (_definition._sDataAddress));

	cpu -> statusRegister ().setBitStatus 
		(F6500::C6500::_CARRYFLAG, false);
	cpu -> statusRegister ().setBitStatus 
		(F6500::C6500::_IRQFLAG, false);

	return (true);
}

// ---
bool COMMODORE::SerialIOPeripheralSimulation::executeReceiveByteTrap (MCHEmul::CPU* cpu, unsigned char& st)
{
	if (_status != COMMODORE::SerialIOPeripheralSimulation::Status::_LISTENING &&
		_status != COMMODORE::SerialIOPeripheralSimulation::Status::_TALKING)
		return (false); // The trap is not for this element!
	
	MCHEmul::UByte data;
	st = receiveByte (cpu, data);

	static_cast <F6500::C6500*> (cpu) -> accumulator ().set ({ data });
	cpu -> statusRegister ().setBitStatus 
		(F6500::C6500::_NEGATIVEFLAG, (data & 0x80) != 0);
	cpu -> statusRegister ().setBitStatus 
		(F6500::C6500::_ZEROFLAG, data == MCHEmul::UByte::_0);
	cpu -> statusRegister ().setBitStatus 
		(F6500::C6500::_CARRYFLAG, false);
	cpu -> statusRegister ().setBitStatus
		(F6500::C6500::_IRQFLAG, false);

	return (true);
}

// ---
bool COMMODORE::SerialIOPeripheralSimulation::executeReadyTrap (MCHEmul::CPU* cpu, unsigned char& st)
{
	if (_status != COMMODORE::SerialIOPeripheralSimulation::Status::_LISTENING &&
		_status != COMMODORE::SerialIOPeripheralSimulation::Status::_TALKING)
		return (false); // The trap is not for this element!

	st = getReady (cpu); // Just in case...

	static_cast <F6500::C6500*> (cpu) -> accumulator ().set ({ MCHEmul::UByte::_1 });
	cpu -> statusRegister ().setBitStatus
		(F6500::C6500::_NEGATIVEFLAG, false);
	cpu -> statusRegister ().setBitStatus
		(F6500::C6500::_ZEROFLAG, false);
	cpu -> statusRegister ().setBitStatus
		(F6500::C6500::_IRQFLAG, false);

	return (true);
}

// ---
void COMMODORE::SerialIOPeripheralSimulation::debugStatus 
	(const std::string& where, MCHEmul::CPU* cpu)
{
	auto allRgsAsAttrs = [&](const MCHEmul::Registers& rgs) -> MCHEmul::Attributes
		{ MCHEmul::Attributes result;
		  for (const auto& i : rgs) 
			  result.insert (MCHEmul::Attributes::value_type (i.name (), i.asString ()));
		  return (result); };

	_deepDebugFile -> writeSimpleLine (where);
	_deepDebugFile -> writeCompleteLine ("SerialIO", cpu -> clockCycles (), 
		"Registers:", allRgsAsAttrs (cpu -> internalRegisters ()));
}

// ---
COMMODORE::SerialNotPresentIOPeripheralSimulation::SerialNotPresentIOPeripheralSimulation
		(const Definition& dt)
	: SerialIOPeripheralSimulation (_ID, 0 /** Not important really. */, dt, 
		{ { "Name", "Device not present" },
		  { "Manufacturer", "ICF Serial Port simulation" } })
{
	setClassName ("SerialIONotPresentSimulation");
}

// ---
bool COMMODORE::SerialNotPresentIOPeripheralSimulation::executeTrap 
	(const MCHEmul::Trap& t, MCHEmul::CPU* cpu)
{
	_IFDEBUG debugStatus (t.asString (), cpu);

	cpu -> memoryRef () -> set (_definition._stAddress, 
		cpu -> memoryRef () -> value (_definition._stAddress) | _definition._notPresent);

	return (true);
}
