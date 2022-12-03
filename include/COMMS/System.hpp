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
			  _error (false), _lastError (MCHEmul::_NOERROR), _lastSender ()
							{ assert (_communicationChannel != nullptr && 
									  _commandBuilder != nullptr); }

		CommunicationSystem (const CommunicationSystem&) = delete;

		CommunicationSystem& operator = (const CommunicationSystem&) = delete;

		virtual ~CommunicationSystem ()
							{ delete (_communicationChannel); }

		/** To derive the answers received, to another CommandExecuter. */
		void deriveAnswerCommandTo (CommandExecuter* cE)
							{ _commandExecuterForAnswers = cE; }

		/** Just to send a message. */
		bool send (const std::string& str, const IPAddress& to)
							{ return (_communicationChannel -> send (str, to)); }

		// Managing the life cycle of the communication system...
		// If something happens the variable _lastError sets to something different than _NOERROR...
		virtual bool initialize ()
							{ return (_communicationChannel -> initialize ()); }
		/** The method returns true when everything was ok and false in other case. */
		virtual bool processMessagesOn (Computer* c);
		virtual bool finalize ()
							{ return (_communicationChannel -> finalize ()); }

		unsigned int error () const
							{ return (_communicationChannel -> lastError ()); }

		bool operator ! () const
							{ return (_error != _NOERROR); }

		protected:
		virtual void manageAnswer (Command *c, const InfoStructure& rst) override;
		virtual void manageErrorInExecution (Command* c, const InfoStructure& rst) override
							{ }

		protected:
		PeerCommunicationChannel* _communicationChannel;
		const std::string _messageFormatter;
		CommandExecuter* _commandExecuterForAnswers;

		// Implementation
		bool _error;
		unsigned int _lastError;
		MCHEmul::IPAddress _lastSender;
	};
}

#endif
  
// End of the file
/*@}*/
