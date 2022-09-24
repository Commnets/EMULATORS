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
		GetRegisterStatusMessage (const Attributes& a)
			: CommunicationMessage (a)
							{ }

		virtual bool executeOn (Computer*) override;
	};

	/** A message to get the data of the memory. */
	class GetMemoryDataMessage : public CommunicationMessage
	{
		public:
		GetMemoryDataMessage (const Attributes& a)
			: CommunicationMessage (a)
							{ }

		virtual bool executeOn (Computer*) override;
	};
}

#endif
  
// End of the file
/*@}*/
