#include <CORE/CPUArchitecture.hpp>

// ---
MCHEmul::CPUArchitecture::CPUArchitecture (size_t nb, size_t iL, bool bE, const Attributes& attrs)
	: _numberBytes (nb), _instructionLength (iL), _numberBits ((size_t) (nb << 3)), _bigEndian (bE), _attributes (attrs), 
	  _longestRegisterPossible (0, "-", MCHEmul::UBytes (std::vector <MCHEmul::UByte> (nb, MCHEmul::UByte::_0))) // The id is an imagination...
{ 
	assert (_numberBytes > 0 && 
		    _numberBytes <= MCHEmul::_MAXBYTESMANAGED && // The maximum byte length supported
		    (_numberBits >> 3) == _numberBytes &&  // To b sure everything matches
		    _instructionLength <= _numberBytes); 
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::CPUArchitecture& a)
{
	o << "---" << std::endl;
	o << "CPU Architecture Info" << std::endl;
	o << a.numberBytes () << " bytes" << std::endl;
	o << a.numberBits () << " bits" << std::endl;
	o << a.instructionLength () << " bytes per instruction" << std::endl;
	o << (a.bigEndian () ? "big endian" : "little endian") << std::endl;
	o << a.attributes ();

	return (o);
}
