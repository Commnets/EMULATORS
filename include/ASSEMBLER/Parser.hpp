/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: Parser.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/06/2021 \n
 *	Description: To read a file and understanding it.
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_PARSER__
#define __ASSEMBLER_PARSER__

#include <CORE/incs.hpp>
#include <ASSEMBLER/Grammar.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** Keeps the context while parsing. */
		class ParserContext
		{
			public:
			ParserContext () = default; // used by queue...

			ParserContext (Semantic* smt)
				: _semantic (smt),
				  _currentLine (""),
				  _lines (), _actionLines (), 
				  _file (""), _currentLineNumber (0), _templateLinesNumber (0),
				  _linesPointer (), _actionLinesPointer (),
				  _errors (),
				  _filesAlreadyParsed (),
				  _lastStartingPointId (0), _lastLabelId (0), _lastBytesId (0), _lastInstructionId (0)
							{ }

			// The parser context doesn't own anything...

			/** Actualize the global parameters from another. */
			void actualizeGlobalParametersFrom (const ParserContext& pC);

			// The data can be actualized directly...
			// The most important variable = The outcoume of the parsing
			/** Where the parsing results are stored. \n
				This is the real outcome of the process. */ 
			Semantic* _semantic;

			// Variables with a meaning within the file being parsed
			/** The current line breing treated. */
			std::string _currentLine;
			/** The lines to be parserd. \n
				They can grow as parsing progress. */
			Strings _lines;
			/** The action lines associated */
			Strings _actionLines;
			/** Current file being parsed. */
			std::string _file;
			/** Current line being parsed. */
			unsigned int _currentLineNumber;
			/** The number of the template code lines added,
				pending to be processed. */
			unsigned int _templateLinesNumber;
			// Implementation
			/** A pointer to the list of lines and same for action lines. */
			Strings::const_iterator _linesPointer, _actionLinesPointer;

			// Variables with a meaning in the parsing process...
			/** The list of errors, being generated. */
			Errors _errors;
			// Implementation
			/** The list of the files already used. */
			Strings _filesAlreadyParsed;
			/** The last starting point identification number used. */
			unsigned int _lastStartingPointId;
			/** The last label identification number used. */
			unsigned int _lastLabelId;
			/** The last bytes identification number used. */
			unsigned int _lastBytesId;
			/** The last instruction identification number used. */
			unsigned int _lastInstructionId;
		};

		/** The parser reads the file using specific pieces 
			aimed to understand specific parts of the code. \n
			The parser (or the command parsers) works for a specific CPU (and it set of instructions). \n
			The parser is really a builder (design pattern). */
		class Parser;
		class CommandParser
		{
			public:
			CommandParser ()
				: _cpu (nullptr), _parser (nullptr)
							{ }

			CommandParser (const CommandParser&) = delete;

			CommandParser& operator = (const CommandParser&) = delete;

			virtual ~CommandParser ()
							{ }

			CommandParser (CommandParser&&) = delete;

			CommandParser& operator = (CommandParser&&) = delete;

			const CPU* cpu () const
							{ return (_cpu); }
			const Parser* parser () const
							{ return (_parser); }

			/** Maybe one of them could need it. */
			virtual void initialize ()
							{ }

			/** Determine whether the current context situation can or not be parsed by this Command Parser. */
			virtual bool canParse (ParserContext* pC) const = 0;
			/** Parse the current context, obviously when it is able to do so. \n
				When parsing the context (more like Semantic) are modified. */
			virtual void parse (ParserContext* pC) const = 0;

			/** Invoked from the Parser's constructor. */
			void setCPU (const CPU* c)
							{ assert (c != nullptr); _cpu = c; }
			void setParser (const Parser* p)
							{ assert (p!= nullptr); _parser = p; }

			protected:
			const CPU* _cpu;
			const Parser* _parser;
		};

		/** To facilitate the use of a list of commands. */
		using CommandParsers = std::vector <CommandParser*>;

		/** To parser a comment.
			Th symbol defining the beginning of a comment can be redefined. */
		class CommentCommandParser final : public CommandParser
		{
			public:
			CommentCommandParser (unsigned char s = ';')
				: CommandParser (),
				  _symbol (s)
							{ }

			unsigned char symbol () const
							{ return (_symbol); }

			virtual bool canParse (ParserContext* pC) const override
							{ return (pC -> _currentLine [0] == _symbol); }
			virtual void parse (ParserContext* pC) const override
							{ pC -> _currentLine = ""; /** Nothing after the comment is important. */}

			private:
			const unsigned char _symbol = ';'; // Adjusted at construction level
		};

		/** To parser a file inclusion.
			The symbol defining that can be changed. */
		class IncludeCommandParser final : public CommandParser
		{
			public:
			IncludeCommandParser (unsigned char s = '#')
				: CommandParser (),
				  _symbol (s)
							{ }

			virtual bool canParse (ParserContext* pC) const override
							{ return (pC -> _currentLine [0] == _symbol); }
			virtual void parse (ParserContext* pC) const override;

			private:
			const unsigned char _symbol = '#'; // Adjusted at construction level
		};

		/** To parser a macro. 
			The symbol defining the separation between definition and value can be redefined. */
		class MacroCommandParser : public CommandParser
		{
			public:
			MacroCommandParser (unsigned char s = '=')
				: CommandParser (),
				  _symbol (s)
							{ }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t eP = pC -> _currentLine.find (_symbol);
							  return (eP != std::string::npos && validLabel (trim (pC -> _currentLine.substr (0, eP)))); }
			virtual void parse (ParserContext* pC) const override;

			protected:
			const unsigned char _symbol = '=';
		};

		/** To parser a template of code. \n 
			The template of code starts always with a symbold and finish wih other. \n
			Both can be redefined. */
		class CodeTemplateDefinitionCommandParser final : public CommandParser
		{
			public:
			CodeTemplateDefinitionCommandParser (unsigned char iS = '{', unsigned char fS = '}')
			: CommandParser (),
				_initialSymbol (iS), _finalSymbol (fS)
						{ }

			virtual bool canParse (ParserContext* pC) const override;
			virtual void parse (ParserContext* pC) const override;

			protected:
			const unsigned char _initialSymbol = '{';
			const unsigned char _finalSymbol = '}';
		};

		/** When that template is about to be used. */
		class CodeTemplateUseCommandParser final : public CommandParser
		{
			public:
			CodeTemplateUseCommandParser (unsigned char s = '.')
				: CommandParser (),
				  _symbol (s)
						{ }

			virtual bool canParse (ParserContext* pC) const override
						{ return (pC -> _currentLine [0] == _symbol && pC -> _currentLine.length () > 1); }
			virtual void parse (ParserContext* pC) const override;

			protected:
			const unsigned char _symbol = '.';
		};

		/** To parser an address macro.
			The symbol defining the separation between definition and value can be redefined. 
			Qhen parser the address assigned is incremented. */
		class StartingPointCommandParser final : public CommandParser
		{
			public:
			StartingPointCommandParser (unsigned char s = '=')
				: CommandParser (),
				  _symbol (s)
							{ }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t eP = pC -> _currentLine.find (_symbol);
							  return (eP != std::string::npos && trim (pC -> _currentLine.substr (0, eP)) == "*"); }
			virtual void parse (ParserContext* pC) const override;

			private:
			const unsigned char _symbol = '=';
		};

		/** To parser a label. */
		class LabelCommandParser final : public CommandParser
		{
			public:
			LabelCommandParser (unsigned char s = ':')
				: CommandParser (),
				  _symbol (s)
							{ }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t eP = pC -> _currentLine.find (_symbol);
							  return (eP != std::string::npos && validLabel (trim (pC -> _currentLine.substr (0, eP)))); }
			virtual void parse (ParserContext* pC) const override;

			private:
			const unsigned char _symbol = ':';
		};

		/** To parser a set of bytes. */
		class BytesCommandParser final : public CommandParser
		{
			public:
			BytesCommandParser ()
				: CommandParser ()
							{ }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t p = firstSpaceIn (pC -> _currentLine);
							  return ((p == std::string::npos) 
								  ? false : upper (trim (pC -> _currentLine.substr (0, p))) == "BYTES"); }
			virtual void parse (ParserContext* pC) const override;
		};

		/** To parser a binary definition. \n
			In a binary data chars to group set of bits can be used. \n 
			The binary definition defines how to interprete those chars in terms of bits. \n
			So a binary parser should need before a binary definition parser. \n
			If not no chars can matched the binary value is got. \n
			The structure of the info is like:
			BINARYDEF A=01 B=10 ...*/
		class BinaryDefinitionParser final : public CommandParser
		{
			public:
			BinaryDefinitionParser ()
				: CommandParser ()
							{ }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t p = firstSpaceIn (pC -> _currentLine);
							  return ((p == std::string::npos) 
								  ? false : upper (trim (pC -> _currentLine.substr (0, p))) == "BINARYDEF"); }
			virtual void parse (ParserContext* pC) const override;

			const std::string& definitionFor (char c) const
							{ DefMap::const_iterator i = _definitionMap.find (c);
							  return ((i == _definitionMap.end ()) ? _DEFVALUE : (*i).second); }

			private:
			static const std::string _DEFVALUE;

			using DefMap = std::map <char, std::string>;
			mutable DefMap _definitionMap;
		};

		/** To parser a binary declaration. \n
			How the binary declaration works is affected by the binary definitio0n instructions. \n */
		class BinaryCommandParser final : public CommandParser
		{
			public:
			BinaryCommandParser (BinaryDefinitionParser* dP = nullptr)
				: CommandParser (),
				  _definitionParser (dP)
							{ }

			void setDefinitionParser (BinaryDefinitionParser* bD)
							{ _definitionParser = bD; }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t p = firstSpaceIn (pC -> _currentLine);
							  return ((p == std::string::npos) 
								  ? false : upper (trim (pC -> _currentLine.substr (0, p))) == "BINARY"); }
			virtual void parse (ParserContext* pC) const override;

			private:
			BinaryDefinitionParser* _definitionParser;
		};

		/** To parser a text declaration. \n
			How the letters of the text are "managed "translated" into bytes 
			will depend on the computer at compilation time. \n */
		class TextCommandParser final : public CommandParser
		{
			public:
			TextCommandParser (const ASCIIConverter* tC)
				: CommandParser (),
				  _ASCIIConverter (tC) // It could be null at construction time...
							{ }

			/** The class is not the owner of the converter as it might be used by other classes. */
			void setASCIIConverter (const ASCIIConverter* tC)
							{ _ASCIIConverter = tC; }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t p = firstSpaceIn (pC -> _currentLine);
							  return ((p == std::string::npos) 
								  ? false : upper (trim (pC -> _currentLine.substr (0, p))) == "TEXT"); }
			virtual void parse (ParserContext* pC) const override;

			private:
			const ASCIIConverter* _ASCIIConverter;
		};

		/** To include a binary set of data comming from a file. */
		class LoadBytesFileCommandParser final : public CommandParser
		{
			public:
			LoadBytesFileCommandParser ()
				: CommandParser ()
							{ }

			virtual bool canParse (ParserContext* pC) const override
							{ size_t p = firstSpaceIn (pC -> _currentLine);
							  return ((p == std::string::npos) 
								  ? false : upper (trim (pC -> _currentLine.substr (0, p))) == "BYTESFILE"); }
			virtual void parse (ParserContext* pC) const override;
		};

		/** To parser an instruction. */
		class InstructionCommandParser final : public CommandParser
		{
			public:
			InstructionCommandParser ()
				: CommandParser ()
							{ }

			virtual bool canParse (ParserContext* pC) const override;
			virtual void parse (ParserContext* pC) const override;
		};

		/** Now it is time to define the parser itself. \n
			To convert a file text (or text) into an internal structure. \n
			The parser is a line parser. That is, it is only able to parser a line. \n
			There can be added specific instruction parsers but always a comment command parser must exist. \n
			It is used to determine whether a line finishes or not. \n
			The parser can be extended later to include / change specific command parsers */
		class Parser
		{
			public:
			Parser (const CPU* c, const ASCIIConverter* aC = new ASCIIConverter /** The deafult one does nothing. */,
					const CommandParsers& lP = // With the standard line parsers...
						{ new CommentCommandParser, // Comments
						  new IncludeCommandParser, // Include other assembler files
						  new MacroCommandParser, // Macro definitions
						  new CodeTemplateDefinitionCommandParser, new CodeTemplateUseCommandParser, // Templates
						  new StartingPointCommandParser, // Starting point for data and code
						  new LabelCommandParser, // Labels for jumps
						  new BytesCommandParser, // Set of bytes
						  new TextCommandParser (nullptr), // A text can be converted into bytes in memory...
						  new LoadBytesFileCommandParser, // Loading a file of bytes
						  new BinaryDefinitionParser, new BinaryCommandParser, // Binary definition
						  new InstructionCommandParser // To understand instructions
						});

			Parser (Parser&) = delete;

			Parser& operator = (const Parser&) = delete;

			/** The parser doesn't own the instructions but the CPU, but the line parsers. */
			~Parser ()
							{ for (auto i : _commandParsers) delete (i); delete (_ASCIIConverter); }

			/** To activate or desactivate to print out the parsing process. 
				using the output channel received as parameter in th eparsing methods. */
			void setPrintOutProcess (bool a)
							{ _printOutProcess = a; }

			const CPU* cpu () const
							{ return (_cpu); }
			const CommandParsers& commandParsers () const
							{ return (_commandParsers); }
			unsigned char commentSymbol () const;

			void initialize ()
							{ for (const auto& i : _commandParsers) i -> initialize (); }

			/**
			  * Type of code that is is accepted:
			  * The example has been written using the machine languaje of the Commodore 64
			  *	; Simple test that everything works \n
			  *	; By Ignacio Cea \n
			  * \n
			  *	; MACROS \n
			  *	FOREGROUND = $D020 \n
			  *	BACKGROUND = $D021 \n
			  * ... \n
			  * ; TEMPLATES \n
			  * { TEMPLATENAME #1,#2,#3...  \n
			  * ...
			  * }
			  * \n
			  *	* = $C000 \n
			  * \n
			  *	; Now the code \n
			  * \n
			  *	START:			LDA #$00 ;Load accumulator \n
			  *					STA BACKGROUND \n
			  *					STA FOREGROUND \n
			  *					BNE START \n
			  * ...
			  *					.TEMPLATENAME VAR1,VAR2,VAR3,...
			  * \n
			  *	; Very simple
			  * Rules:
			  *	; Means comment. After that, nothing will be taken into account. \n
			  * MACROS & LABELS are represented using characteres (upper and lower case) and numbers, but never starting with number. \n
			  * NUMBERS AND DIRECTIONS can be represented using decimal, octal (starting with 0) and hexadecimal (with $) numbers. \n
			  * * = xxxx will identify the address from which the code after will be inserted! \n
			  * Templates can be used. A template is a piece of "repitable" code. \n
			  * When the instruction "." is found then, the piece of code which name is just behind 
			  * is "virtualy" inserting just behind replacing the #1,#2... references for the values VAR1,VAR2,... \n
			  * If an error parsing those new line were found the line pointed would be the one of the template definition. \n
			  * \n
			  * The method can receive also another file with "actions" to execute over any line. \n
			  * This action must be simply number. How to interpret them should be determine later. \n
			  * In the default implementation the number 0 will mean nothing, and the number 1 to stop. \n
			  * The method can also recive as parameter an external ParameterContext reference. \n
			  * It is possible to print out what the parser is doing. @see method printOutProcess.
			  */
			Semantic* parse (const std::string& fN, const std::string& fA = "",
				std::ostream& oC = std::cout) const; // A context is created...
			Semantic* parse (const std::string& fN, const std::string& fA, 
				ParserContext* pC /** When the context is outside. */, std::ostream& oC = std::cout) const;

			Errors errors () const
							{ return (_errors); }
			/** To simplify checking whether there was or not an error parsing. */
			bool operator ! () const
							{ return (!_errors.empty ()); }

			protected:
			/** To create the right semantic for this parser. */
			virtual Semantic* createSemantic () const
							{ return (new Semantic); }
			/** To create the right context to manage suring the parsing. */
			virtual ParserContext* createParserContext () const
							{ return (new ParserContext (createSemantic ())); }

			/** To read a file and convert it into Lines to be treaten later. */
			Strings readLines (const std::string& fN) const;
			/** To parse in detail. This part of the code can be invoked recursevly. */
			void parseLines (ParserContext* pC, std::ostream& oC) const;

			private:
			const CPU* _cpu;
			const ASCIIConverter* _ASCIIConverter;
			CommandParsers _commandParsers;
			bool _printOutProcess;

			// Implementation
			mutable Errors _errors;
			mutable unsigned char _commentSymbol;
			mutable std::string _lastLinePrintedOut;
		};
	}
}

#endif
  
// End of the file
/*@}*/