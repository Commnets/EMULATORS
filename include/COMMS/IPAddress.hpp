/** \ingroup COMMS */
/*@{*/

/**	
 *	@file	
 *	File: Address.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/06/2021 \n
 *	Description: Define the ip address needed to locate any communication channel.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_IPADDRESS__
#define __MCHEMUL_IPADDRESS__

#include <CORE/global.hpp>

namespace MCHEmul
{
	/** IP Address. V4 Format. Can have an error when creating from strring. */
	class IPAddress
	{
		public:
		IPAddress ()
			: _a (0), _b (0), _c (0), _d (0), _port (0), _error (true)
						{ }

		IPAddress (unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short p)
			: _a (a), _b (b), _c (c), _d (d), _port (p), _error (false)
						{ assert (_a <= 0xff && _b <= 0xff && _c < 0xff && _port <= 0xfff); }

		IPAddress (const std::string& ip);

		IPAddress (const IPAddress&) = default;

		IPAddress& operator = (const IPAddress&) = default;

		/** The parts of the ip address */
		unsigned char byte1 () const
						{ return (_a); }
		unsigned char byte2 () const
						{ return (_b); }
		unsigned char byte3 () const
						{ return (_c); }
		unsigned char byte4 () const
						{ return (_d); }
		unsigned short port () const
						{ return (_port); }

		std::string ipAsString () const
						{ return (std::to_string (_a) + "." + std::to_string (_b) + "." + std::to_string (_c)); }
		std::string asString () const
						{ return (ipAsString () + ":" + std::to_string (_port)); }

		bool error () const
						{ return (_error); }

		/** To make it more readable whether there is an error. */
		bool operator ! () const
						{ return (_error); }

		bool operator == (const IPAddress& a) const
						{ return (_a == a._a && _b == a._b && _c == a._c && _d == a._d && _port == a._port); }
		bool operator != (const IPAddress& a) const
						{ return (_a != a._a || _b != a._b || _c != a._c || _d != a._d || _port != a._port); }

		friend std::ostream& operator << (std::ostream& o, const IPAddress& a)
						{ return (o << ((a._error) ? "Error" : a.asString ())); }

		private:
		unsigned char _a, _b, _c, _d;
		unsigned short _port;
		mutable bool _error;
	};
}

#endif
  
// End of the file
/*@}*/
