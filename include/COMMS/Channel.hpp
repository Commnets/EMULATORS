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
		This class manages many communications at the same time. */
	class PeerCommunicationChannel
	{
		public:
		/** 
		  * To create the communication channel, but not to open it. \n
		  *	@param p	: The communication channel needs to listen at a specific port. \n
		  * @param nC	: The default number of communications supported.
		  */
		PeerCommunicationChannel (unsigned short p, unsigned int nC);

		PeerCommunicationChannel (const PeerCommunicationChannel&) = delete;

		PeerCommunicationChannel& operator = (const PeerCommunicationChannel&) = delete;

		virtual ~PeerCommunicationChannel ();

		// Channel data...
		unsigned short listenAtPort () const
							{ return (_listenAtPort); }
		unsigned short simulatenousConnections () const
							{ return (_simultaneousConnections); }
		bool connectedTo (const IPAddress& a)
							{ return (_channelsConnected.find (a) != _channelsConnected.end ()); }

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
		/** To send the pending messages.
			Returns true if everything work ok, and false when a message was not possible to be sent. */
		bool sendPendingMessages ();

		unsigned int lastError () const
							{ return (_lastError); }

		bool operator ! () const
							{ return (_lastError != MCHEmul::_NOERROR); }

		protected:
		/** To connect against another element. \n
			This method only starts the connection but ir doesn't finish it. \n
			Return true when the method to open the connection went ok and false in other case. */
		bool connectTo (const IPAddress& a);

		protected:
		/** The post this channel is listen at. */
		const unsigned short _listenAtPort; 
		/** The number of maximum clients that can be connected against this. */
		const unsigned short _simultaneousConnections;
	
		// Implementation
		struct MsgToSend
		{
			MsgToSend (const std::string& msg, const IPAddress& a)
				: _message (msg), _address (a)
								{ }

			std::string _message;
			IPAddress _address;
		};

		/** This is required by RakNet library. */
		RakNet::RakPeerInterface* _peer;
		/** To point whether the channel has or not been initialized. */
		mutable bool _channelInitialized;
		/** To keep a track of all other channels connected against this one.
			Usually clients that want to execute actions on it. */
		mutable std::map <IPAddress, bool> _channelsConnected;
		mutable std::queue <MsgToSend> _messagesToSend;
		mutable unsigned int _lastError;
	};
}

#endif
  
// End of the file
/*@}*/
