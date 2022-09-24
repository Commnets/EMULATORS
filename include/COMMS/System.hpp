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

#include <CORE/global.hpp>
#include <COMMS/Channel.hpp>
#include <COMMS/Message.hpp>

namespace MCHEmul
{
	class Computer;

	class CommunicationSystem
	{
		public:
		CommunicationSystem (PeerCommunicationChannel* cC, MessageBuilder* mB)
			: _communicationChannel (cC), _messageBuilder (mB),
			  _error (false), _lastError (MCHEmul::_NOERROR)
							{ assert (_communicationChannel != nullptr && _messageBuilder != nullptr); }

		CommunicationSystem (const CommunicationSystem&) = delete;

		CommunicationSystem& operator = (const CommunicationSystem&) = delete;

		virtual ~CommunicationSystem ()
							{ delete (_communicationChannel); delete (_messageBuilder); }

		// Managing the system...
		virtual bool initialize ()
							{ return (_communicationChannel -> initialize ()); }
		/** The method returns a potential additional info to the caller. */
		virtual unsigned int processMessagesOn (Computer* c);
		virtual bool finalize ()
							{ return (_communicationChannel -> finalize ()); }

		bool error () const
							{ return (_communicationChannel -> lastError ()); }

		bool operator ! () const
							{ return (!_communicationChannel); }

		protected:
		MessageBuilder* _messageBuilder;
		PeerCommunicationChannel* _communicationChannel;

		// Implementation
		bool _error;
		unsigned int _lastError;
	};
}

#endif
  
// End of the file
/*@}*/
