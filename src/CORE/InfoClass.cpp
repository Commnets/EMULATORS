#include <CORE/InfoClass.hpp>

// ---
MCHEmul::InfoStructure MCHEmul::InfoClass::getInfoStructure () const
{ 
	MCHEmul::InfoStructure result; 
	
	result.add ("CLASSNAME", className ()); 
	
	return (result); 
}
