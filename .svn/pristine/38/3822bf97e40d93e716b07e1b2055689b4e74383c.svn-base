/** \ingroup language */
/*@{*/

/**	
 *	@file	
 *	File: Parser.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: To read a file and convert it into set of data
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_PARSER__
#define __MCHEMUL_PARSER__

#include <vector>
#include <functional>
#include <ostream>

#include <core/CPU.hpp>
#include <language/Instruction.hpp>
#include <core/UBytes.hpp>
#include <core/Address.hpp>

namespace MCHEmul
{
	class Parser
	{
		public:
		/** The possible errors. */
		enum class ParserError
		{
			_NOERROR,
			_CODENOTFOUND,
			_BADNUMBERPARAMETERS,
			_BADTYPEPARAMETERS,
			_BADDEFINEDLINE
		};

		using Lines = std::vector <std::string>;
		using Labels = std::map <std::string, Address>;
		using Macros = std::map <std::string, std::vector <MCHEmul::UByte>>;

		/** An structurer to keep a line of code. */
		struct CodeLine
		{
			public:
			CodeLine ()
				: _address (), _code (), _originalLine ("")
							{ }

			CodeLine (const CodeLine&) = default;

			CodeLine& operator = (CodeLine&) = default;

			friend std::ostream& operator << (std::ostream& o, const CodeLine& cL);

			Address _address;
			std::vector <UByte> _code;
			std::string _originalLine;
		};

		/** The code is made up of many code lines. */
		using Code = std::vector <CodeLine>;

		Parser () = delete;

		Parser (CPU* i)
			: _instructions (i -> instructions ()),
			  _architecture (i -> architecture ()),
			  _parserError (ParserError::_NOERROR), _lastParserLine (0)
							{ }

		Parser (Parser&) = delete;

		Parser& operator = (const Parser&) = delete;

		/** The parser doesn't own the instructions but th CPU. */
		virtual ~Parser ()
							{ }

		Code parse (const std::string& fN) const;
		ParserError parserError () const
							{ return (_parserError); }
		unsigned int lastParserLine () const
							{ return (_lastParserLine); }

		bool operator ! () const
							{ return (_parserError == ParserError::_NOERROR); }

		protected:
		/** To read a file and convert it into Lines to be treaten later. */
		Lines readLines (const std::string& fN) const;
		/** To convert a line into the part of the code. */
		Code parseLine (std::string& l, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;

		// Implementation
		Code parseCommentInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseIncludeInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseMacroInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseLabelInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;
		Code parseOpcodeInst (std::string& cl, Labels& lbs, Macros& mcrs, Address& cA, ParserError& e) const;

		protected:
		Instructions _instructions;
		CPUArchitecture _architecture;

		// Implementation
		mutable ParserError _parserError;
		mutable unsigned int _lastParserLine;
	};
}

#endif
  
// End of the file
/*@}*/