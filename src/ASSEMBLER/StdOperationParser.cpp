#include <ASSEMBLER/StdOperationParser.hpp>
#include <ASSEMBLER/StdOperations.hpp>

// ---
MCHEmul::Assembler::OperationElement* MCHEmul::Assembler::StdOperationParser::createBinaryOperationElement
	(unsigned char s, MCHEmul::Assembler::OperationElement* l, MCHEmul::Assembler::OperationElement* r) const
{
	MCHEmul::Assembler::OperationElement* result = nullptr;
	switch (s)
	{
		case '+':
			result = new MCHEmul::Assembler::AddFunctionOperationElement (l, r);
			break;

		case '-':
			result = new MCHEmul::Assembler::SubtractFunctionOperationElement (l, r);
			break;

		case '*':
			result = new MCHEmul::Assembler::MultiplyFunctionOperationElement (l, r);
			break;

		case '/':
			result = new MCHEmul::Assembler::DivideFunctionOperationElement (l, r);
			break;

		default:
			break;
	}

	return (result);
}

// ---
MCHEmul::Assembler::OperationElement* MCHEmul::Assembler::StdOperationParser::createUnaryOperationElement
	(unsigned char s, MCHEmul::Assembler::OperationElement* o) const
{
	MCHEmul::Assembler::OperationElement* result = nullptr;
	switch (s)
	{
		case '>':
			result = new MCHEmul::Assembler::LSBFunctionOperationElement (o);
			break;

		case '<':
			result = new MCHEmul::Assembler::MSBFunctionOperationElement (o);
			break;

		default:
			break;
	}

	return (result);
}
