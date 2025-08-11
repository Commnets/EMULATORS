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
#include <SINCLAIR/incs.hpp>
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
		/** This type of information can not be written back to the file. */
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override
							{ return (false); }
	};

	/** In a ZX81/ZX80 the way the tokens are read and 
		the keystrokes associated are managed is totally different. */
	class KeystrokeTypeIO final : public SINCLAIR::KeystrokeTypeIO
	{
		public:
		// Depends on the type...
		KeystrokeTypeIO (ZX81::Type t)
			: SINCLAIR::KeystrokeTypeIO 
				((t == ZX81::Type::_ZX80 ?  _ZX80TOKENS : _ZX81TOKENS),
				 (t == ZX81::Type::_ZX80 ?  _ZX80KEYSTROKES : _ZX81KEYSTROKES)),
			  _type (t)
							{ }

		private:
		/** Neither in ZX81 nor in ZX80 uppercase letters are not allowed. */
		virtual MCHEmul::Strings generateKeystrokeForToken (const std::string& t) const override;
		/** In ZX81 there are tokens like "**" and '""' and in ZX80 "**" is also valid. */
		virtual bool isARealToken (char u, const std::string& l, size_t p) const override;

		private:
		/** The type of simulator. */
		ZX81::Type _type;

		// The tokens and the keystrokes of this computers...
		static const MCHEmul::Strings _ZX81TOKENS;
		static const std::map <std::string, MCHEmul::Strings> _ZX81KEYSTROKES;
		static const MCHEmul::Strings _ZX80TOKENS;
		static const std::map <std::string, MCHEmul::Strings> _ZX80KEYSTROKES;
	};

}

#endif
  
// End of the file
/*@}*/
