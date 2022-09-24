/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: IO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: Emulating the input / output.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_IO__
#define __MCHEMUL_IO__

#include <CORE/Chip.hpp>
#
namespace MCHEmul
{
	class IODevice
	{
		public:
		enum class Type { _INPUT, _OUTPUT };

		IODevice () = delete;

		IODevice (Type t, int id, const Attributes& attrs = { })
			: _type (t), _id (id), _chips (), _attributes ()
							{ }

		IODevice (const IODevice&) = delete;

		IODevice& operator = (const IODevice&) = delete;

		/** The device doesn't own the chips, only work with them. */
		virtual ~IODevice () 
							{ }

		Type type () const
							{ return (_type); }

		int id () const
							{ return (_id); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }
		/** To link to chips. 
			The IO device never owns the chips. */
		virtual void linkToChips (const Chips& c)
							{ _chips = c; }
		const Chips& chips () const
							{ return (_chips); }

		/** To initialize the device. */
		virtual bool initialize () = 0;
		/** To emulate the way it works. \n
			Return true, if everything was ok. */
		virtual bool refresh () = 0;

		friend std::ostream& operator << (std::ostream& o, const IODevice& d);

		protected:
		const Type _type; // Modified at constrution level
		const int _id; // Idem
		Chips _chips; // linked when computer instance is built!
		const Attributes _attributes = { }; // Maybe modified at construction level
	};

	/** To simplify the management of a map of devices. */
	using IODevices = std::map <int, IODevice*>;
}

#endif
  
// End of the file
/*@}*/
