#include <ASSEMBLER/StdOperations.hpp>

// ---
MCHEmul::UInt MCHEmul::Assembler::FunctionOperationElement::value 
	(const MCHEmul::Assembler::Macros& ms, const MCHEmul::Assembler::OperationParser* oP) const
{
	bool e = false;
	std::vector <MCHEmul::UInt> v;
	for (MCHEmul::Assembler::OperationElements::const_iterator i = _operationElements.begin ();
			i != _operationElements.end () && !e; i++) // Just one error and finishes...
		{ v.push_back ((*i) -> value (ms, oP)); e |= !*(*i); /** Error? */ }
	e |= numberParameters () != -1 && v.size () != numberParameters ();

	_error = e 
		? MCHEmul::Assembler::ErrorType::_BADARGUMENTFUNCTION : MCHEmul::Assembler::ErrorType::_NOERROR;
	return ((_error == ErrorType::_NOERROR) ? doFunction (v) : _whenError); 
}

// ---
MCHEmul::UInt MCHEmul::Assembler::DivideFunctionOperationElement::doFunction 
	(const std::vector <UInt>& v) const
{
	_error = (v [1] == MCHEmul::UInt::_0)
		? MCHEmul::Assembler::ErrorType::_DIVISIONBYCERO : MCHEmul::Assembler::ErrorType::_NOERROR;
	
	return ((_error == MCHEmul::Assembler::ErrorType::_NOERROR) 
		? v [0] / v [1] : _whenError); 
}

// ---
MCHEmul::UInt MCHEmul::Assembler::LSBFunctionOperationElement::doFunction 
	(const std::vector <MCHEmul::UInt>& v) const
{
	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	return (MCHEmul::UInt (v [0].values ().LSUBytes (1)));
}

// ---
MCHEmul::UInt MCHEmul::Assembler::MSBFunctionOperationElement::doFunction 
	(const std::vector <MCHEmul::UInt>& v) const
{
	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	return (MCHEmul::UInt (v [0].values ().MSUBytes (1)));
}
