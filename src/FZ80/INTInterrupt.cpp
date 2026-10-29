#include <FZ80/INTInterrupt.hpp>
#include <FZ80/CZ80.hpp>

// ---
FZ80::INTInterrupt::INTInterrupt ()
	: Interrupt (_ID, 0, 0),
	  _im1MCycles
		({ _Z80_INTACK7 (), _Z80_MW3 (), _Z80_MW3 () }),
	  _im2MCycles
		({ _Z80_INTACK7 (), _Z80_MW3 (), _Z80_MW3 (),
		   _Z80_MR3 (), _Z80_MR3 () }),
	  _INTMode (0),
	  _dataBusValue (MCHEmul::UByte::_FF),
	  _im1Timing (_im1MCycles),
	  _im2Timing (_im2MCycles)
{
	// IM0 retains its split between launch and injected execution.
	setCyclesToLaunch (2);

	setClassName ("INTInterrupt");
}

// ---
const FZ80::MCyclesData* FZ80::INTInterrupt::memoryAccessTiming () const
{
	if (_INTMode == 2)
		return (&_im2Timing);

	if (_INTMode == 1 ||
		(_INTMode == 0 && (_dataBusValue.value () & 0xc7) == 0xc7))
		return (&_im1Timing);

	return (nullptr);
}

// ---
void FZ80::INTInterrupt::initialize ()
{
	MCHEmul::CPUInterrupt::initialize ();

	_dataBusValue = MCHEmul::UByte::_FF;
}

// ---
MCHEmul::InfoStructure FZ80::INTInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (FZ80::Interrupt::getInfoStructure ());

	result.add ("MODE", _INTMode);
	result.add ("DATABUS", _dataBusValue);

	return (result);
}

// ---
unsigned int FZ80::INTInterrupt::isTime (MCHEmul::CPU* c, unsigned int cC) const
{ 
	FZ80::CZ80* z80 = static_cast <FZ80::CZ80*> (c);

	return ((z80 -> IFF1 () && (z80 -> instAfterEIToLaunchINT () == 0)) 
		? MCHEmul::CPUInterrupt::_EXECUTIONALLOWED : MCHEmul::CPUInterrupt::_EXECUTIONNOTALLOWED);
}

// ---
bool FZ80::INTInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);
	
	FZ80::CZ80* c80 = static_cast <FZ80::CZ80*> (c);

	c80 -> resetHalt ();
	c80 -> incrementRegisterR (); // Accepting a INT interruption increments the R register in 1 too...

	// Additional execution belongs only to this response, including after mode changes.
	_cyclesAfterLaunch = 0;

	bool result = true;
	// To avoid further INTs until other thing was defined by the programmer...
	c80 -> setIFF2 (false); 
	c80 -> setIFF1 (false); 
	switch (_INTMode)
	{
		// In the type 0, the instruction with the code of the value in the data bus, is executed...
		// If it didn't exist, the method would return false...
		case 0:
			{
				MCHEmul::UByte nC = _dataBusValue;

				if ((nC.value () & 0xc7) == 0xc7)
				{
					// The opcode came from the acknowledge bus: preserve the return PC.
					// R has already been incremented once by interrupt acceptance.
					_exeAddress = c -> programCounter ().asAddress ();
					c -> memoryRef () -> stack () -> push (_exeAddress.bytes ());
					c -> programCounter ().setAddress
						(MCHEmul::Address (2, nC.value () & 0x38));
					// Launch contributes two cycles; the complete RST response takes thirteen.
					_cyclesAfterLaunch =
						(unsigned int) (_im1Timing.numberTCycles ()) - cyclesToLaunch ();
				}
				else
				{
					// Preserve execution of other injected instructions without claiming
					// support for their interrupt-response contention.
					_exeAddress = MCHEmul::Address ();
					if (!c -> existsInstruction ((unsigned int) nC.value ()))
						result = false;
					else
					{
						MCHEmul::Instruction* inst =
							c -> instruction ((unsigned int) nC.value ());
						result = inst -> execute
							(c, c -> memoryRef (), c -> memoryRef () -> stack (),
							 &c -> programCounter ());
						_cyclesAfterLaunch = inst -> clockCyclesExecuted ();
					}
				}
			}

			break;

		// In the type 1, the program jumps always to the same position
		case 1:
			{
				c -> memoryRef () -> stack () -> 
					push ((_exeAddress = c -> programCounter ().asAddress ()).bytes ());
	
				c -> programCounter ().setAddress (c80 -> INT1VectorAddress ());
			}

			break;

		// In the type 2, the program jumps into the direction defined 
		// by the value of the register I (high) and the value in the data bus (low)
		case 2:
			{
				c -> memoryRef () -> stack () -> 
					push ((_exeAddress = c -> programCounter ().asAddress ()).bytes ());

				// The first address is the table entry, not the service routine.
				// Read both bytes independently so $ffff wraps to $0000.
				MCHEmul::Address vA = c80 -> INT2VectorTableAddress (_dataBusValue);
				c -> programCounter ().setAddress (MCHEmul::Address
					({ c -> memoryRef () -> value (vA),
					   c -> memoryRef () -> value (vA.next (1)) },
					 false /** Little - endian */));
			}

			break;

		default:
			{ 
				// It should be here!
				_LOG ("Z80 INTerrupt mode not supported:" + 
					std::to_string ((int) _INTMode));
				assert (false); // Just when running in a debug compilation...
			}
	}
	
	return (result);
}
