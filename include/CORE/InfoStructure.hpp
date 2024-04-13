/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: InfoStructure.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/11/2022 \n
 *	Description: Class to manage set of information.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_INFOSTRUCTURE__
#define __MCHEMUL_INFOSTRUCTURE__

#include <CORE/global.hpp>
#include <CORE/UByte.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/UInt.hpp>
#include <CORE/Address.hpp>

namespace MCHEmul
{
	/** Class to group the attributes of a class. \n
		It can hold also nested attributes. */
	class InfoStructure;
	using InfoStructures = std::map <std::string, InfoStructure>;
	class InfoStructure final
	{
		public:
		InfoStructure () = default;

		InfoStructure (const Attributes& a, const InfoStructures& iS)
			: _attributes (a), _infoStructures (iS)
							{ }

		InfoStructure (Attributes&& a, InfoStructures&& iS) noexcept
			: _attributes (std::move (a)), _infoStructures (std::move (iS))
							{ }

		/** Returns true when there is no information. */
		bool empty () const
							{ return (_attributes.empty () && _infoStructures.empty ()); }

		// Managing the attributes
		bool existsAttribute (const std::string& n) const;
		/** To get an attribute from the structure. \n
			When the data is not found a "" is returned. To look for a nested attribute uses '.' */
		std::string attribute (const std::string& n) const;
		const Attributes& attributes () const
							{ return (_attributes); }
		void add (const std::string& an, const std::string& av)
							{ _attributes.insert (Attributes::value_type (an, av)); }
		void add (std::string&& an, std::string&& av)
							{ _attributes.insert (Attributes::value_type (std::move (an), std::move (av))); }
		void add (const std::string& an, unsigned long long av)
							{ add (an, std::to_string (av)); }
		void add (const std::string& an, long long av)
							{ add (an, std::to_string (av)); }
		void add (const std::string& an, unsigned long av)
							{ add (an, std::to_string (av)); }
		void add (const std::string& an, long av)
							{ add (an, std::to_string (av)); }
		void add (const std::string& an, unsigned int av)
							{ add (an, std::to_string (av)); }
		void add (const std::string& an, int av)
							{ add (an, std::to_string (av)); }
		void add (const std::string& an, double av)
							{ add (an, std::to_string (av)); }
		void add (const std::string& an, bool av)
							{ add (an, av ? _YES : _NO); }
		void add (const std::string& an, const UByte& av)
							{ add (an, av.asString (UByte::OutputFormat::_HEXA, 2 /** default length of a byte in hexa notation. */)); }
		void add (const std::string& an, const UBytes& av)
							{ add (an, av.asString (UByte::OutputFormat::_HEXA, ',', 2)); }
		void add (const std::string& an, const std::vector <UByte>& av)
							{ add (an, UBytes (av)); }
		void add (const std::string&& an, std::vector <UByte>&& av)
							{ add (an, UBytes (std::move (av))); }
		void add (const std::string& an, const UInt& av)
							{ add (an, av.asString (UByte::OutputFormat::_HEXA, '\0', 2)); }
		void add (const std::string& an, const Address& av)
							{ add (an, av.asString (UByte::OutputFormat::_HEXA, '\0', 2)); }

		// Managing the nested info structures
		bool existsInfoStructure (const std::string& n) const;
		/** To get an infostructure from the structure. \n
			When the infostructure is not found an empty one is returned. To look for a nested attribute uses '.' */
		InfoStructure infoStructure (const std::string& n) const;
		const InfoStructures& infoStructures () const
							{ return (_infoStructures); }
		void add (const std::string& an, const InfoStructure& av)
							{ _infoStructures.insert (InfoStructures::value_type (an, av)); }
		void add (std::string&& an, InfoStructure&& av)
							{ _infoStructures.insert (InfoStructures::value_type (std::move (an), std::move (av))); }
		void add (const std::string& an, const Attributes& attrs)
							{ InfoStructure iS; for (const auto& i : attrs) iS.add (i.first, i.second); add (an, iS); }

		/** To delete either an Attribute or an InfoStructure if exists. */
		void remove (const std::string& an);

		/** 
		  * To convert the information stored here into a string. \n
		  * @param s	To separate the elements.
		  * @param se	To separate the name and the value. (see also pF).
		  * @param bI	Character to include at the beginning of the internal infostructures.
		  * @param bF	Character ro include at the end of those.
		  * @param pF	Wheher the name (the value is always included) has to be included as part of the result.
		  * @param wE	Text to consider when there is no data to include.
		  * @param bS	Every this number of registers printed out a carry return is added. \n
		  *				-1 if nothing has to be added, what is the default configuration.
		  */
		std::string asString (const std::string& s = "|", 
			const std::string& se = ":", const std::string& bI = "[", const std::string& bF = "]", bool pF = true, 
			const std::string& wE = "none" /** What to generate when the info structure is empty. */,
			int bs = -1) const;

		/** The opposite action. */
		static InfoStructure fromStr (const std::string& str,
			char s = '|', char se = ':', char bI = '[', char bF = ']');

		friend std::ostream& operator << (std::ostream& o, const InfoStructure& iS)
							{ return (o << iS.asString ()); /** very basic output. */ }

		private:
		Attributes _attributes;
		InfoStructures _infoStructures;
	};
}

#endif

// End of the file
/*@}*/