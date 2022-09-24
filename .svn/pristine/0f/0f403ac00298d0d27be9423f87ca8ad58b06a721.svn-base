/** \ingroup COMMS */
/*@{*/

/**	
 *	@file	
 *	File: Messages.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/06/2021 \n
 *	Description: The message flowing through a communication channel, and the builder needed to create them.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_COMMMESSAGE__
#define __MCHEMUL_COMMMESSAGE__

#include <CORE/incs.hpp>
#include <COMMS/IPAddress.hpp>

namespace MCHEmul
{
	class Computer;

	/** The CommunicationMessage class referes to the different instructions a client
		can sent to a server or a server can answer to a client on ergard to processing an emulator. */
	class CommunicationMessage
	{
		public:
		CommunicationMessage (unsigned char t, const Attributes& a)
			: _type (t), _attributes (a)
							{ }

		/** To avoid uncontrolable behaviours of any one inheriting from this. */
		CommunicationMessage (const CommunicationMessage&) = delete;

		CommunicationMessage& operator = (const CommunicationMessage&) = delete;

		virtual ~CommunicationMessage ()
							{ }

		unsigned char type () const
							{ return (_type); }

		const Attributes attributes () const
							{ return (_attributes); }

		std::string toString () const;

		/** The very important method. \n
			This method execute what was required from the computer. \n
			It must be overloaded per any specific behaviour. \n
			The communication message can have an answer, and it is given in the second parameter. */
		virtual bool executeOn (Computer*, CommunicationMessage*&) = 0;

		protected:
		/** The type of the message. */
		unsigned int _type;
		/** The attributes of the message. */
		Attributes _attributes;

		// Implementation
		bool _error;
	};

	/** 
	  * A builder class to create the messages. \n
	  * An instance of this class is created in the Computer. \n
	  * The structure of the message should be as follow: \n
	  * 1 byte (char) indicating the code of the message. \n
	  * 1 byte (char) indicating the number of parameters after. \n
	  * a string indicating the ip (and port) origin of the message.
	  * X bytes with the parameters of the message. 
	  * Usually that parameters are in the form NAMEATTR=VALUE. \n
	  * That parameters should be separated using commas. \n
	  * Neither names nor values can have neither equal nor comma sign in the middle.
	  */
	class MessageBuilder
	{
		public:
		MessageBuilder ()
							{ }

		/** To avoid later uncontrolable behaviours. */
		MessageBuilder (const MessageBuilder&) = delete;

		MessageBuilder& operator = (const MessageBuilder&) = delete;

		virtual ~MessageBuilder ()
							{ }

		virtual CommunicationMessage* createMessage (const std::string&) = 0;

		protected:
		// Implementation
		/** To verify the right structure of the message rerceived. */
		bool verifyStructure (const std::string& str);
		/** To create the list of the parameters any message will need from a string. */
		Attributes attributesFromStr (const std::string& str) const;
	};
}

#endif
  
// End of the file
/*@}*/
