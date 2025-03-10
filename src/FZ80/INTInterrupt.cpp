#include <FZ80/INTInterrupt.hpp>
#include <FZ80/CZ80.hpp>

// ---
MCHEmul::InfoStructure FZ80::INTInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (FZ80::Interrupt::getInfoStructure ());

	result.add ("MODE", _INTMode);

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
				_exeAddress = MCHEmul::Address (); // Not important!

				MCHEmul::UByte nC = c -> lastInstruction () -> INOUTData ()[0];
				if (!c -> existsInstruction ((unsigned int) nC.value ()))
					result = false; // The instruction doesn't exist!
				else
				{
					MCHEmul::Instruction* inst = c -> instruction ((unsigned int) nC.value ());
					result = inst -> execute 
						(c, c -> memoryRef (), c -> memoryRef () -> stack (), &c -> programCounter ());
					_cyclesAfterLaunch = inst -> clockCyclesExecuted (); // The ones that the instruction took!
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
	
				c -> programCounter ().setAddress (c80 -> INT2VectorAddress ());
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
