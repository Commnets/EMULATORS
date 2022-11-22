#include <ASSEMBLER/Operation.hpp>

MCHEmul::UInt MCHEmul::Assembler::OperationElement::_whenError = MCHEmul::UInt::_0;

// ---
MCHEmul::UInt MCHEmul::Assembler::VariableOperationElement::value () const
{
	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	MCHEmul::Assembler::Macros::const_iterator i = _macros.find (_name);
	if (i == _macros.end ()) // The macro requested doesn't exist...
	{
		_error = MCHEmul::Assembler::ErrorType::_MACRONOTDEFINED;

		return (_whenError);
	}

	MCHEmul::UBytes r = (*i).second.value (_macros); // To get the value of the macro involved...
	_error = (*i).second.error (); // But getting this values an error can be generated and then dragged into this one...
	return ((_error == MCHEmul::Assembler::ErrorType::_NOERROR) 
		? MCHEmul::UInt (r) /** When no error, the set of bytes are converted into UInt. */ : _whenError);
}

// ---
MCHEmul::UInt MCHEmul::Assembler::FunctionOperationElement::value () const
{
	bool e = false;
	std::vector <MCHEmul::UInt> v;
	for (MCHEmul::Assembler::OperationElements::const_iterator i = _operationElements.begin ();
			i != _operationElements.end () && !e; i++) // Just one error and finishes...
		{ v.push_back ((*i) -> value ()); e |= !*(*i); /** Error? */ }

	_error = e 
		? MCHEmul::Assembler::ErrorType::_BADARGUMENTFUNCTION : MCHEmul::Assembler::ErrorType::_NOERROR;
	return ((_error == ErrorType::_NOERROR) ? doFunction (v) : _whenError); 
}

// ---
MCHEmul::UInt MCHEmul::Assembler::DivideFunctionOperationElement::doFunction 
	(const std::vector <UInt>& v) const
{
	_error = (v[1] == MCHEmul::UInt::_0)
		? MCHEmul::Assembler::ErrorType::_DIVISIONBYCERO : MCHEmul::Assembler::ErrorType::_NOERROR;
	
	return ((_error == MCHEmul::Assembler::ErrorType::_NOERROR) 
		? v [0] / v [1] : _whenError); 
}
