/** \ingroup CORE */
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
	/** To create formatters. \
		Implimenets the singleton design pattern. */
	class FormatterBuilder
	{
		public:
		static const std::shared_ptr <Formatter> _noFormatter;

		/** The constructor receives and read a set of files where formats are defined. \n
			The formatters are created later, once the element is initialized. */
		FormatterBuilder (const Strings& nF);

		~FormatterBuilder ();

		/** Create the formatters from the lines read at constructions time. */
		bool initialize ();

		// Managing formatters...
		/** To change the file of formaters taken into account. */
		const std::string& defaultFormatFile () const
							{ return (_defaultFormatFile); }
		void setDefaultFormatFile (const std::string& ff)
							{ _defaultFormatFile = ff; }
		
		/** To get a formatter. \n
			defaultFormatter if the requested one doesn't exist. */
		bool existsFormatter (const std::string& n) const;
		const Formatter* formatter (const std::string& n) const;
		Formatter* formatter (const std::string& n)
							{ return ((Formatter*) (((const FormatterBuilder*) this) -> formatter (n))); }

		/** To get & set the default formatter. */
		const Formatter* defaultFormatter () const
							{ return (_defaultFormatter); }
		Formatter* defaultFormatter ()
							{ return (_defaultFormatter); }
		void setDefaultFormatter (Formatter* fmter) const
							{ if (fmter != nullptr) _defaultFormatter = fmter; }

		/** Gets the last error. */
		unsigned int lastEror () const
							{ return (_lastError); }

		bool operator ! () const
							{ return (_lastError == _NOERROR); }

		/** To get the unique instance of the formatter builder. 
			If ih has been everinvoked before, a default instance is created using the parameters received and initialized too. */
		static std::shared_ptr <FormatterBuilder> instance (const Strings& nF = { "./defformatters.fmt" });

		protected:
		/** To create the right formatter. \n
			The method receives the name of the file, the name of the formatter under definition 
			and the lines being read now that will form up the formatter. \n
			By default the basic formatter is created. */
		virtual Formatter* createFormatter (const std::string& f, const std::string& nF, const Strings& l)
							{ return (new StdFormatter (l)); }

		private:
		/** The copy constructor is not allowed. */
		FormatterBuilder (const FormatterBuilder&) = delete;
		/** And the same with the operator = */
		FormatterBuilder& operator = (const FormatterBuilder&) = delete;

		/** To read the lines from a format file. */
		Strings readLinesFrom (const std::string& nF);

		protected:
		/** The default file of format being used. */
		std::string _defaultFormatFile;
		/** All formatters. */
		std::map <std::string, Formatters> _formatters;
		/** The one that will be choosen by default. 
			It can be either fixed from a const invocation! */
		mutable Formatter* _defaultFormatter;
		mutable unsigned int _lastError;

		// Implementation
		/** The lines read from the different files. */
		std::map <std::string, Strings> _linesPerFile;
		/** Singleton design patter. */
		static std::shared_ptr <FormatterBuilder> _instance;
	};
}

#endif

// End of the file
/*@}*/#pragma once
