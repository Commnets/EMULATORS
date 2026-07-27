/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description:	The specific file readers for MSX emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_FILEIO__
#define __MSX_FILEIO__

#include <CORE/incs.hpp>

namespace MSX
{
	/** In the MSX the keystrokes are generated in a special manner. */
	class KeystrokeTypeIO final : public MCHEmul::KeystrokeTypeIO
	{
		public:
		KeystrokeTypeIO (const std::map <char, MCHEmul::Strings>& sK)
			: MCHEmul::KeystrokeTypeIO (),
			  _SPECIALKEYS (sK)
							{ }

		private:
		/** It is like the standard one, but no lowecase letters are allowed.
			So any letter will be managed as an uiper ase letter. */
		virtual MCHEmul::Strings generateKeystrokeForToken (const std::string& t) const override;

		private:
		const std::map <char, MCHEmul::Strings> _SPECIALKEYS;
		// The default ones...
		static const std::map <char, MCHEmul::Strings> _DEFAULTSPECIALKEYS;
	};

	/** The CAS format. \n
		It keeps the byte blocks found between the standard CAS synchronization markers. */
	class CASFileData final : public MCHEmul::FileData
	{
		public:
		enum class BlockType
		{
			_DATA = 0,
			_BINARYHEADER,
			_BASICHEADER,
			_ASCIIHEADER
		};

		struct Block
		{
			Block ()
				: _type (BlockType::_DATA),
				  _fileName (""),
				  _bytes ()
							{ }

			static std::string typeName (BlockType t);
			std::string typeName () const
							{ return (typeName (_type)); }

			BlockType _type;
			std::string _fileName;
			std::vector <MCHEmul::UByte> _bytes;
		};

		using Blocks = std::vector <Block>;

		static const std::vector <MCHEmul::UByte> _SIGNATURE;

		CASFileData ()
			: MCHEmul::FileData (),
			  _blocks ()
							{ }
		CASFileData (const Blocks& b)
			: MCHEmul::FileData (),
			  _blocks (b)
							{ }

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;
		virtual std::string asString () const override;

		Blocks _blocks;
	};

	/** The extension able to read and write CAS file format. */
	class CASFileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		CASFileTypeIO ()
			: MCHEmul::FileTypeIO ()
							{ }
		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;
		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <CASFileData*> (fD) != nullptr); }
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override;

		private:
		bool isCASHeaderAt (const std::vector <MCHEmul::UByte>& dt, size_t p) const;
		CASFileData::BlockType blockTypeFor (const std::vector <MCHEmul::UByte>& dt) const;
		std::string blockTypeName (CASFileData::BlockType t) const
							{ return (CASFileData::Block::typeName (t)); }
		std::string fileNameFor (const std::vector <MCHEmul::UByte>& dt,
			CASFileData::BlockType t) const;
	};
}

#endif
  
// End of the file
/*@}*/
