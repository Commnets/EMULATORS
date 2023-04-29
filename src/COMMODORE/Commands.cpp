#include <COMMODORE/Commands.hpp>
#include <COMMODORE/Computer.hpp>
#include <sstream>

const std::string COMMODORE::VICStatusCommand::_NAME = "CVICII";
const std::string COMMODORE::CIAStatusCommand::_NAME = "CCIA";
const std::string COMMODORE::SIDStatusCommand::_NAME = "CSID";
const std::string COMMODORE::DatasetteStatusCommand::_NAME = "CDATASETTE";
const std::string COMMODORE::SIDWrapperCommand::_NAME = "CSIDW";


// ---
void COMMODORE::VICStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> vicII () == nullptr)
		return;

	rst.add ("VICII", std::move (static_cast <COMMODORE::Computer*> (c) -> vicII () -> getInfoStructure ()));
}

// ---
void COMMODORE::CIAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> cia () == nullptr)
		return;

	rst.add ("CIA", std::move (static_cast <COMMODORE::Computer*> (c) -> cia () -> getInfoStructure ()));
}

// ---
void COMMODORE::SIDStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> sid () == nullptr)
		return;

	rst.add ("SID", std::move (static_cast <COMMODORE::Computer*> (c) -> sid () -> getInfoStructure ()));
}

// ---
void COMMODORE::SIDWrapperCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a C64 one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> sid () == nullptr)
		return;

	MCHEmul::SoundLibWrapper* wrapper = nullptr;
	switch (std::atoi (parameter ("00").c_str ()))
	{
		case 0:
			{
				wrapper = new COMMODORE::SoundRESIDWrapper
					(c -> clock ().cyclesPerSecond (), RESID::SAMPLE_FAST,
						static_cast <COMMODORE::Computer*> (c) -> sid () -> samplingFrecuency ());
			}

			break;

		case 1:
			{
				wrapper = new COMMODORE::SoundSimpleWrapper 
					(c -> clock ().cyclesPerSecond (),
					 static_cast <COMMODORE::Computer*> (c) -> sid () -> samplingFrecuency ());
			}

			break;

		default:
			break;
	}

	if (wrapper == nullptr)
		rst.add ("ERRORS", std::string ("Wrapper not defined"));
	else
		static_cast <COMMODORE::Computer*> (c) -> sid () -> setSoundWrapper (wrapper);
}

// ---
void COMMODORE::DatasetteStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	// Look for the datasette if any...
	MCHEmul::IOPeripherals prhs = std::move (c -> peripherals ()); // The list has been built up...
	COMMODORE::DatasettePeripheral* ds = nullptr;
	for (MCHEmul::IOPeripherals::const_iterator i = prhs.begin (); 
			i != prhs.end () && ds == nullptr; i++)
		ds = dynamic_cast <COMMODORE::DatasettePeripheral*> ((*i).second);

	if (ds != nullptr) rst = ds -> getInfoStructure ();
}
