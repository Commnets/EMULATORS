/** \ingroup CPU */
/*@{*/

/**
 *	@file
 *	File: Formatter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/10/2022 \n
 *	Description: Defines an element to format fields and values into a string.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_FORMATTER__
#define __MCHEMUL_FORMATTER__

#include <CORE/global.hpp>

namespace MCHEmul
{
	/** The class to format a set of attributes. */
	class Formatter
	{
		public:
		/** To stablish a default formater in the builer. */
		class Set
		{
			public:
			Set ()
				: _format ("")
							{ }

			Set (const std::string& s)
				: _format (s)
							{ }

			Set (const Set&) = delete;

			Set& operator = (const Set&) = delete;

			/** The specisl method to aloww formatter to be in the output string. */
			friend std::ostream& operator << (std::ostream& o, const Set& s);

			private:
			std::string _format;
		};

		static const Formatter _nullFormatter;

		/** 
		  * The constructor receives the name of the formater and the information needed to format. \n
		  * That information should be in the following way:
		  * ...
		  * XXX @FIELD@ YYY Meaning:
		  * XXX = any text. 
		  * @FIELD@ = the name of the field which value has to be included.
		  * YYY more text.
		  * XXX @FIELD_?YYY@ ZZZ Meaning:
		  * @FIELD_?YYY = Describes and array of values FIELD_0, FIELD_1,... (starting from 0) \n
		  * and YYY is what to add after any element, if it is not the last!
		  * If YYY = \n and enter should be added after every element...
		  */
		Formatter (const Strings& l);

		Formatter (const Formatter&) = default;

		Formatter& operator = (const Formatter&) = default;

		virtual ~Formatter ()
							{ }

		/** To format a set of attributes into a string. */
		virtual std::string format (const Attributes& a) const;

		protected:
		struct Piece
		{
			enum class Type { _DATA, _ARRAY, _TEXT };
			
			Piece () = default;
			
			Piece (Type t, const std::string& d, const std::string& pt = "")
				: _type (t), _data (d), _post (pt)
							{ }
			
			Piece (const Piece&) = default;
			
			Piece& operator = (const Piece&) = default;

			Type _type;
			std::string _data;
			std::string _post;
		};

		using Pieces = std::vector <Piece>;

		Pieces _pieces;
	};

	using Formatters = std::map <std::string, Formatter>;
}

#endif

// End of the file
/*@}*/