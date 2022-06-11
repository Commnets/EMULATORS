/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: Error.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/06/2021 \n
 *	Description: The type of error can be generated parsing a instructions file.
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_ERROR__
#define __ASSEMBLER_ERROR__

#include <CPU/incs.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** The type of errors admitted by the system. */
		enum class ErrorType
		{ 
			_NOERROR = 0, 
			// Related with Macros
			_MACROBADDEFINED, 
			_MACRONOTDEFINED, 
			// Related with Semantic Elements
			_LABELNOTVALID,
			_LABELNOTDEFINED,
			_BYTESNOTVALID,
			_INSTRUCTIONNOTVALID,
			_INSTRUCTIONNOTDEFINED,
			_STARTINGPOINTNOTVALID,
			// Related with Semantic structure
			_GRAMARELEMENTNOTVALID, 
			_STARTINGPOINTNOTDEFINED,
			_DUPLICATEMACRO,
			_SEMANTICERROR,
			// Related with Parser errors
			_FILEEMPTY, 
			_PARSERNOTFOUND, 
		};

		/** The error class. */
		struct Error
		{
			Error ()
				: _type (ErrorType::_NOERROR), 
				  _file (""),
				  _line (0), _column (0)
							{ }

			Error (ErrorType eT, const std::string& f, unsigned int l, unsigned int c)
				: _type (eT), _file (f), _line (l), _column (c)
							{ }

			Error (const Error&) = default;

			Error& operator = (const Error&) = default;

			friend std::ostream& operator << (std::ostream& o, const Error& e);

			ErrorType _type;
			std::string _file;
			unsigned int _line;
			unsigned int _column;
		};

		using Errors = std::vector <Error>;
	}
}

#endif
  
// End of the file
/*@}*/