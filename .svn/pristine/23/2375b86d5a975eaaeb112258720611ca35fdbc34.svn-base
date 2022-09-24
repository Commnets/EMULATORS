#include <COMMS/Channel.hpp>
#include <BitStream.h>

// ---
MCHEmul::CommunicationChannel::CommunicationChannel (unsigned int p, unsigned int nC)
	: _server (true),
	  _data (MCHEmul::CommunicationChannel::ServerData (p, nC)),
	  _peer (nullptr),
	  _channelOpened (false),
	  _lastError (MCHEmul::_NOERROR)
{
	if ((_peer = RakNet::RakPeerInterface::GetInstance ()) == nullptr)
		_lastError = MCHEmul::_COMMSNOTCREATED;
}

// ---
MCHEmul::CommunicationChannel::CommunicationChannel (const MCHEmul::IPAddress& to)
	: _server (true),
	  _data (to),
	  _peer (nullptr),
	  _channelOpened (false),
	  _lastError (MCHEmul::_NOERROR)
{
	if ((_peer = RakNet::RakPeerInterface::GetInstance ()) == nullptr)
		_lastError = MCHEmul::_COMMSNOTCREATED;
}

// ---
MCHEmul::CommunicationChannel::~CommunicationChannel ()
{
	if (isChannelOpened ()) 
		closeChannel ();

	RakNet::RakPeerInterface::DestroyInstance (_peer);

	_peer = nullptr;
	
	_channelOpened = false;
}
	
// ---
bool MCHEmul::CommunicationChannel::openChannel ()
{
	if (_lastError != MCHEmul::_NOERROR)
		return (false);

	if (_channelOpened)
		return (true);

	if (_server)
	{
		RakNet::SocketDescriptor sd (_data._serverData._listenAtPort, 0);
		if (_peer -> Startup (_data._serverData._simultaneousConnections, &sd, 0) != RakNet::StartupResult::RAKNET_STARTED)
		{
			_lastError = MCHEmul::_COMMSNOTOPENED;

			return (false);
		}

		_channelOpened = true;
	}
	else
	{
		if (_peer -> Connect (_data._connectedTo.ipAsString ().c_str (), _data._connectedTo.port (), nullptr, 0) != 
			RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED)
		{
			_lastError = MCHEmul::_COMMSNOTOPENED;

			return (false);
		}

		// The channel is not opned until the confirmation from the server is rerceived (@see above)
	}

	return (true);
}

// ---
bool MCHEmul::CommunicationChannel::closeChannel ()
{
	if (_server)
		return (true); // Server can't be closed...

	_peer -> CloseConnection (_peer -> GetMyGUID (), true); // Informing about the disconnection...

	_channelOpened = false;

	return (true);
}

// ---
bool MCHEmul::CommunicationChannel::receiveString (std::string& str)
{
	// Both clients and servers receive in RakNet packages...
	// The difference is the type of packages that one and other can receive...

	bool result = true;

	RakNet::Packet* packet = NULL;
	while (packet = _peer -> Receive ())
	{
		switch (packet -> data [0]) // The first char has the id of the mesaage...
		{
			// A client has been disconnected...
			case DefaultMessageIDTypes::ID_REMOTE_DISCONNECTION_NOTIFICATION:
				break;

			// A client has lost the connection, what is similar by the way to the previous one...
			case DefaultMessageIDTypes::ID_REMOTE_CONNECTION_LOST:
				break;

			// A new customer has connected...
			case DefaultMessageIDTypes::ID_REMOTE_NEW_INCOMING_CONNECTION:
				break;

			// A new request of connection has been acepted...
			case DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED:
				_channelOpened = true;
				break;

			// A new request connection is incoming...
			case DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION:
				break;

			// The server can't acept more connections because it reached the max...
			case DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS:
				break;

			// The connection has been lost 
			// with either the clint or with the server, depending on which one received the communication)
			case DefaultMessageIDTypes::ID_CONNECTION_LOST:
				break;
				
			// Our message...
			case _MESSAGEID: 
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn (packet -> data, packet -> length, false);
					bsIn.IgnoreBytes (sizeof (RakNet::MessageID));
					if (!(result = bsIn.Read (rs)))
						_lastError = MCHEmul::_COMMSREADERROR;
					else
						str = rs.C_String (); // The string to be returned is then set...
				}

				break;
			
			default:
				assert (false);
		}
	}

	return (result);
}

// ---
bool MCHEmul::CommunicationChannel::sendString (const std::string& str)
{
	if (_server)
		return (true); // Server can't send comms... 

	// If it is client, but the channel is not still openen, nothing could be sent
	if (!_channelOpened)
	{
		_lastError = MCHEmul::_COMMSNOTOPENED;

		return (false);
	}

	// Use a BitStream to write a custom user message...
	// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
	RakNet::BitStream bsOut;
	bsOut.Write ((RakNet::MessageID) _MESSAGEID);
	bsOut.Write (str.c_str ());
	RakNet::SystemAddress sA (_data._connectedTo.ipAsString ().c_str (), _data._connectedTo.port ());
	_peer -> Send (&bsOut, IMMEDIATE_PRIORITY, UNRELIABLE_SEQUENCED, 0, sA, false);
	// UNREALIABLE_SEQUENCE means to use UDP

	return (true);
}
