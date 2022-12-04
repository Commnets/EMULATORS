#include <COMMS/Channel.hpp>
#include <MessageIdentifiers.h>
#include <BitStream.h>

// ---
MCHEmul::PeerCommunicationChannel::PeerCommunicationChannel (unsigned short p, unsigned int nC)
	: _listenAtPort (p), _simultaneousConnections (nC),
	  _peer (nullptr),
	  _channelInitialized (false),
	  _error (MCHEmul::_NOERROR),
	  _pendingMessages (), _statusLinks ()
{
	if ((_peer = RakNet::RakPeerInterface::GetInstance ()) == nullptr)
		_error = MCHEmul::_COMMSINTNOTCREATED_ERROR;
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
	if (_error != MCHEmul::_NOERROR)
		return (false);

	if (!_channelInitialized)
	{
		RakNet::SocketDescriptor sd (_listenAtPort, 0);
		if (_peer -> Startup (_simultaneousConnections, &sd, 1) != RakNet::StartupResult::RAKNET_STARTED)
		{
			_error = MCHEmul::_COMMSNOTOPENED_ERROR;

			return (false);
		}

		_peer -> SetMaximumIncomingConnections (_simultaneousConnections);

		_channelInitialized = true;
	}

	_pendingMessages = { };
	_statusLinks = { };

	return (true);
}

// ---
bool MCHEmul::PeerCommunicationChannel::finalize ()
{
	_peer -> CloseConnection (_peer -> GetMyGUID (), true); // Informing about the disconnection...

	_channelInitialized = false;

	return (true);
}

// ---
bool MCHEmul::PeerCommunicationChannel::receive (std::string& str, MCHEmul::IPAddress& from)
{
	if (!*this)
		return (false);

	if (!_channelInitialized)
	{
		_error = MCHEmul::_COMMSNOTOPENED_ERROR;

		return (false);
	}

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
			// A message has been received...
			case (ID_USER_PACKET_ENUM + 1):
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn (packet -> data, packet -> length, false);
					bsIn.IgnoreBytes (sizeof (RakNet::MessageID));
					if (!(result = bsIn.Read (rs)))
						_error = MCHEmul::_CHANNELREADERROR_ERROR;
					else
						str += rs.C_String (); // The string to be returned is then set...
											   // The string can be splitted in many!
				}

				break;

			// The connection against other element has been accepted...
			// Or other external element is requesting a connection aginst me!
			case ID_NEW_INCOMING_CONNECTION:							// Element is requesting connection.
			case ID_CONNECTION_REQUEST_ACCEPTED:						// A connection has been accepted
				{ 
					MCHEmul::PeerCommunicationChannel::StatusLinks::iterator i = 
						_statusLinks.find (from.asString ());
					if (i != _statusLinks.end ()) (*i).second = true;
					else _statusLinks.insert (MCHEmul::PeerCommunicationChannel::StatusLinks::value_type (from.asString (), true));
				}

				break;

			// Related with the connection with the server...
			case ID_DISCONNECTION_NOTIFICATION: 						// Element disconnected.
			case ID_CONNECTION_LOST:									// Element connection lost.
				{
					MCHEmul::PeerCommunicationChannel::StatusLinks::iterator i = 
						_statusLinks.find (from.asString ());
					if (i != _statusLinks.end ()) (*i).second = false;
					else _statusLinks.insert (MCHEmul::PeerCommunicationChannel::StatusLinks::value_type (from.asString (), false));
					// This last situation should happen, 
					// as this message is the result of a previous already stablished connection. 
					// That is, the element might exist already, but...
				}

				break;

			// No other messages are managed...
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:					// Element has disconnected.
			case ID_REMOTE_CONNECTION_LOST:								// Element has lost the communication.
			case ID_REMOTE_NEW_INCOMING_CONNECTION:						// Another element connection has been done.
			case ID_NO_FREE_INCOMING_CONNECTIONS:						// This element is full. 
			case ID_CONNECTED_PING:										// An element is trying to ping.
			case ID_UNCONNECTED_PING:									// A element is trying to ping.
			case ID_ALREADY_CONNECTED:									// Connection not posible because the system is already in.
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:						// The communication protocol is different than the one the server expects.
			case ID_CONNECTION_BANNED:									// Banned from this element.
			case ID_CONNECTION_ATTEMPT_FAILED:							// Not posssible to reach the element. IP is right?
			case ID_INVALID_PASSWORD:
			default:
				break;
		}
	}

	return (result);
}

// ---
bool MCHEmul::PeerCommunicationChannel::send (const std::string& str, const MCHEmul::IPAddress& to)
{
	if (!*this)
		return (false);

	// If the full channel has not been initialized yet, no messages can be sent
	// and the method returns false y finishes!
	if (!_channelInitialized)
	{
		_error = MCHEmul::_COMMSNOTOPENED_ERROR;

		return (false);
	}

	bool result = true;
	// If the link with the destination is already established, then send the message right now...
	MCHEmul::PeerCommunicationChannel::StatusLinks::const_iterator i = _statusLinks.find (to.asString ());
	if (i != _statusLinks.end () && (*i).second /** it is connected. */)
		result = sendRightNow (str, to);
	// ...other case, launches the connection and store the message to be sent
	// later when the conecction is established...
	else
	{ 
		if ((result = _peer -> Connect 
				(to.asString ().c_str (), to.port (), nullptr, 0) == RakNet::CONNECTION_ATTEMPT_STARTED))
			_pendingMessages.push_back (MCHEmul::PeerCommunicationChannel::PendingMessage (str, to));
		else
			_error = MCHEmul::_CHANNELWRITEERROR_ERROR;
	}

	return (result);
}

// ---
bool MCHEmul::PeerCommunicationChannel::sendPendingMessages ()
{
	bool result = true;

	for (MCHEmul::PeerCommunicationChannel::PendingMessages::iterator i = _pendingMessages.begin ();
			i != _pendingMessages.end ();)
	{
		MCHEmul::PeerCommunicationChannel::StatusLinks::const_iterator j = _statusLinks.find ((*i)._to.asString ());
		if (j == _statusLinks.end () || !(*j).second) // Open?
			i++; // ..no, so it cann't be sent...
		else
		{
			// It might be sent as the communication link is open already...
			// But the result variable will keep the real result!
			result &= sendRightNow ((*i)._message, (*i)._to);
			i = _pendingMessages.erase (i); // The iterator is actualized...
		}
	}

	return (result);
}

// ---
bool MCHEmul::PeerCommunicationChannel::sendRightNow (const std::string& str, const MCHEmul::IPAddress& to)
{
	bool result = true;

	RakNet::BitStream bsOut;
	bsOut.Write ((RakNet::MessageID) (ID_USER_PACKET_ENUM + 1));
	bsOut.Write (MCHEmul::removeAll0 (str).c_str ()); // All 0 are take off before being transmitted through internet...
	RakNet::SystemAddress sA (to.ipAsString ().c_str (), to.port ());
	if (_peer -> Send (&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, sA, false) == 0)
	{
		_error = MCHEmul::_CHANNELWRITEERROR_ERROR;

		result = false;
	}

	return (result);
}
