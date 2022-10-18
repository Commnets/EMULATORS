/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: StdFmterBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 18/10/2022 \n
 *	Description: The standard formatter builder to create standard formatters.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STDFMTERBUILDER__
#define __MCHEMUL_STDFMTERBUILDER__

#include <CORE/FmterBuilder.hpp>
#include <CORE/StdFormatter.hpp>

namespace MCHEmul
{
	class StdFormatterBuilder : public FormatterBuilder
	{
		public:
		StdFormatterBuilder (const Strings& nF)
			: FormatterBuilder (nF)
							{ }

		protected:
		virtual Formatter* createFormatter (const std::string& f, const std::string& nF, const Strings& l) override
							{ return (new StdFormatter (l)); }
	};
}

#endif

// End of the file
/*@}*/
