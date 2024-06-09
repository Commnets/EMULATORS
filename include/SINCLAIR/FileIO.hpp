/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description:	The specific file readers for sinclair emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_FILEIO__
#define __SINCLAIR_FILEIO__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
	/** Format .P and .81. */
	struct Pand81FileData final : public MCHEmul::FileData
	{
		Pand81FileData ()
			: _extension (),
			  _dataBlock ()
							{ }

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		virtual std::string asString () const override
							{ return ("Type:" + _extension + 
								"(Blocksize: " + std::to_string (_dataBlock.size ()) + ")"); }

		std::string _extension;
		MCHEmul::UBytes _dataBlock;
	};

	/** To read/write raw data. \n
		As it is created from the OS. */
	class Pand81FileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		Pand81FileTypeIO ()
			: MCHEmul::FileTypeIO ()
									{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <Pand81FileData*> (fD) != nullptr); }
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override;
	};
}

#endif
  
// End of the file
/*@}*/
