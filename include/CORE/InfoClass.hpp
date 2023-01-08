/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: InfoClass.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/11/2022 \n
 *	Description: Class to manage allow other to generate a InfoStructure element.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_INFOCLASS__
#define __MCHEMUL_INFOCLASS__

#include <CORE/global.hpp>
#include <CORE/InfoStructure.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

namespace MCHEmul
{
	class InfoClass
	{
		public:
		/** A name must be provided always. */
		InfoClass () = delete;
		
		InfoClass (const std::string& cN)
			: _className (cN)
							{ }

		const std::string& className () const
							{ return (_className); }
		void setClassName (const std::string& cN)
							{ _className = cN; }

		virtual InfoStructure getInfoStructure () const = 0;

		/** Generic print out method. */
		friend std::ostream& operator << (std::ostream& o, const InfoClass& iC)
							{ return (o << FormatterBuilder::instance () -> 
								formatter (iC.className ()) ->  format (iC.getInfoStructure ())); }

		private:
		std::string _className;
	};
}

#endif

// End of the file
/*@}*/