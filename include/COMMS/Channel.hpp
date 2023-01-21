/** \ingroup COMMS */
/*@{*/

/**	
 *	@file	
 *	File: Comms.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/06/2021 \n
 *	Description: Defines the tube though out one process communicates to other. 
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_COMMCHANNEL__
#define __MCHEMUL_COMMCHANNEL__

#include <CORE/incs.hpp>
#include <COMMS/IPAddress.hpp>
#include <RakPeerInterface.h>

namespace MCHEmul
{
	/** The class PeerCommunicationChannel allows the communication between 
		the emulator and other external elements: e.g. any external tool or a external debugger. \n
		This class manages many communications at the same time, acting as a server. */
	class PeerCommunicationChannel
	{
		public:
		/** 
		  * To create the communication channel, but not to open it. \n
		  *	@param p	: The communication channel needs to listen at a specific port. \n
		  * @param nC	: The default number of communications supported simultaneously.
		  */
		PeerCommunicationChannel (unsigned short p, unsigned int nC);

		PeerCommunicationChannel (const PeerCommunicationChannel&) = delete;

		PeerCommunicationChannel& operator = (const PeerCommunicationChannel&) = delete;

		virtual ~PeerCommunicationChannel ();

		PeerCommunicationChannel (PeerCommunicationChannel&&) = delete;

		PeerCommunicationChannel& operator = (PeerCommunicationChannel&&) = delete;

		// Channel data...
		unsigned short listenAtPort () const
							{ return (_listenAtPort); }
		unsigned short simulatenousConnections () const
							{ return (_simultaneousConnections); }

		// Mamanging the Status of the channel
		bool initialize ();
		bool isChannelInitiated ()
							{ return (_channelInitialized); }
		bool finalize ();

		// Managing the communication
		/** Receiving a communication. \
			@returns true when the communication has been well managed. 
			In that case "str" will hold the string received and "from" the address of the machine sending it through. */
		bool receive (std::string& str, IPAddress& from);
		/** To send a communication to another machine. \n
			The parameters needed are tyhe string to send it accross and the address of the machine to ssend it to. \n
			If the channel with the element required is not open yet, the communication is stored to be send later.
			@return true is everything went ok. */
		bool send (const std::string& str, const IPAddress& to);
		/** Send the pending messages, if any. \n
		A pending messages is generated when the communication with a target is not opened yet. \n
		The pennding messages list can be deleted. \n
		Returns true when all pending messages has been sent and false if not. */
		bool sendPendingMessages ();

		unsigned int error () const
							{ return (_error); }
		/** To simplify the way the errors are managed. */
		bool operator ! () const
							{ return (_error != MCHEmul::_NOERROR); }

		protected:
		/** To send just now the messages. \n
			This method is invoked from send and sendPendingMessages. */
		bool sendRightNow (const std::string& str, const IPAddress& to);

		protected:
		/** The post this channel is listen at. */
		const unsigned short _listenAtPort; 
		/** The number of maximum clients that can be connected against this. */
		const unsigned short _simultaneousConnections;

		/** This is required by RakNet library. */
		RakNet::RakPeerInterface* _peer;
		/** To point whether the channel has or not been initialized. */
		mutable bool _channelInitialized;
		/** To keep a track of all other channels connected against this one.
			Usually clients that want to execute actions on it. */
		mutable unsigned int _error;

		// Implementation
		/** To store the messages pending to be sent. */
		struct PendingMessage
		{
			PendingMessage () = default;

			PendingMessage (const std::string& msg, const IPAddress& to)
				: _message (msg), _to (to)
							{ }

			PendingMessage (std::string&& msg, const IPAddress& to) noexcept
				: _message (std::move (msg)), _to (to)
							{ }

			std::string _message;
			IPAddress _to;
		};

		using PendingMessages = std::vector <PendingMessage>;
		using StatusLinks = std::map <std::string /** The IP as string to look for it quicker. */, bool>;
		/** The list of pensing messages. */
		mutable PendingMessages _pendingMessages;
		/** The status of the connections. */
		mutable StatusLinks _statusLinks;
	};
}

#endif
  
// End of the file
/*@}*/
