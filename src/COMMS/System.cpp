#include <COMMS/System.hpp>
#include <COMMS/IPAddress.hpp>
#include <COMMS/AnsSysCommand.hpp>

// ---
bool MCHEmul::CommunicationSystem::initialize ()
{ 
	_messageSent = false;
	_messageReceived = false; 
	
	return (_communicationChannel -> initialize ()); 
}

// ---
bool MCHEmul::CommunicationSystem::processMessagesOn (MCHEmul::Computer* c)
{
	// If the system has errors, it cannot process messages...
	if (!*this)
		return (false);

	MCHEmul::CommandExecuter::executePendingCommands ();

	/** Before processing new messages, the old ones have to be sent. */
	if (!_communicationChannel -> sendPendingMessages ())
		return (false);

	std::string str;

	// Error receiving new messages?
	if (!_communicationChannel -> receive (str, _lastSender /** To store ho send the message. */))
		return (false);

	// Nothing in the content received
	if (str == "")
		return (true); // Nothing received, but executed...

	_messageReceived = true;
	_messageSent = false;

	// The received command can't be interpreted, 
	// so an error will be generated...
	str = MCHEmul::trim (str);
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
			MCHEmul::CommsSystemAnswerCommand* cA = static_cast <MCHEmul::CommsSystemAnswerCommand*> (c);
			MCHEmul::Command* oC = // Try to get the original command to use the right format to print it out!
				_commandExecuterForAnswers -> commandBuilder () -> command (cA -> lastCommandAnswerReceived ());
			_commandExecuterForAnswers -> manageAnswer ((oC != nullptr) ? oC : c, rst);
		}
		else
			std::cout << rst << std::endl; // A very last type of exit...
	}
	else
	{
		if (c != nullptr)
		{ 
			std::string h = 
				// The name of the command...
				MCHEmul::CommsSystemAnswerCommand::_NAME + " " + 
				// The first parameter is the formatter used...
				MCHEmul::CommsSystemAnswerCommand::_PARFORMATTER + "=" + _messageFormatter + " ";

			std::string msg = h + 
				// The second parameter is the name of the command which answer is being sent,
				MCHEmul::CommsSystemAnswerCommand::_PARORIGINALCMMD + "=" + c -> name () + " " +
				// ...and the third parameter is the data itself...
				MCHEmul::CommsSystemAnswerCommand::_PARANSWER + "=" + 
				MCHEmul::CommsSystemAnswerCommand::replaceCharsForComms  
					(FormatterBuilder::instance () -> formatter (_messageFormatter) -> format (rst));

			if (msg.length () >= 0xffff)
				msg = h + 
					"ERRROR=" + c -> name () + " " +
					// In this case the original command is change into an error...
					MCHEmul::CommsSystemAnswerCommand::_PARANSWER + "=" + 
					MCHEmul::CommsSystemAnswerCommand::replaceCharsForComms
						(FormatterBuilder::instance () -> formatter (_messageFormatter) -> format 
							(InfoStructure ({ { "ERROR", "Message to long" } }, { })));

			if (_communicationChannel -> send (msg, _lastSender))
			{ 
				_messageSent = true; 
				_messageReceived = false; 
			}
		}
	}
}
