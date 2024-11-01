#include <CORE/LogChannel.hpp>
#include <fstream>

// ---
std::unique_ptr <MCHEmul::LogSystem> MCHEmul::LogSystem::_LOGSYSTEM = 
	std::unique_ptr <MCHEmul::LogSystem> (new MCHEmul::LogSystem ({ }));

// ---
void MCHEmul::LogChannel::write (const std::string& str)
{ 
	writeImplementation (str);

	if (++_counter == _numberLines)
	{
		restartLogChannel ();

		_counter = 0;
	}
}

// ---
MCHEmul::LogFileChannel::LogFileChannel (int id, const std::string& fN, unsigned int nL)
	: MCHEmul::LogChannel (id, nL),
	  _fileName (fN),
	  _logFile ()
{
	_logFile.open (_fileName, std::ios::out);
}

// ---
void MCHEmul::LogFileChannel::writeImplementation (const std::string& str)
{
	if (_logFile.is_open ())
		_logFile << str << std::endl; // Only if it is opened...
}

// ---
void MCHEmul::LogFileChannel::restartLogChannel ()
{
	if (_logFile.is_open ())
	{
		_logFile.close ();
		_logFile.open (_fileName, std::ios::out);
	}
}

// ---
MCHEmul::LogSystem::~LogSystem ()
{ 
	for (const auto& i : _logChannels)
		delete (i.second);

	delete (_STDLOGCHANNEL); 
}
