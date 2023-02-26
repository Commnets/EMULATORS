/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: StdFormatter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 18/10/2022 \n
 *	Description: Defines a formatter based of file definitions.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STDFORMATTER__
#define __MCHEMUL_STDFORMATTER__

#include <CORE/Formatter.hpp>

namespace MCHEmul
{
	/** The Standard Formatter is based on pieces. */
	class StdFormatter final : public Formatter
	{
		public:
		/** 
		  * The constructor receives the "pure" lines that will make up the formatter. \n
		  * Thise line will follow the next pattern: \n
		  * XXX @FIELD{prm1,prm2,...}?|>NNN@ YYY Meaning: \n
		  * XXX = any text. \n
		  * @FIELD@ = the name of the field which value has to be included. \n
		  * {prm1=value1,prm2=value2,...} = a set of attributes than can affect the formatting if the field. \n
		  * If defining a value a "," or a " " is needed uses: "\," or "\ ".
		  * ?: The field has to refer to a InfoStructure. The info of every element in that InfoStructure 
		  *	   will be including using a standard "empty" formatter (@see StdFormatter). \n
		  * >: The field has to refer to a InfoStructure. The info of that InfoStructure 
		  *	   will be formatting using a Formatter with the same name than the InfoStructure. \n
		  * NNN: Text to be added after any element of the Array (?) formatted. \n
		  * YYY more text after the field. \n
		  */
		StdFormatter (const Strings& l)
			: Formatter (l),
			  _pieces (),
			  _defSeparator ("\n"), _defEqual ("="), _printFirst (true), _whenEmpty ("none")
							{ }

		StdFormatter (Strings&& l)
			: Formatter (std::move (l)),
			  _pieces (),
			  _defSeparator ("\n"), _defEqual ("="), _printFirst (true), _whenEmpty ("none")
							{ }

		virtual ~StdFormatter ()
							{ for (auto& i : _pieces) delete (i); }

		/** To change the elements used to format things when a no piece is defined for that. */
		void setDefFormatElements (const std::string& s, const std::string& e, bool pF, const std::string& wE)
							{ _defSeparator = s; _defEqual = e; _printFirst = pF; _whenEmpty = wE; }

		/** To initialize the formatter. 
			This method create all pieces and invokes the factiry method: createPiece. */
		virtual void initialize () override;

		/** To format a set of attributes into a string. 
			It can be redefined to created specific formatters .eg. not based on pieces. */
		virtual std::string format (const InfoStructure& a) const override;
		/** This function is not possible in this type of Formatter. */
		virtual InfoStructure unFormat (const std::string& str) const override
							{ return (InfoStructure ()); }

		protected:
		/** Any formatter is made up of different pieces. \n
			Many different types of pieces can be added later, but a couple of them are provided by default. */
		class Piece
		{
			public:
			/** The pieces cab belong to 4 different types : \n
				_TEXT	The name of the piece represents what to get. \n
				_DATA	The name of the piece represents the name of the attribute to get. \n
				_ARRAY	The name of the piece represents the name of the InfoStructure which attributes get. \n
				_INVOKE	The name of the piece represents the name of the InfoStructure which attributes format using
						another formatter with the same n ame of that InfoStructure. */
			enum class Type { _TEXT, _DATA, _ARRAY, _INVOKE };

			/** The constructor reveives: \n
			  *	@param tp		The type of formatter requiered. \n
			  *	@param n		The name of the formatter to create. \n
			  *	@param prms		The list of parameters for the formatter \n
			  *	@param pt		Something to write after the formatter works.
			  */
			Piece (Type t, const std::string& n, const Strings& prms, const std::string& pt)
				: _type (t),
				  _name (n), _attributes (), _post (pt),
				  _parameters (prms)
							{ _attributes = attrsFromStrings (_parameters); }

			Piece (const Piece&) = default;

			Piece& operator = (const Piece&) = default;

