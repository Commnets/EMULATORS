#include <ASSEMBLER/Operation.hpp>

MCHEmul::UInt MCHEmul::Assembler::OperationElement::_whenError = MCHEmul::UInt::_0;

// ---
MCHEmul::UInt MCHEmul::Assembler::VariableOperationElement::value 
(const MCHEmul::Assembler::Macros& ms, const MCHEmul::Assembler::OperationParser* oP) const
{
	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	MCHEmul::Assembler::Macros::const_iterator i = ms.find (_name);
	if (i == ms.end ()) // The macro requested doesn't exist...
	{
		_error = MCHEmul::Assembler::ErrorType::_MACRONOTDEFINED;

		return (_whenError);
	}

	MCHEmul::UBytes r = (*i).second.value (ms, oP); // To get the value of the macro involved...
	_error = (*i).second.error (); // But getting this values an error can be generated and then dragged into this one...
	return ((_error == MCHEmul::Assembler::ErrorType::_NOERROR) 
		? MCHEmul::UInt (r) /** When no error, the set of bytes are converted into UInt. */ : _whenError);
}
