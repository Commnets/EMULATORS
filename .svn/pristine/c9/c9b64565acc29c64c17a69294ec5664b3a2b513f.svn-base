/** \ingroup COMMS */
/*@{*/

/**	
 *	@file	
 *	File: StdMessages.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/06/2021 \n
 *	Description: The standard messages defined.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STDMESSAGES__
#define __MCHEMUL_STDMESSAGES__

#include <COMMS/Message.hpp>

namespace MCHEmul
{
	/** The standard message builder. \n
		This class instantiates the default message supported by the standard MCHEmul library. */
	class StandardMessageBuilder : public MessageBuilder
	{
		public:
		StandardMessageBuilder ()
			: MessageBuilder ()
							{ }

		virtual CommunicationMessage* createMessage (const std::string& str) override;
	};

	/** A message to get the status of the internal registers. */
	class GetRegisterStatusMessage : public CommunicationMessage
	{
		public:
		static const unsigned char _ID = 'A';

		GetRegisterStatusMessage (const Attributes& a)
			: CommunicationMessage (_ID, a)
							{ }

		virtual bool executeOn (Computer* c, CommunicationMessage*& ans) override;
	};

	/** A message to get the data of the memory. */
	class GetMemoryDataMessage : public CommunicationMessage
	{
		public:
		static const unsigned char _ID = 'B';

		GetMemoryDataMessage (const Attributes& a)
			: CommunicationMessage (_ID, a)
							{ }

		virtual bool executeOn (Computer* c, CommunicationMessage*& ans) override;
	};
}

#endif
  
// End of the file
/*@}*/
