/** \ingroup CPU */
/*@{*/

/**
 *	@file
 *	File: FmterBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/10/2022 \n
 *	Description: A builder to create formatters.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_FMTERBUILDER__
#define __MCHEMUL_FMTERBUILDER__

#include <CORE/global.hpp>
#include <CORE/Formatter.hpp>

namespace MCHEmul
{
	/** To create formatter, and implimenets the singleton design pattern. */
	class FormatterBuilder
	{
		public:
		static const std::unique_ptr <FormatterBuilder>& instance (const std::string& ff = "./formats.txt")
							{ return (_instance == nullptr 
								? (_instance = std::unique_ptr <FormatterBuilder> (new FormatterBuilder (ff))) : _instance); }

		~FormatterBuilder ()
							{ _instance = nullptr; }

		// Managing formatters...
		/** To get a formatter. 
			null formatter if the requested one doesn't exist. */
		const Formatter& formatter (const std::string& n) const;
		/** To get the default formatter. */
		const Formatter& defaultFormatter () const
							{ return (_defaultFormatter); }
		void setDefaultFormatter (const Formatter& fmter) const
							{ _defaultFormatter = fmter; }
		void setDefaultFormatter (const std::string& s) const
							{ _defaultFormatter = formatter (s); }

		private:
		// The constructor and equal operators can not be invoked directly...
		FormatterBuilder (const std::string& ff);

		FormatterBuilder (const FormatterBuilder&) = delete;

		FormatterBuilder& operator = (const FormatterBuilder&) = delete;

		protected:
		/** All formatters. */
		Formatters _formatters;
		/** The one that will be choosen by default. 
			It can be either fixed from a const invocation! */
		mutable Formatter _defaultFormatter;

		// Implementation
		/** Singleton design patter. 
			unique_ptr used. */
		static std::unique_ptr <FormatterBuilder> _instance;
	};
}

#endif

// End of the file
/*@}*/#pragma once
