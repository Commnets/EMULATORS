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
	class CommunicationSystem
	{
		public:
		/** To create a communication system it is needed a channel and
			a CommandBuilder to build the messages received as text,
			and the name of the formatter used to format the messages before sending them back through the line. */
		CommunicationSystem (PeerCommunicationChannel* cC, CommandBuilder* cB, 
				const std::string& msgFmter = "COMMS")
			: _communicationChannel (cC), _commandBuilder (cB),
			  _messageFormatter (msgFmter),
			  _error (false), _lastError (MCHEmul::_NOERROR)
							{ assert (_communicationChannel != nullptr && 
									  _commandBuilder != nullptr); }

		CommunicationSystem (const CommunicationSystem&) = delete;

		CommunicationSystem& operator = (const CommunicationSystem&) = delete;

		virtual ~CommunicationSystem ()
							{ delete (_communicationChannel); delete (_commandBuilder); }

		// Managing the life cycle of the communication system...
		// If something happens the variable _lastError sets to something different than _NOERROR...
		virtual bool initialize ()
							{ return (_communicationChannel -> initialize ()); }
		/** Through this method the computer return additional actions to guide the one invoking it.
			@see Computer for further details. Computer::_ACTIONNOTHING means to continue the cycles. */
		virtual unsigned int processMessagesOn (Computer* c);
		virtual bool finalize ()
							{ return (_communicationChannel -> finalize ()); }

		bool error () const
							{ return (_communicationChannel -> lastError ()); }

		bool operator ! () const
							{ return (!_communicationChannel); }

		protected:
		CommandBuilder* _commandBuilder;
		PeerCommunicationChannel* _communicationChannel;
		const std::string _messageFormatter;

		// Implementation
		bool _error;
		unsigned int _lastError;
	};
}

#endif
  
// End of the file
/*@}*/