			virtual ~Piece ()
							{ }

			Type type () const
							{ return (_type); }
			const std::string name () const
							{ return (_name); }
			const Attributes& attributes () const
							{ return (_attributes); }
			std::string attribute (const std::string& aN) const
							{ MCHEmul::Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? "" : (*i).second); }
			const std::string& post () const
							{ return (_post); }
			const Strings& parameters () const
							{ return (_parameters); }

			/** To format the set of attributes.
				It can be overloaded. */
			virtual std::string format (const InfoStructure&) const = 0;

			protected:
			Attributes attrsFromStrings (const Strings& strs);

			protected:
			Type _type;
			std::string _name;
			std::string _post;
			Attributes _attributes;

			// Implementation
			Strings _parameters;
		};

		/** Just a fix text. */
		class FixTextPiece final : public Piece
		{
			public:
			/** The constructor receives just the text to print. No formatted is added. */
			FixTextPiece (const std::string& n)
				: Piece (Type::_TEXT, n, { }, "")
							{ }

			/** The infoStructure received is not taken into account at all. */
			virtual std::string format (const InfoStructure&) const override
							{ return (_name); }
		};

		/** Get just a data from the list of attributes. */
		class AttributePiece final : public Piece
		{
			public:
			AttributePiece (const std::string& n, const Strings& prms, const std::string& pt)
				: Piece (Type::_DATA, n, prms, pt)
							{ }

			virtual std::string format (const InfoStructure& iS) const override
							{ return (iS.attribute (_name)); }
		};

		/** To format the a list of elements like a table. \n
			The parameters are: \n
			blocksize	The size of every block of data. \n
			listsep		The character separating every original element in the list. \n 
			tablesep	The element to separate every element in the table. \n
			minsize		The minimun size that every element in the table has to have. */
		class TablePiece final : public Piece
		{
			public:
			TablePiece (const std::string& n, const Strings& prms, const std::string& pt)
				: Piece (Type::_DATA, n, prms, pt)
							{ }

			virtual std::string format (const InfoStructure& iS) const override;
		};

		/** To format the elements of a structure. 
			The attributes can be:
			fmter			To indicate whether if there is any specific formatter to use for every element in the list. \n
							It has to be used when there is only elements of this type in the infostructure. \n
							If the fmter is = ".CLASS" the formatter with the name od the class is taken. \n
							That className should be found witin the attribute ".CLASSNAME" of the InfoStructure.
			or:
			key = YES|NO	To indicate whether the key element of every element in the array is or not printed out. 
			equal			In the case the previous element is YES, then a separator has to be indicated. */
		class ArrayPiece : public Piece
		{
			public:
			ArrayPiece (const std::string& n, const Strings& prms, const std::string& pt)
				: Piece (Type::_ARRAY, n , prms, pt)
							{ }

			virtual std::string format (const InfoStructure& iS) const override;
		};

		/** Invoke another formatter. */
		class InvokePiece : public Piece
		{
			public:
			InvokePiece (const std::string& n, const Strings& prms, const std::string& pt)
				: Piece (Type::_INVOKE, n , prms, pt)
							{ }

			virtual std::string format (const InfoStructure& iS) const override;
		};

		using Pieces = std::vector <Piece*>;

		/** The method receives:
		  *	@param tp		The type of formatter requiered. \n
		  *	@param n		The name of the formatter to create. \n
		  *	@param prms		The list of parameters for the formatter \n
		  *	@param pt		Something to write after the formatter works. \n
		  * When tp == _DATA && n == BYTES a TablePiece is created instead an AttributePiece.
		  */
		virtual Piece* createPiece (Piece::Type tp, const std::string& n, const Strings& prms, const std::string& pt) const;

		protected:
		Pieces _pieces;
		std::string _defSeparator, _defEqual;
		bool _printFirst;
		std::string _whenEmpty;
	};
}

#endif

// End of the file
/*@}*/