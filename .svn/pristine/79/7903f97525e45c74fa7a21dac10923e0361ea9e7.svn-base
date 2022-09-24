#include <COMMS/Channel.hpp>
#include <BitStream.h>

// ---
MCHEmul::PeerCommunicationChannel::PeerCommunicationChannel (unsigned short p, unsigned int nC, const MCHEmul::IPAddress& to)
	: _listenAtPort (p), _simultaneousConnections (nC), _connectedTo (to),
	  _peer (nullptr),
	  _channelInitialized (false), _channelConnected (false),
	  _lastError (MCHEmul::_NOERROR)
{
	if ((_peer = RakNet::RakPeerInterface::GetInstance ()) == nullptr)
		_lastError = MCHEmul::_COMMSINTNOTCREATED;
}

// ---
MCHEmul::PeerCommunicationChannel::~PeerCommunicationChannel ()
{
	RakNet::RakPeerInterface::DestroyInstance (_peer);
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
			_lastError = MCHEmul::_COMMSNOTOPENED;

			return (false);
		}

		_channelInitialized = true;
	}

	if (!_channelConnected && _connectedTo != MCHEmul::IPAddress ())
	{
		if (_peer -> Connect (_connectedTo.ipAsString ().c_str (), _connectedTo.port (), nullptr, 0) 
				!= RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED)
		{
			_lastError = MCHEmul::_COMMSNOTOPENED;

			return (false);
		}
	}

	return (true);
}

// ---
bool MCHEmul::PeerCommunicationChannel::finalize ()
{
	_peer -> CloseConnection (_peer -> GetMyGUID (), true); // Informing about the disconnection...

	_channelConnected = false;
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
			// Usually client messages:
			case ID_DISCONNECTION_NOTIFICATION: 						// Connection lost normally.
			case ID_ALREADY_CONNECTED:									// Connection not posible because the system is already in.
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:						// The communication protocol is different than the one the server expects.
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:					// Other client has been disconnected.
			case ID_REMOTE_CONNECTION_LOST:								// Clients of another client disconnecting forcefully.
			case ID_REMOTE_NEW_INCOMING_CONNECTION:						// Clients of another client connecting
			case ID_CONNECTION_BANNED:									// Banned from this server
			case ID_CONNECTION_ATTEMPT_FAILED:							// Not posssible to reach the server. IP is right?
			case ID_NO_FREE_INCOMING_CONNECTIONS:						// Server is full. 
			case ID_INVALID_PASSWORD:								
			case ID_CONNECTION_LOST:									// Couldn't deliver a reliable packet (Server was terminated)
				_channelConnected = false;
				break;

			case DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED:	// The server has accepted the connection
				_channelConnected = true; // Noww the communication is possible...
				break;

			// Usually server messages:
			case ID_NEW_INCOMING_CONNECTION:							// Client is requested connection
			case ID_CONNECTED_PING:										// A Client is trying to ping
			case ID_UNCONNECTED_PING:									// A non client is trying to ping
				break;

			// Our important message...
			case _MESSAGEID: 
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn (packet -> data, packet -> length, false);
					bsIn.IgnoreBytes (sizeof (RakNet::MessageID));
					if (!(result = bsIn.Read (rs)))
						_lastError = MCHEmul::_CHANNELREADERROR;
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

	if (!_channelInitialized)
	{
		_lastError = MCHEmul::_COMMSNOTOPENED;

		return (false);
	}

	RakNet::BitStream bsOut;
	bsOut.Write ((RakNet::MessageID) _MESSAGEID);
	bsOut.Write (str.c_str ());
	RakNet::SystemAddress sA (to.ipAsString ().c_str (), to.port ());
	if (_peer -> Send (&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, sA, false) == 0)
	{
		_lastError = MCHEmul::_CHANNELWRITEERROR;

		return (false);
	}

	return (true);
}
