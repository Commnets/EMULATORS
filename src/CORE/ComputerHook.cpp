#include <CORE/ComputerHook.hpp>
#include <CORE/ComputerAction.hpp>
#include <CORE/Computer.hpp>

// ---
const MCHEmul::Strings MCHEmul::ComputerMemoryAccessHook::_INFO = {
	"Code: 0",
	"Description: Stop the computer when a range of memory positions is acceses (read/write)",
	"Parameters:",
	"=> InitAddress (madatory): The initial address of the range",
	"=> FinalAddress (optional): The final address of the range" } ;

// ---
MCHEmul::ComputerHook::ComputerHook (unsigned int id, int type, MCHEmul::ComputerAction* cA)
	: MCHEmul::InfoClass ("ComputerHook"),
	  _id (id),
	  _type (type),
	  _action (cA), 
	  _actionId (std::numeric_limits <unsigned int>::max ())
{
	// Nothing else
}

// ---
MCHEmul::ComputerHook::ComputerHook (unsigned int id, int type, unsigned int cAId )
	: MCHEmul::InfoClass ("ComputerHook"),
	  _id (id),
	  _type (type),
	  _action (nullptr),
	  _actionId (cAId)
{
	// Nothing else
}

// ---
bool MCHEmul::ComputerHook::verifyAndExecuteAction (Computer* c)
{ 
	return (conditionReached (c) 
		? (_action != nullptr) 
			? _action -> execute (c)
			: (c -> existTemplaceAction (_actionId)
				? c -> templateAction (_actionId) -> execute (c)
				: false)
		: false); 
}

// ---
MCHEmul::InfoStructure MCHEmul::ComputerHook::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID", _id);
	result.add ("TYPE", _type);
	result.add ("ACTION", 
		(_action != nullptr) 
			? std::to_string (_action -> id ()) 
			: (_actionId != std::numeric_limits <unsigned int>::max () 
				? std::to_string (_actionId) : "-"));
	result.add ("INFO", asString ());

	return (result);

}

// ---
MCHEmul::ComputerMemoryAccessHook::ComputerMemoryAccessHook 
		(unsigned int id, const MCHEmul::Address& start, const MCHEmul::Address& end, MCHEmul::ComputerAction* cA)
	: ComputerHook (id, _TYPE, cA),
	  _start (start), _end (end)
{ 
	assert (end >= start);
}

// ---
MCHEmul::ComputerMemoryAccessHook::ComputerMemoryAccessHook
		(unsigned int id, const MCHEmul::Address& start, const MCHEmul::Address& end, unsigned int cAId)
	: ComputerHook (id, _TYPE, cAId),
	  _start (start), _end (end),
	  _lastAddress ()
{
	assert (end >= start);
}

// ---
bool MCHEmul::ComputerMemoryAccessHook::conditionReached (MCHEmul::Computer* c)
{
	const MCHEmul::Address& lA = c -> cpu () -> lastINOUTAddress ();

	bool cR = lA >= _start && lA <= _end;

	bool result = false;
	if (cR && (_lastAddress != lA))
	{
		result = true;

		_lastAddress = lA;
	}

	return (result);
}
	
// ---
std::string MCHEmul::ComputerMemoryAccessHook::asString () const
{
	return (
		"Hook between $" + 
		MCHEmul::removeAll0 (_start.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) +
		" and $" + 
		MCHEmul::removeAll0 (_end.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) +
		" with action " + 
			((_action != nullptr) ? std::to_string (_action -> id ()) 
				: ((_actionId != std::numeric_limits <unsigned int>::max ()) 
					? std::to_string (_actionId) : "no action")));
}

// ---
MCHEmul::ComputerHooksPool::ComputerHooksPool ()
	: MCHEmul::InfoClass ("ComputerHooksPool"),
	  _hooks ()
{
	// Nothing else to do
}

// ---
MCHEmul::ComputerHooksPool::~ComputerHooksPool ()
{
	for (const auto& i : _hooks)
		delete (i.second);
}

// ---
std::tuple <MCHEmul::ComputerHook*, std::string> 
	MCHEmul::ComputerHooksPool::hook (unsigned int id, int t, const Strings& strs) const
{
	MCHEmul::ComputerHook* hook = nullptr;
	std::string err = "";

	// Lookf for the hook...
	MCHEmul::ComputerHooksMap::const_iterator i = _hooks.find (id);
	if (i != _hooks.end ())
		hook = (*i).second; // If exists, it is returnes, whatever the other attributes were!
	else
	{
		std::tie (hook, err) = createHook (id, t, strs);
		// If the hook is finally created, 
		// it is added to the list of those controlled by the pool
		if (hook != nullptr)
			_hooks.insert (MCHEmul::ComputerHooksMap::value_type (id, hook));
	}

	return (std::tuple <MCHEmul::ComputerHook*, std::string> (hook, err));
}

// ---
void MCHEmul::ComputerHooksPool::removeHook (unsigned int id)
{
	MCHEmul::ComputerHooksMap::const_iterator i = _hooks.find (id);
	if (i != _hooks.end ())
		_hooks.erase (i);
}

// ---
MCHEmul::Strings MCHEmul::ComputerHooksPool::help () const
{
	MCHEmul::Strings result;

	// The info for ComputerMemoryAccessHook
	result.push_back ({
		MCHEmul::concatenateStrings (MCHEmul::ComputerMemoryAccessHook::_INFO, "\n") });
	// There is none else so far...

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::ComputerHooksPool::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	// Not expected more than 100 (0 to 99) hooks...
	size_t ct = 0;
	MCHEmul::InfoStructure hks;
	for (const auto& i : _hooks)
		hks.add (MCHEmul::fixLenStr (std::to_string (ct++), 2, true, MCHEmul::_CEROS), 
			i.second -> getInfoStructure ());
	result.add ("Hooks", hks);

	return (result);
}

// ---
std::tuple <MCHEmul::ComputerHook*, std::string> 
	MCHEmul::ComputerHooksPool::createHook (unsigned int id, int t, const Strings& prms) const
{
	MCHEmul::ComputerHook* hook = nullptr;
	std::string err = "";

	switch (t)
	{
		// Memory hook, stopping the execution of the computer...
		case MCHEmul::ComputerMemoryAccessHook::_TYPE:
			{
				if (prms.size () == 0) err = "Insuficient mumber of parameters";
				else if (prms.size () > 2) err = "Too much arguments";
				else
				{
					MCHEmul::Address iA = MCHEmul::Address::fromStr (prms [0]);
					MCHEmul::Address fA = (prms.size () == 2)
						? MCHEmul::Address::fromStr (prms [1])
						: iA;
					hook = new MCHEmul::ComputerMemoryAccessHook 
						(id, iA, fA, MCHEmul::Computer::_ACTIONSTOP);
				}
			}

			break;

		// This type of hook is no supported so far...
		default:
			{
				err = "Hook type not supported";
			}

			break;
	}

	return (std::tuple <MCHEmul::ComputerHook*, std::string> (hook, err));
}
