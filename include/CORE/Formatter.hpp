/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: Formatter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/10/2022 \n
 *	Description: Defines an element to format fields and values into a string.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_FORMATTER__
#define __MCHEMUL_FORMATTER__

#include <CORE/global.hpp>
#include <CORE/InfoStructure.hpp>

namespace MCHEmul
{
	/** Class to format an InfoStructure. */
	class Formatter
	{
		public:
		Formatter () = default;

		Formatter (const Strings& l)
			: _lines (l)
							{ }

		Formatter (Strings&& l) noexcept
			: _lines (std::move (l))
							{ }

		Formatter (const Formatter&) = delete;

		Formatter& operator = (const Formatter&) = delete;

		virtual ~Formatter ()
							{ }

		Formatter (Formatter&&) = delete;

		Formatter& operator = (Formatter&&) = delete;

		/** To initialize the formatter.
			This method create all neceessary internal elements to manipulate the format. */
		virtual void initialize () = 0;

		/** To format a set of attributes into a string.
			It can be redefined to created specific formatters .eg. not based on pieces or JSON... */
		virtual std::string format (const InfoStructure& a) const = 0;
		/** To unformat an string into an InfoStructure. \n
		By default it is not possible, just a couple of Formatters could allow this. */
		virtual InfoStructure unFormat (const std::string& str) const = 0;

		protected:
		// Implementation
		/** The lines that makes up the formatter. */
		Strings _lines;
	};
}

#endif

// End of the file
/*@}*/