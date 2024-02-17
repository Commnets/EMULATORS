/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: FmterBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/10/2022 \n
 *	Description: A builder to create formatters. \n
 *				 The builder has been designed as a singleton in the framework. \n
 *				 All formatters are kept inside the builder as unique instances of those in the application. \n
 *				 So it is not needed to delete them, as the builder destroys them when finishes.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_FMTERBUILDER__
#define __MCHEMUL_FMTERBUILDER__

#include <CORE/global.hpp>
#include <CORE/Formatter.hpp>

namespace MCHEmul
{
	/** The map of formatters is based on a shared pointers, 
	to simplify the way that the builder manages them. */
	using Formatters = std::map <std::string, std::shared_ptr <Formatter>>;

	/** To create formatters. \n
		Implements the singleton design pattern. */
	class FormatterBuilder
	{
		public:
		static const std::shared_ptr <Formatter> _noFormatter;

		FormatterBuilder () = delete;

		/** The constructor receives and read a set of files where formats are defined. \n
			The formatters are created later, once the element is initialized. */
		FormatterBuilder (const Strings& nF);

		FormatterBuilder (const FormatterBuilder&) = delete;

		FormatterBuilder& operator = (const FormatterBuilder&) = delete;

		virtual ~FormatterBuilder ()
							{ }

		FormatterBuilder (FormatterBuilder&&) = delete;

		FormatterBuilder& operator = (FormatterBuilder&&) = delete;

		/** Create the formatters from the lines read at constructions time.
			If something is wrong during this process, the _error avriable will be different than _NOERROR. */
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
		const std::shared_ptr <Formatter>& formatter (const std::string& n) const;
		std::shared_ptr <Formatter>& formatter (const std::string& n)
							{ return ((std::shared_ptr <Formatter>&) (((const FormatterBuilder*) this) -> formatter (n))); }

		/** To get & set the default formatter. */
		const std::shared_ptr <Formatter>& defaultFormatter () const
							{ return (_defaultFormatter); }
		std::shared_ptr <Formatter>& defaultFormatter ()
							{ return (_defaultFormatter); }
		void setDefaultFormatter (const std::shared_ptr <Formatter>& fmter) const
							{ if (fmter != nullptr) _defaultFormatter = fmter; }

		/** Gets the last error. */
		unsigned int error () const
							{ return (_error); }
		/** To simplify the way the errors are managed. */
		bool operator ! () const
							{ return (_error == _NOERROR); }

		/** To get the unique instance of the formatter builder. 
			If it has never been invoked before, a default instance is created using the parameters received.
			That new instance is also initialized too. */
		static std::shared_ptr <FormatterBuilder> instance (const Strings& nF = { "./defformatters.fmt" });

		protected:
		/** To create the right formatter. \n
			The method receives the name of the file, the name of the formatter under definition 
			and the lines being read now that will form up the formatter. */
		virtual Formatter* createFormatter (const std::string& f, const std::string& nF, const Strings& l) = 0;

		private:
		/** To read the lines from a format file. */
		Strings readLinesFrom (const std::string& nF);

		protected:
		/** The default file of format being used. */
		std::string _defaultFormatFile;
		/** All formatters. */
		std::map <std::string, Formatters> _formatters;
		/** The one that will be choosen by default. 
			It can be either fixed from a const invocation! */
		mutable std::shared_ptr <Formatter> _defaultFormatter;
		mutable unsigned int _error;

		// Implementation
		/** The lines read from the different files. */
		std::map <std::string, Strings> _linesPerFile;
		/** Singleton design patter. */
		static std::shared_ptr <FormatterBuilder> _instance;
	};
}

#endif

// End of the file
/*@}*/
