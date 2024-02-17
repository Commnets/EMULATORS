/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: StdFmterBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 18/10/2022 \n
 *	Description: The standard formatter builder to create standard formatters.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STDFMTERBUILDER__
#define __MCHEMUL_STDFMTERBUILDER__

#include <CORE/FmterBuilder.hpp>
#include <CORE/StdFormatter.hpp>
#include <CORE/JSONFormatter.hpp>

namespace MCHEmul
{
	/** The standard formatter builder, creates the nasic formatter.
		All of them will the Standard except when the requested formatter if type JSON. */
	class StdFormatterBuilder : public FormatterBuilder
	{
		public:
		StdFormatterBuilder (const Strings& nF)
			: FormatterBuilder (nF)
							{ }

		protected:
		/** In the standard formatter the file where there formatter has been defined is not taken into account. \n
			Just the name of the formatter invoked. */
		virtual Formatter* createFormatter (const std::string& f, const std::string& nF, const Strings& l) override
							{ return ((nF == "JSON" // JSON is taking different...
								? (Formatter*) new JSONFormatter (l) : (Formatter*) new StdFormatter (l))); }
	};
}

#endif

// End of the file
/*@}*/
