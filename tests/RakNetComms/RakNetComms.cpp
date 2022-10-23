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

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

static std::string _COMMAND = "";
static RakNet::SystemAddress _SERVERADDRESS;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

void sendMessage (RakNet::RakPeerInterface* peer, const std::string& msg, const RakNet::SystemAddress& address)
{
	// Use a BitStream to write a custom user message
	// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
	RakNet::BitStream bsOut;
	std::cout << "sending:" << msg << std::endl;
	bsOut.Write ((RakNet::MessageID) ID_GAME_MESSAGE_1);
	bsOut.Write (msg.c_str ());
	peer -> Send (&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
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

		std::cout << bk.substr (0, _COMMAND.length ()) << _COMMAND;
	}

	return (result);
}

int main (void)
{
	char str [512];

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance ();
	bool isServer;
	RakNet::Packet* packet;

	std::cout << "(C) or (S)erver?" << std::endl;
	std::cin >> str;

	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		RakNet::SocketDescriptor sd;
		peer -> Startup (1, &sd, 1);
		isServer = false;
	}
	else 
	{
		RakNet::SocketDescriptor sd (SERVER_PORT, 0);
		peer -> Startup (MAX_CLIENTS, &sd, 1);
		isServer = true;
	}

	if (isServer)
	{
		std::cout << "Starting the server" << std::endl;
		// We need to let the server accept incoming connections from the clients
		peer -> SetMaximumIncomingConnections (MAX_CLIENTS);
	}
	else 
	{
		std::cout << "Enter server IP or hit enter for 127.0.0.1" << std::endl;
		std::cin >> str;
		if (str [0] == 0)
			strcpy_s (str, "127.0.0.1");
		std::cout << "Starting the client" << std::endl;
		peer -> Connect (str, SERVER_PORT, 0, 0);
	}

	// True when the client is ready!
	bool cReady = false;

	while (1)
	{
		// When client us ready more messages can be sent...
		if (cReady && readCommand ())
		{
			sendMessage (peer, _COMMAND, _SERVERADDRESS);
			_COMMAND = "";
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
						cReady = true;
						std::cout << "Our connection request has been accepted" << std::endl;
						sendMessage (peer, "STOP", _SERVERADDRESS = packet -> systemAddress);
					}
					break;

				case ID_NEW_INCOMING_CONNECTION:
					std::cout << "A connection is incoming" << std::endl;
					break;

				case ID_NO_FREE_INCOMING_CONNECTIONS:
					std::cout << "The server is full" << std::endl;
					break;

				case ID_DISCONNECTION_NOTIFICATION:
					std::cout << (isServer 
						? "A client has disconnected" : "We have been disconnected") << std::endl;
					break;

				case ID_CONNECTION_LOST:
					std::cout << (isServer 
						? "A client lost the connection" : "Connection lost") << std::endl;
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
