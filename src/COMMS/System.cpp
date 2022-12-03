#include <COMMS/System.hpp>
#include <COMMS/IPAddress.hpp>
#include <COMMS/AnsSysCommand.hpp>

// ---
bool MCHEmul::CommunicationSystem::processMessagesOn (MCHEmul::Computer* c)
{
	MCHEmul::CommandExecuter::executePendingCommands ();

	/** Before processing new messages, the old ones have to be sent. */
	if (!_communicationChannel -> sendPendingMessages ())
		return (false);

	// The system hasn't been initialized well!
	if (!*this)
		return (false);

	std::string str;

	// Error receiving new messages?
	if (!_communicationChannel -> receive (str, _lastSender /** To store ho send the message. */))
		return (false);

	// Nothing in the content received
	if (str == "")
		return (true); // Nothing received, but executed...

	// The received command can't be interpreted, 
	// so an error will be generated...
	str = MCHEmul::upper (MCHEmul::trim (str));
	MCHEmul::Command* cmd = commandBuilder () -> command (str);
	if (cmd == nullptr)
		return (false); 

	return (executeCommandNow (cmd, c));
}

// ---
void MCHEmul::CommunicationSystem::manageAnswer (MCHEmul::Command* c, const MCHEmul::InfoStructure& rst)
{
	// If the command being managed is the answer 
	// to a command that has initially been sent through the line
	// then it could be moved up...other wise it is deleted...
	if (dynamic_cast <MCHEmul::CommsSystemAnswerCommand*> (c) != nullptr)
	{
		if (_commandExecuterForAnswers != nullptr)
		{
			MCHEmul::MoveParametersToAnswerCommand* mCmd = dynamic_cast <MCHEmul::MoveParametersToAnswerCommand*>
				(_commandExecuterForAnswers -> commandBuilder () -> command (MoveParametersToAnswerCommand::_NAME));
			if (mCmd != nullptr)
			{
				mCmd -> setParameters (MCHEmul::Attributes ({ { "RESULT", rst.attribute ("RESULT") } }));

				_commandExecuterForAnswers -> executeCommand (mCmd, nullptr);
			}
		}
		else
			std::cout << rst << std::endl; // A very last type of exit...
	}
	else
	{
		if (!rst.empty ())
		{
			if (!_communicationChannel -> send (MCHEmul::CommsSystemAnswerCommand::_NAME + " " + 
				MCHEmul::CommsSystemAnswerCommand::replaceCharsForComms 
					(FormatterBuilder::instance () -> formatter (_messageFormatter) -> format (rst)), _lastSender))
				_lastError = MCHEmul::_CHANNELWRITEERROR_ERROR;
		}
	}
}
