#include <CORE/CPUArchitecture.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
MCHEmul::CPUArchitecture::CPUArchitecture (size_t nb, size_t iL, bool bE, const Attributes& attrs)
	: MCHEmul::InfoClass ("CPUArchitecture"),
	  _numberBytes (nb), _instructionLength (iL), _numberBits ((size_t) (nb << 3)), _bigEndian (bE), _attributes (attrs), 
	  _longestRegisterPossible (0, "-", MCHEmul::UBytes (std::vector <MCHEmul::UByte> (nb, MCHEmul::UByte::_0))) // The id is an imagination...
{ 
	assert (_numberBytes > 0 && 
		    _numberBytes <= MCHEmul::_MAXBYTESMANAGED && // The maximum byte length supported
		    (_numberBits >> 3) == _numberBytes &&  // To b sure everything matches
		    _instructionLength <= _numberBytes); 
}

// ---
MCHEmul::InfoStructure MCHEmul::CPUArchitecture::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ATTRS",			_attributes);
	result.add ("NBYTES",			_numberBytes);
	result.add ("NBITS",			_numberBits);
	result.add ("NBYTESPERINST",	_instructionLength);
	result.add ("ENDIAN",			_bigEndian);

	return (result);
}
