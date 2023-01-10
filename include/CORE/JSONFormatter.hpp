/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: JSONFormatter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 18/10/2022 \n
 *	Description: Defines a formatter to create JSON structures.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_JSONFORMATTER__
#define __MCHEMUL_JSONFORMATTER__

#include <CORE/Formatter.hpp>

namespace MCHEmul
{
	/** The JSON formatter. \n
		The format is always:
		{ \n
		ATTRSET:{ \n
		ATTR:VALUE \n
		... \n
		{ \n
		... \n
		} \n
		} \n
		}
		The separator beetween the attr name and it value can be redefined, by default it is a ":".
	  */
	class JSONFormatter final : public Formatter
	{
		public:
		JSONFormatter (const Strings& l, char dE = ':')
			: Formatter (l),
			  _defEqual (dE)
							{ }

		JSONFormatter (Strings&& l, char dE = ':') noexcept
			: Formatter (std::move (l)),
			  _defEqual (dE)
							{ }

		virtual void initialize () override
							{ }

		char defEqual () const
							{ return (_defEqual); }
		void setDefEqual (char dE)
							{ _defEqual = dE; }

		virtual std::string format (const InfoStructure& a) const override;
		virtual InfoStructure unFormat (const std::string& str) const override;

		private:
		char _defEqual;
	};
}

#endif

// End of the file
/*@}*/