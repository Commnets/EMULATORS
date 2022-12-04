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
		JSONFormatter (const Strings& l)
			: Formatter (l),
			  _defEqual (":")
							{ }

		virtual void initialize () override
							{ }

		virtual std::string format (const InfoStructure& a) const override;
		virtual InfoStructure unFormat (const std::string& str) const override;

		private:
		std::string _defEqual;
	};
}

#endif

// End of the file
/*@}*/