#include <C264/C6529B1.hpp>
#include <C264/C6529B1Registers.hpp>
#include <C264/OSIO.hpp>

// ---
bool C264::C6529B1::initialize ()
{
	assert (memoryRef () != nullptr);

	// The parent is not initialized because the registers are not the parent ones...

	// Gets the memory block dedicated to this register...
	if (!(_C6529B1Registers =
		dynamic_cast <C264::C6529B1Registers*> (memoryRef () -> subset (C264::C6529B1Registers::_C6529B1REGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_C6529BRegisters = _C6529B1Registers;

	_C6529B1Registers -> initialize ();

	return (true);
}

// ---
void C264::C6529B1::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const C264::InputOSSystem::Keystrokes& ks = 
					((C264::InputOSSystem*) n) -> keystrokesFor // Will depend on the implementation...
						(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_C6529B1Registers -> setKeyboardStatusMatrix (j.first, j.second, true);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const C264::InputOSSystem::Keystrokes& ks = 
					((C264::InputOSSystem*) n) -> keystrokesFor // Will depend on the implementation...
						(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_C6529B1Registers -> setKeyboardStatusMatrix (j.first, j.second, false);
			}

			break;

		default:
			break;
	}
}
