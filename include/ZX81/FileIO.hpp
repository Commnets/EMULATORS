/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 22/06/2024 \n
 *	Description:	The specific file readers for ZX81 emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_FILEIO__
#define __ZX81_FILEIO__

#include <CORE/incs.hpp>
#include <ZX81/Type.hpp>

namespace ZX81
{
	/** The structure can be valid for: 
	  * .O and .80 for Z80 and 
	  * .P and .81 and .P81 for ZX81 */
	struct OAndPFileData final : public MCHEmul::FileData
	{
		OAndPFileData ()
			: MCHEmul::FileData (),
			  _name (""),
			  _initialAddress (),
			  _bytes ()
							{ }

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		virtual std::string asString () const override
							{ return (_name + 
								"(Bytes: " + std::to_string (_bytes.size ()) + ")"); }


		std::string _name;
		MCHEmul::Address _initialAddress;
		std::vector <MCHEmul::UByte> _bytes;
	};

	/** To read/write OAndPFileData. */
	class OAndPFileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		OAndPFileTypeIO ()
			: MCHEmul::FileTypeIO ()
									{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <OAndPFileData*> (fD) != nullptr); }
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override;
	};
}

#endif
  
// End of the file
/*@}*/
