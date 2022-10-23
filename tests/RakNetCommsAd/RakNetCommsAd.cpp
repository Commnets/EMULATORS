// RakNetClient.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "Gets.h"
#include "RakNetTypes.h"  // MessageID
#include <queue>

#define MAX_CLIENTS 2
#define EMULATOR_IP "127.0.0.1"

unsigned int _SERVER_PORT;
std::string _COMMAND = "";
bool _connectedToEmulator = false;
RakNet::SystemAddress _EMULATOR_ADDRESS;
unsigned int _EMULATOR_PORT;
struct MsgTo
{
	MsgTo (const std::string& msg, const RakNet::SystemAddress& sA)
		: _message (msg), _address (sA)
							{ }

	std::string _message;
	RakNet::SystemAddress _address;
};

std::queue <MsgTo> _PENDINGMESSAGES;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

void sendPendingMesssages (RakNet::RakPeerInterface* peer)
{
	while (!_PENDINGMESSAGES.empty ())
	{
		MsgTo msg = _PENDINGMESSAGES.front ();
		_PENDINGMESSAGES.pop ();

		RakNet::BitStream bsOut;
		bsOut.Write ((RakNet::MessageID)ID_GAME_MESSAGE_1);
		bsOut.Write (msg._message.c_str ());
		std::cout << "Sending:" << msg._message << std::endl;
		peer -> Send (&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, msg._address, false);
	}
}

void sendMessage (RakNet::RakPeerInterface* peer, const std::string& msg, const RakNet::SystemAddress& address)
{
	if (!_connectedToEmulator)
	{
		peer -> Connect (EMULATOR_IP, _EMULATOR_PORT, nullptr, 0);

		_PENDINGMESSAGES.push (MsgTo (msg, address));
	}
	else
	{
		_PENDINGMESSAGES.push (MsgTo (msg, address));

		sendPendingMesssages (peer);
	}
}

#ifdef _WIN32
#include <conio.h>
bool readChar (char& chr)
{
	bool result = false;
	if (result = (_kbhit () != 0))
	{
		int k = _getch ();
		if (k == 0 || k == 0xe0)
			k += _getch ();
		chr = static_cast <char> (k);
	}

	return (result);
}
#endif

bool readCommand ()
{
	static const std::string bk (200, '\b');

	bool result = false;

	char chr;
	if (readChar (chr))
	{
		if (chr >= 0 &&
			(std::isalnum ((int) chr) || chr == ' ' || chr == '$'))
			_COMMAND += chr;

		if (chr == 13)
			result = true;

		std::cout << bk.substr (0, _COMMAND.length () - 1) << _COMMAND;
	}

	return (result);
}

int main (void)
{
	std::string sPort;
	std::cout << "This Server Port:";
	std::cin >> sPort;
	_SERVER_PORT = (unsigned int) std::atoi (sPort.c_str ());

	std::string ePort;
	std::cout << "Emulator Port:";
	std::cin >> ePort;
	_EMULATOR_PORT = (unsigned int)  std::atoi (ePort.c_str ());

	_EMULATOR_ADDRESS = RakNet::SystemAddress (EMULATOR_IP, _EMULATOR_PORT);

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance ();
	RakNet::Packet* packet;
	RakNet::SocketDescriptor sd (_SERVER_PORT, 0);
	peer -> Startup (MAX_CLIENTS, &sd, 1);
	std::cout << "Starting the communications" << std::endl;
	peer -> SetMaximumIncomingConnections (MAX_CLIENTS);

	std::cout << "Introduce Command:";
	while (1)
	{
//		sendPendingMesssages (peer);
		if (readCommand ())
		{
			sendMessage (peer, _COMMAND, _EMULATOR_ADDRESS);
			_COMMAND = "";
			std::cout << std::endl;
			std::cout << "Introduce Command:";
		}

		for (packet = peer -> Receive (); packet; 
				peer -> DeallocatePacket (packet), packet = peer -> Receive ())
		{
			switch (packet -> data [0])
			{
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					std::cout << "Another client has disconnected" << std::endl;
					break;

				case ID_REMOTE_CONNECTION_LOST:
					std::cout << "Another client has lost the connection" << std::endl;
					break;

				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					std::cout << "Another client has connected" << std::endl;
					break;

				case ID_CONNECTION_REQUEST_ACCEPTED:
					{
						_connectedToEmulator = true;
						sendPendingMesssages (peer);
						std::cout << "Our connection request has been accepted" << std::endl;
					}

					break;

				case ID_NEW_INCOMING_CONNECTION:
					std::cout << "A connection is incoming" << std::endl;
					break;

				case ID_NO_FREE_INCOMING_CONNECTIONS:
					std::cout << "The server is full" << std::endl;
					break;

				case ID_DISCONNECTION_NOTIFICATION:
					std::cout << "We have been disconnected" << std::endl;
					break;

				case ID_CONNECTION_LOST:
					std::cout << "Connection lost" << std::endl;
					break;

				case ID_GAME_MESSAGE_1:
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn (packet -> data, packet -> length, false);
					bsIn.IgnoreBytes (sizeof (RakNet::MessageID));
					bsIn.Read (rs);
					std::cout << "Recibido:" << rs.C_String () << std::endl;
				}
				break;

				default:
					std::cout << "Message with identifier: " << packet -> data [0] << " has arrived" << std::endl;
					break;
			}
		}
	}

	RakNet::RakPeerInterface::DestroyInstance (peer);

	return (0);
}
