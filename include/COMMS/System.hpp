/** \ingroup COMMS */
/*@{*/

/**	
 *	@file	
 *	File: CommsSys.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/06/2021 \n
 *	Description: To manage th full communication system.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_COMMSSYS__
#define __MCHEMUL_COMMSSYS__

#include <CORE/incs.hpp>
#include <COMMS/Channel.hpp>

namespace MCHEmul
{
	class Computer;

	/** The communication system is what makes messages to flow from a computer to another. */
	class CommunicationSystem : public CommandExecuter
	{
		public:
		static const int _ID = -2;

		CommunicationSystem () = delete;

		/** 
		  *	To create a communication system it is needed:
		  * @param cC	a Channel to communicate.
		  * @param cB	A Command Builder to execute the commands received. \n
		  *				After the execution of a command, its answer is sent back encapsulated 
		  *				in the command "CommandExecuteAnswerCommand". \n
		  * msgFmter	The formatter used to send back the answer of a command. By default is JSON. 
		  */
		CommunicationSystem (PeerCommunicationChannel* cC, CommandBuilder* cB, 
				const std::string& msgFmter = "JSON" /** @see JSONFormatter to send back the answers */)
			: CommandExecuter (_ID, cB),
			  _communicationChannel (cC),
			  _messageFormatter (msgFmter),
			  _commandExecuterForAnswers (nullptr), // No derivation possible...
			  _lastSender (),
			  _messageSent (false),
			  _messageReceived (false) // Becomes true when a message is received...
							{ assert (_communicationChannel != nullptr && 
									  _commandBuilder != nullptr); }

		CommunicationSystem (const CommunicationSystem&) = delete;

		CommunicationSystem& operator = (const CommunicationSystem&) = delete;

		virtual ~CommunicationSystem () override
							{ delete (_communicationChannel); }

		CommunicationSystem (CommunicationSystem&&) = delete;

		CommunicationSystem& operator = (CommunicationSystem&&) = delete;

		/** To know and change the message formatter using to send the answer to messages
			when this communication system is acting as a CommandExecuter. */
		const std::string& messageFormatter () const
							{ return (_messageFormatter); }
		void setMessageFormatter (const std::string& mF)
							{ _messageFormatter = mF; }

		/** To derive the answers received, to another CommandExecuter. \n
			Usually the Communication Systems is part of a "major" CommandExecuter, like a Console. */
		void deriveAnswerCommandTo (CommandExecuter* cE)
							{ _commandExecuterForAnswers = cE; }

		/** Just to send a message. */
		bool send (const std::string& str, const IPAddress& to)
							{ _messageSent = true; _messageReceived = false;
							  return (_communicationChannel -> send (str, to)); }
		/** Returns true when a message has been sent. */
		bool messageSent () const
							{ return (_messageSent); }

		// Managing the life cycle of the communication system...
		// If something happens the variable _error sets to something different than _NOERROR...
		virtual bool initialize ();
		/** The method returns true when everything was ok and false in other case. */
		virtual bool processMessagesOn (Computer* c);
		/** Returns true when a message has just recived after processinf the messages. */
		bool messageReceived () const
							{ return (_messageReceived); }
		virtual bool finalize ()
							{ return (_communicationChannel -> finalize ()); }

		/** To know whether the system is waiting for an answer. */
		bool waitingForAnswer () const
							{ return (_messageSent && !_messageReceived); }

		unsigned int error () const
							{ return (_communicationChannel -> error ()); }
		/** To simplify the way the errors are managed. */
		bool operator ! () const
							{ return (!*_communicationChannel); }

		virtual void manageAnswer (Command *c, const InfoStructure& rst) override;
		virtual void manageErrorInExecution (Command* c, const InfoStructure& rst) override
							{ }

		protected:
		PeerCommunicationChannel* _communicationChannel;
		std::string _messageFormatter;
		CommandExecuter* _commandExecuterForAnswers;

		// Implementation
		MCHEmul::IPAddress _lastSender;
		bool _messageSent;
		bool _messageReceived;
	};
}

#endif
  
// End of the file
/*@}*/
