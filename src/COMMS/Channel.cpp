#include <COMMS/Channel.hpp>
#include <MessageIdentifiers.h>
#include <BitStream.h>

// ---
MCHEmul::PeerCommunicationChannel::PeerCommunicationChannel (unsigned short p, unsigned int nC)
	: _listenAtPort (p), _simultaneousConnections (nC),
	  _peer (nullptr),
	  _channelInitialized (false), 
	  _channelsConnected (),
	  _lastError (MCHEmul::_NOERROR)
{
	if ((_peer = RakNet::RakPeerInterface::GetInstance ()) == nullptr)
		_lastError = MCHEmul::_COMMSINTNOTCREATED_ERROR;
}

// ---
MCHEmul::PeerCommunicationChannel::~PeerCommunicationChannel ()
{
	RakNet::RakPeerInterface::DestroyInstance (_peer);

	// And all communications will be closed if any...
}
	
// ---
bool MCHEmul::PeerCommunicationChannel::initialize ()
{
	if (_lastError != MCHEmul::_NOERROR)
		return (false);

	if (!_channelInitialized)
	{
		RakNet::SocketDescriptor sd (_listenAtPort, 0);
		if (_peer -> Startup (_simultaneousConnections, &sd, 1) != RakNet::StartupResult::RAKNET_STARTED)
		{
			_lastError = MCHEmul::_COMMSNOTOPENED_ERROR;

			return (false);
		}

		_peer -> SetMaximumIncomingConnections (_simultaneousConnections);

		_channelInitialized = true;
	}

	return (true);
}

// ---
bool MCHEmul::PeerCommunicationChannel::finalize ()
{
	_peer -> CloseConnection (_peer -> GetMyGUID (), true); // Informing about the disconnection...

	_channelsConnected = { };
	_channelInitialized = false;

	return (true);
}

// ---
bool MCHEmul::PeerCommunicationChannel::receive (std::string& str, MCHEmul::IPAddress& from)
{
	bool result = true; // By default the result will be true...
 
	RakNet::Packet* packet = nullptr;
	while (packet = _peer -> Receive ())
	{
		// The sender address is always kept...
		char chr [100];
		packet -> systemAddress.ToString_Old (true, chr);
		from = MCHEmul::IPAddress (chr);

		// The first char has the id of the mesaage...
		switch (packet -> data [0]) 
		{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:					// Element has disconnected.
			case ID_REMOTE_CONNECTION_LOST:								// Element has lost the communication.
			case ID_REMOTE_NEW_INCOMING_CONNECTION:						// Another element connection has been done.
			case ID_NEW_INCOMING_CONNECTION:							// Element is requesting connection.
			case ID_NO_FREE_INCOMING_CONNECTIONS:						// This element is full. 
			case ID_DISCONNECTION_NOTIFICATION: 						// Element disconnected.
			case ID_CONNECTION_LOST:									// Element connection lost.
			case ID_CONNECTED_PING:										// An element is trying to ping.
			case ID_UNCONNECTED_PING:									// A element is trying to ping.
			case ID_ALREADY_CONNECTED:									// Connection not posible because the system is already in.
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:						// The communication protocol is different than the one the server expects.
			case ID_CONNECTION_BANNED:									// Banned from this element.
			case ID_CONNECTION_ATTEMPT_FAILED:							// Not posssible to reach the element. IP is right?
			case ID_INVALID_PASSWORD:								
				break;

			// When the server accept the connection...
			case DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED:	// The connection has been accepted
				_channelsConnected [from] = true;
				break;

			// A message has been received...
			case (DefaultMessageIDTypes::ID_USER_PACKET_ENUM + 1):
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn (packet -> data, packet -> length, false);
					bsIn.IgnoreBytes (sizeof (RakNet::MessageID));
					if (!(result = bsIn.Read (rs)))
						_lastError = MCHEmul::_CHANNELREADERROR_ERROR;
					else
						str += rs.C_String (); // The string to be returned is then set...
											   // The string can be splitted in many!
				}

				break;
	
			// No other messages are managed...
			default:
				result = false;
		}
	}

	return (result);
}

// ---
bool MCHEmul::PeerCommunicationChannel::send (const std::string& str, const MCHEmul::IPAddress& to)
{
	if (!*this)
		return (false);

	bool result = true;

	if (!_channelInitialized)
	{
		_lastError = MCHEmul::_COMMSNOTOPENED_ERROR;

		return (false);
	}

	// If the destination is already connected...
	if (connectedTo (to))
	{
		// Adds the message...
		_messagesToSend.push (MCHEmul::PeerCommunicationChannel::MsgToSend (str, to));
		// ...and then send all pending messages...
		result = sendPendingMessages ();
	}
	// Otherwise...
	else
	{
		// First send the connection try...
		if ((result = connectTo (to)))
			// ...but keeps the message to be sent when possible...
			_messagesToSend.push (MCHEmul::PeerCommunicationChannel::MsgToSend (str, to));
	}

	return (result);
}

// ---
bool MCHEmul::PeerCommunicationChannel::sendPendingMessages ()
{
	bool result = true;

	while (!_messagesToSend.empty ())
	{ 
		MCHEmul::PeerCommunicationChannel::MsgToSend msg = _messagesToSend.front ();
		_messagesToSend.pop ();

		// Just in case...
		if (!connectedTo (msg._address))
			continue;

		RakNet::BitStream bsOut;
		bsOut.Write ((RakNet::MessageID)DefaultMessageIDTypes::ID_USER_PACKET_ENUM + 1);
		bsOut.Write (msg._message.c_str ());
		RakNet::SystemAddress sA (msg._address.ipAsString ().c_str (), msg._address.port ());
		if (_peer -> Send (&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, sA, false) == 0)
		{
			_lastError = MCHEmul::_CHANNELWRITEERROR_ERROR;

			result = false;
		}
	}

	return (result);
}

// ---
bool MCHEmul::PeerCommunicationChannel::connectTo (const MCHEmul::IPAddress& a)
{
	if (connectedTo (a))
		return (true);

	if (_peer -> Connect (a.ipAsString ().c_str (), a.port (), nullptr, 0)
		!= RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED)
	{
		_lastError = MCHEmul::_COMMSNOTOPENED_ERROR;

		return (false);
	}

	return (true);
}
