#include <iostream>

#include <COMMS/Channel.hpp>
#include <COMMS/Message.hpp>
#include <COMMS/StdMessages.hpp>

#define __MAXCOMMANDLEN__			60

int main ()
{
	MCHEmul::PeerCommunicationChannel* comms =
		new MCHEmul::PeerCommunicationChannel (200, 1, MCHEmul::IPAddress ("127.0.0.1:100"));
	if (!*comms || !comms -> initialize ())
	{
		delete (comms);

		return (0);
	}

	MCHEmul::MessageBuilder* mB = new MCHEmul::StandardMessageBuilder;
	char cmdTxt [__MAXCOMMANDLEN__];
	bool exit = false;
	while (!exit)
	{
		std::cout << ">> ";
		std::cin.getline (cmdTxt, __MAXCOMMANDLEN__);
		std::string cmdStr = MCHEmul::upper (MCHEmul::trim (cmdTxt));
		bool p = false;

		if (cmdStr == "QUIT")
		{
			exit = true;

			continue;
		}

		MCHEmul::CommunicationMessage* msg = mB -> createMessage (cmdStr);
		if (msg == nullptr)
			continue;

		std::string answ = "";
		MCHEmul::IPAddress from;
		comms -> send (*msg);
		comms -> receive (answ, from);
		if (answ != "")
			std::cout << answ << std::endl;
	}

	delete (mB);

	comms -> finalize ();
	delete (comms);

	return (0);

}
