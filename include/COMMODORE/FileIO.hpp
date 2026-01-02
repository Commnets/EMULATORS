/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 31/01/2023 \n
 *	Description:	The specific file readers for commodore emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_FILEIO__
#define __COMMODORE_FILEIO__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** Struct to conect tap info.
	  *	It has been extracted from:
	  * https://vice-emu.sourceforge.io/vice_17.html
	  * The information is a bit different depending on whether the 
	  * CompuerVersion is _C64, _VIC20, _C16, _PET, _C5x0 or _C6x0, althought it is basically the same.
	  * The file has a header:
	  * A signature of 12 bytes: "C64-TAPE-RAW" for C64 computers or "C16-TAPE-RAW" for C16 computers, etc.
	  * 1 byte with the TAP version: 0,1, or 2. In C64 and VIC20 it is typically 0, and in C16 it is typically 2.
	  * 1 byte with the computer version: 0 = C64, 1 = VIC20, 2 = C16,...
	  * 1 byte with the video version: 0 = PAL, 1 = NTSC
	  * 1 byte more not used (set to 0).
	  * 4 bytes with the data size (little endian).
	  */
	struct TAPFileData final : public MCHEmul::FileData
	{
		enum ComputerVersion
		{
			_C64 = 0,
			_VIC20 = 1,
			_C16 = 2,
			_PET = 3,
			_C5x0 = 4,
			_C6x0 = 5 /** Valid also for _C7x0 */
		};

		enum VideoVersion
		{
			_PAL = 0,
			_NTSC = 1,
			_OLDNTSC = 2
		};

		/** Default constructor. 
			The default values are for a Commodore 64 commputer in a PAL video system. */
		TAPFileData (
				const std::string& sgn = "C64-TAPE-RAW", 
				unsigned char v = 0, ComputerVersion cV = _C64, VideoVersion vV = _PAL)
			: _signature (sgn), // The default one...
			  _version (v), // The default one...
			  _computerVersion (cV),
			  _videoVersion (vV),
			  _dataSize (0),
			  _bytes (),
			  _attributes { }
							{ }

		/** Constructor from a memory data block...
			With the possibility to change some of the parameters!
			The default values are for a C64 computer in a PAL video system. */
		TAPFileData (
				const MCHEmul::ExtendedDataMemoryBlocks& dMB, // This is mandatory...
				const std::string& sgn = "C64-TAPE-RAW",
				unsigned char v = 0, ComputerVersion cV = _C64, VideoVersion vV = _PAL)
			: _signature (sgn),
			  _version (v), // The default one...
			  _computerVersion (cV),
			  _videoVersion (vV),
			  _dataSize (0),
			  _bytes (),
			  _attributes { }
							{ addFromMemoryBlock (dMB); }

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		/** Adds all info possible from a ExtendedDataMemoryBlock. */
		void addFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
							{ addHeaderFromMemoryBlock (dMB); addDataBlockFromMemoryBlock (dMB); }
		/** Adds the header from a extended data block. */
		void addHeaderFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB);
		/** Creates and adds (if possible) a Data Block from a DataMemoryBlock info received as parameter. \n
			If not possible, a nullptr is returned. \n
			The method behaves as a factory method. */
		void addDataBlockFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB);
		/** Create a new object from a ExtendedDataMemoryBlock...if possible. 
			and with some default values, that can be changed to support other computers and versions. */
		static TAPFileData* createFromMemoryBlock 
			(const MCHEmul::ExtendedDataMemoryBlocks& dMB,
			 const std::string& sgn = "C64-TAPE-RAW",
			 unsigned char v = 0, ComputerVersion cV = _C64, VideoVersion vV = _PAL)
							{ return (new TAPFileData (dMB, sgn, v, cV, vV)); }

		virtual std::string asString () const override
							{ return (_signature + 
								"(Version: " + std::to_string (_version) + "," +
								"ComputerVersion: " + std::to_string (_computerVersion) + "," +
								"VideoVersion: " + std::to_string (_videoVersion) + "," +
								"Size: " + std::to_string (_dataSize) + ")"); }

		std::string _signature;
		unsigned char _version;
		ComputerVersion _computerVersion;
		VideoVersion _videoVersion;
		unsigned int _dataSize;
		MCHEmul::UBytes _bytes;
		MCHEmul::Attributes _attributes; // Attributes of the file, like the name (FNAME), etc.
	};

	/** To read/wrote TAP files. */
	class TAPFileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		TAPFileTypeIO ()
			: MCHEmul::FileTypeIO ()
							{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <TAPFileData*> (fD) != nullptr); }
		/** This format for COMMODORE than can be written back,
			because it is the one managed by the standard datasette. */
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override;
	};

	/** The most simple type of info for COMMODORE computers,
		That is the PRG format. */
	struct PRGFileData final : public MCHEmul::FileData
	{
		MCHEmul::Address _address;
		MCHEmul::UBytes _bytes;

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		virtual std::string asString () const override
							{ return (_address.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0') +
								" (Size: " + std::to_string (_bytes.size ()) + ")"); }
	};

	/** The loader for PRG. */
	class PRGFileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		PRGFileTypeIO ()
			: MCHEmul::FileTypeIO ()
							{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <PRGFileData*> (fD) != nullptr); }
		/** This type of format can not be written back to any file. */
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override
							{ return (false); }
	};

	/**	Struct to content the cartridge info...
	  *	It has been extracted from:
	  * https://vice-emu.sourceforge.io/vice_17.html
	  */
	struct T64FileData final : public MCHEmul::FileData
	{
		/** The header of the file.
			Of if this elements per file. */
		struct TapeRecord
		{
			TapeRecord ()
				: _descriptor (""),
				  _version (0),
				  _entries (),
				  _usedEntries (),
				  _userDescriptor ("")
							{ }

			std::string _descriptor;				// $00 - $20 : 32 bytes. DOS tape descriptor. Internal.
			unsigned short _version;				// $20 - $21 : 2 bytes. Tape version ($0200 e.g.).
			unsigned short _entries;				// $22 - $23 : 2 bytes. Number of directory entries (potentially).
			unsigned short _usedEntries;			// $24 - $25 : 2 bytes. Number of used entries (the ones really used).
			// $26 - $27 : 2 bytes. Free for future uses.
			std::string _userDescriptor;			// $28 - $39 : 40 bytes. User descriptor displayed in the tape menu.
		};

		/** Per each different entry there will be one of these elements. */
		struct FileRecord
		{
			enum EntryType
			{
				_FREE = 0,
				_NORMAL = 1,
				_WITHHEADER = 2,
				_SNAPSHOT = 3,
				_TAPEBLOCK = 4,
				_DIGITALIZED = 5
			};

			FileRecord ()
				: _entryType (),
				  _fileType ('\0'),
				  _startLoadAddress (),
				  _endLoadAddress (),
				  _offset (0),
				  _fileName ("")
							{ }

			EntryType _entryType;					// $00 - $00 : 1 bytes. Entry type (@see above).
			char _fileType;							// $01 - $01 : 1 byte. File type.
			MCHEmul::Address _startLoadAddress;		// $02 - $03 : 2 bytes. Where to start to load the data in memory.
			MCHEmul::Address _endLoadAddress;		// $04 - $05 : 2 bytes. Where to finish that loading.
			// $06 - $07: 2 bytes. Free for future uses...
			unsigned int _offset;					// $08 - $0c : offset. Where the content tp load really starts in this data file
													// It has to be counted from the beginning of the file, inclusing the header.
			// $0c - $0f 2 bytes. Free for future uses...
			std::string _fileName;					// $10 - $1f : Name of the file.
		};

		// In the tape format there might be more than a file record...
		using FileRecords = std::vector <FileRecord>;

		T64FileData ()
			: _tapeRecord (),
			  _fileRecords (),
			  _content ()
							{ }

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		virtual std::string asString () const override;

		TapeRecord _tapeRecord;
		FileRecords _fileRecords;
		MCHEmul::UBytes _content;
	};

	/** To read/write the T64 type files. */
	class T64FileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		T64FileTypeIO ()
			: MCHEmul::FileTypeIO ()
									{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <T64FileData*> (fD) != nullptr); }
		/** This type of file can not be written back. */
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override
							{ return (false); }
	};

	/**	Struct to content the cartrige info...
	  *	It has been extracted from:
	  * https://vice-emu.sourceforge.io/vice_17.html
	  */
	struct CRTFileData final : public MCHEmul::FileData
	{
		struct ChipData
		{
			ChipData ()
				: _signature (""),
				  _packageSize (0),
				  _type (0),
				  _bankNumber (0),
				  _startingLoadAddress (),
				  _romSize (0),
				  _content ()
							{ }

			std::string _signature;					// $00 - $03 :	Always 4 chars, but they have to content "CHIP" to be valid...
			unsigned int _packageSize;				// $04 - $07 :	Size including the header.
			unsigned short _type;					// $08 - $09 :	0 = ROM, 1 = RAM, no ROM data, 2 = Flash ROM.
			unsigned short _bankNumber;				// $0a - $0b :	0 = Normal cartridge.
			MCHEmul::Address _startingLoadAddress;	// $0c - $0d
			unsigned short _romSize;				// $0e - $0f :	Typically $2000 = 8k or $4000 = 16k
			MCHEmul::UBytes _content;				// $10 - ... :	The real content finally...
		};

		// In a cartridge there might be more than 1 ChipData...
		using ChipsData = std::vector <ChipData>; 

		CRTFileData ()
			: _signature (""),
			  _headerSize (0),
			  _cartridgeVersion (0),
			  _cartridgeType (0),
			  _EXROMActive (false),
			  _GAMEActive (false),
			  _name (""),
			  _chipsData ()
							{ }

		/** 
			The attributes kept are: \n
			TYPE:		The type of card. \n
			VERSION:	The version of card. \n 
		  */
		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		virtual std::string asString () const override;

		std::string _signature;						// $00 - $0f :	No more than 16 chars...
		unsigned int _headerSize;					// $10 - $13 :	Usually $40 = 64.
		unsigned short _cartridgeVersion;			// $14 - $15
		unsigned short _cartridgeType;				// $16 - $17
		bool _EXROMActive;							// $18 Only valid for C64 machines
		bool _GAMEActive;							// $19 Only valid for C64 machines
		// $20 - $23 Reserved for future used...
		std::string _name;							// $20 - $3f :	No more than 32 chars...
		ChipsData _chipsData;						// $40 - ...
	};

	/** To read/write the CRT type files. */
	class CRTFileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		CRTFileTypeIO ()
			: MCHEmul::FileTypeIO ()
							{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <CRTFileData*> (fD) != nullptr); }
		/** This type of file can not be written back. */
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override
							{ return (false); }
	};

	/** In the COMMODORE the keystrokes are generated in a special manner. */
	class KeystrokeTypeIO final : public MCHEmul::KeystrokeTypeIO
	{
		public:
		KeystrokeTypeIO (const std::map <char, MCHEmul::Strings>& sK = _DEFAULTSPECIALKEYS)
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
}

#endif
  
// End of the file
/*@}*/
