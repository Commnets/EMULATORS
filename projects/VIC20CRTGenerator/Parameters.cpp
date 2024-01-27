#include "Parameters.hpp"

const VIC20::Parameter VIC20::Parameters::_NONE = 
	VIC20::Parameter (std::shared_ptr <VIC20::ParameterTemplate> (new VIC20::none_ParameterTemplate ()));

// ---
bool VIC20::Parameter::addValue (const std::string& v)
{ 
	MCHEmul::Strings nVal (_values); 
	nVal.emplace_back (v); 
	
	bool result = _template -> validValues (nVal);
	if (result)
		_values = nVal;
	return (result);
}

// ---
bool VIC20::Parameters::iterate (const std::function <bool(const VIC20::Parameter&)>& f) const
{ 
	bool ok = true; 
	for (const auto& i : _parameters) 
		ok &= f (i.second); // Execute the function over each parameter...
							// If any mistake happens the ok = false...
	return (ok); 
}
