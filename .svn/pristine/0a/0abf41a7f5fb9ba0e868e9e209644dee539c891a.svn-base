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
#include <MessageIdentifiers.h>

namespace MCHEmul
{
	/** The class PeerCommunicationChannel allows the communication between pieces of the emulator.
		e.g. emulator and any external tool. \n
		It could be useful e.g. to run in debug mode the emulator or to know 
		which is the status of the computer. */
	class PeerCommunicationChannel
	{
		public:
		/** The id of the package having the specific meesages to control emulator. */
		static const unsigned char _MESSAGEID = 
			DefaultMessageIDTypes::ID_USER_PACKET_ENUM + 1;

		/** 
		  * To create the communication channel, but not to open it. \n
		  *	@param p	: The communication channel needs to listen at a specific port.
		  * @param nC	: The default number of communications supported. 1 default for a pure peer comm.
		  * @param to	: The direction this peer is connected to, if any. It could be nothing!
		  */
		PeerCommunicationChannel (unsigned short p, unsigned int nC, const IPAddress& a = IPAddress ());

		PeerCommunicationChannel (const PeerCommunicationChannel&) = delete;

		PeerCommunicationChannel& operator = (const PeerCommunicationChannel&) = delete;

		virtual ~PeerCommunicationChannel ();

		// Channel data...
		unsigned short listenAtPort () const
							{ return (_listenAtPort); }
		unsigned short simulatenousConnections () const
							{ return (_simultaneousConnections); }
		const IPAddress& connectedTo () const // IPAddress () when none...
							{ return (_connectedTo); }

		// Mamanging the Status of the channel
		bool initialize ();
		bool isChannelInitiated ()
							{ return (_channelInitialized); }
		bool isChannelConnected ()
							{ return (_channelConnected); }
		bool finalize ();

		// Managing the communication
		/** Receiving a communication. \
			@returns true when the communication has been well managed. 
			In that case "str" will hold the string received and "from" the address of the machine sending it through. */
		bool receive (std::string& str, IPAddress& from);
		/** To send a communication to another machine. 
			The parameters needed are tyhe string to send it accross and the address of the machine to ssend it to. 
			@return true is everything went ok. */
		bool send (const std::string& str, const IPAddress& to);
		/** To send a communication to the connected machine if any. 
			Otherwise an error is generated. */
		bool send (const std::string& str);

		unsigned int lastError () const
							{ return (_lastError); }

		bool operator ! () const
							{ return (_lastError != MCHEmul::_NOERROR); }

		protected:
		const unsigned short _listenAtPort; 
		const unsigned short _simultaneousConnections;
		const IPAddress _connectedTo;
	
		// Implementation
		RakNet::RakPeerInterface* _peer;
		mutable bool _channelInitialized;
		mutable bool _channelConnected;
		mutable unsigned int _lastError;
	};
}

#endif
  
// End of the file
/*@}*/
