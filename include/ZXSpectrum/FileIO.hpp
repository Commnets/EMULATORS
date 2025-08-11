/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description:	The specific file readers for ZXSpectrum emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_FILEIO__
#define __ZXSPECTRUM_FILEIO__

#include <CORE/incs.hpp>
#include <SINCLAIR/incs.hpp>
#include <ZXSpectrum/Type.hpp>

namespace ZXSPECTRUM
{
	/** The structure can be valid for: .TAP files. \n
		The structure is made up of blocks of data. \n
		There must be 1 block at least. \n
		The structure of every blosk is: \n
		-> 2 bytes with the length of the block, not including this two bytes but including the checksum. \n
		-> 1 byte with the type of block. 0x00 for the header and 0xff for data. \n
		-> x data bytes. \n
		-> 1 byte for the check sum. \n
		The structure of the header file is: \n
		-> 1 byte with the type of code. 0 program, 1 number array, 2 character and 3 code. \n
		-> 10 bytes with the name of the file. \n
		-> 2 bytes with the length of the data. \n
		-> 2 bytes for a parameter 1 and \n
		-> 2 bytes more for a parameter 2. \n
		In the case of a program file, the parameter 1 will represent the start 1, and the 2 the autostart line. \n
		So, the minimum length of a TAP file is: 21 bytes. */
	struct TAPFileData final : public MCHEmul::FileData
	{
		// All info blocks inherits from this one...
		struct InfoBlock
		{
			InfoBlock ()
				: _length (0),
				  _data (),
				  _checksum (0)
							{ }

			virtual ~InfoBlock ()
							{ }

			/** The way the block is recognized into the definition file. */
			virtual unsigned char flag () const
							{ return (0xff); }

			/** The block as a memory block. */ 
			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const;

			/** The block as a string. */
			virtual std::string asString () const;

			/** The block is filled from a byte array, usually data read from a definition file. \n
				The variable e will be true if there were any error reading the data. */
			virtual void fillFrom (char* bd, unsigned short nR, bool& e);
	
			unsigned short _length;
			MCHEmul::DataMemoryBlock _data;

			// Implementation...
			unsigned char _checksum;
		};

		// The header block is a specific type of block...
		// ...to define the length of the data and the type of data...
		struct HeaderBlock final : public InfoBlock
		{
			enum class Type
			{
				_PROGRAM = 0,
				_NUMERICARRAY = 1,
				_STRINGARRAY = 2,
				_CODE = 3
			};

			HeaderBlock ()
				: InfoBlock (),
				  _type (Type::_PROGRAM),
				  _name (""),
				  _dataLength (0), // The length in terms of bytes of the data after this header...
				  _parameter1 { 0x00, 0x00 },_parameter2 { 0x00, 0x00 }
							{ }

			virtual unsigned char flag () const override
							{ return (0x00); }

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			virtual std::string asString () const override;

			virtual void fillFrom (char* bd, unsigned short nR, bool& e) override;

			Type _type; // Type of block...
			std::string _name; // Name of the block...
			unsigned short _dataLength; // Defined in the header, and regarding the next...
			char _parameter1 [2], _parameter2 [2]; // 2 parameters which meaning will depend on the type before...
		};

		using InfoBlocks = std::vector <InfoBlock*>;

		TAPFileData ()
			: MCHEmul::FileData (),
			  _infoBlocks ()
							{ }

		~TAPFileData ();

		/** Get the info as memory data blocks, with the following attributes per data block. \n
			HEADERLENGTH: Length of the header. Usually two bytes less than the data length. \n
			HEADERCHECKSUM: Checksum used for the header info. \n
			DATALENGTH: Length of the data. \n
			TYPE: Type of data. \n
			PRM10, PRM11, PRM20 and PRM21: The meaning will depend on the type of data. \n
			DATACHECKSUM: Checksum of the data. */
		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		virtual std::string asString () const override;

		InfoBlocks _infoBlocks;
	};

	/** To read/write TAPFileData. */
	class TAPFileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		TAPFileTypeIO ()
			: MCHEmul::FileTypeIO ()
									{ }

		virtual bool canRead (const std::string& fN) const override;
		/** If there were any error in the structure of the file (partial checksums didn't match or type of block not supported) 
			an empty object will be returned (with no data blocks at all). This situation has to be managed. */
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <TAPFileTypeIO*> (fD) != nullptr); }
		/** This type of information can not be written back to the file. */
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override
							{ return (false); }
	};

	/** This structure was created by Tomas Kaz, Martjin Van der Heide & Jordi Pérez (metalbrain). \n
		The structure is very flexible and complex. \n
		It was created to maintain the information as it was originally created. \n
		It stores the information in types of blocks. \n
		There are many types of blocks. The format of each can be very different.
		One type of block is equivalent to TAP...but... */
	struct TZXFileData final : public MCHEmul::FileData
	{
		/** This format always has a constant header. \n
			The only thing that can vary are the version and 
			subversion and the number of blocks to load. */
		struct Header final
		{
			Header ()
				: _name ("ZXTape!"), _majorVersion (1), _minorVersion (0)
							{ }

			Header (unsigned char mav, unsigned char miv)
				: _name ("ZXTape!"), _majorVersion (mav), _minorVersion (miv)
							{ }

			std::string _name;
			// There is 1 byte between the name and the versions...
			unsigned char _majorVersion, _minorVersion;

			std::string asString () const
							{ return (_name + " Version:" + 
								std::to_string ((int) _majorVersion) + "," + std::to_string ((int) _minorVersion)); }
		};

		/** All type of blocks inherit from this one. \n
			Every DataBlock can be transformed into "DirectRecordingBytes". \n
			This format is the only one that is understood by the peripheral Datasett (@see the documentation of the block). */
		struct DataBlock
		{
			DataBlock (unsigned char t, const std::vector <MCHEmul::UByte>& d = { })
				: _type (t),
				  _data (d)
							{ }

			virtual std::string asString () const 
							{ return ("Type:" + std::to_string ((int) _type) + 
								"(" + std::to_string (_data.size ()) + " bytes)"); }

			// To determine how portable the information in the block is...
			/** To know whether the block has sampling info. \n
				By default none is. */
			virtual bool isSamplingInfo () const
							{ return (false); }
			/** To know whether the blokc has like TAP formato info. 
				By default none is. */
			virtual bool isLikeTAPInfo () const
							{ return (false); }

			/** To returns the data of the block adapted to MCHEmul framework. */
			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const;

			unsigned char _type;
			std::vector <MCHEmul::UByte> _data;
		};

		/** The most standard data block in TZX.
			The info is saved like in TAP format, plus the length of the pause after the block. \n
			Additional info is kept also to transform, if needed, the bytes into direct-recording. */
		struct StandardSpeedDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 10;

			StandardSpeedDataBlock ()
				: DataBlock (_ID, { }), 
				  // The values are the standard ones...
				  _pilotPulse (2168), _pilotNumberPulses (8063), _syncFirstPulse (667), _syncSecondPulse (735),
				  _zeroPulse (855), _onePulse (1710),
				  _bitsUsedLast (8),
				  _blockLapse (1000)
							{ }

			StandardSpeedDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			virtual bool isLikeTAPInfo () const override
							{ return (true); }

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			/** The duration in miliseconds of the lapse time after the block.
				usually 1000 miliseconds. */
			unsigned short _pilotPulse, _pilotNumberPulses, _syncFirstPulse, _syncSecondPulse;
			unsigned short _zeroPulse, _onePulse;
			unsigned short _bitsUsedLast;
			unsigned short _blockLapse;
		};

		/** This block represents Direct Recording. \n
			In this fomat every bit in every byte represente or a 1 or a 0 in the EAR (bit 6 port 0xfe)
			The bits are scanned from the MSB to the LSB every x T-States. \n
			Those T-States are also defined in the parameters of the data block. */
		struct DirectRecordinDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 15;

			DirectRecordinDataBlock ()
				: DataBlock (_ID, { }),
				  _tstates (0), _blockLapse (0), _bitsUsedLast (8)
							{ }

			DirectRecordinDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			/** This one is smapling info. */
			virtual bool isSamplingInfo () const override
							{ return (true); }

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			unsigned short _tstates;
			unsigned short _blockLapse;
			unsigned short _bitsUsedLast;
		};

		/** The block with information about the program. */
		struct InformationDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 20;

			InformationDataBlock ()
				: DataBlock (_ID, { }),
				  _title (""), _house (""), _author (""), _year (""), _language (""), 
				  _game (""), _price (""), _protection (""), _origin (""),
				  _comments { }
							{  }

			InformationDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			virtual std::string asString () const override
			{ return (DataBlock::asString () + "(" +
				"Title:" + _title + "," + "House:" + _house + "," + "Author:" + _author + "," +
				"Year:" + _year + "," + "Language:" + _language + "," + "Game:" + _game + "," +
				"Protection:" + _protection + "," +	"Origin:" + _origin + "," +
				"Comments:" + MCHEmul::concatenateStrings (_comments, ".") + ")"); }

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			/** This block is neither sampling not normal info. 
				It might be excluded from the checks about that behaviour. \n
				However, by default, the method returns false. */

			std::string _title;
			std::string _house;
			std::string _author;
			std::string _year;
			std::string _language;
			std::string _game;
			std::string _price;
			std::string _protection;
			std::string _origin;
			MCHEmul::Strings _comments;
		};

		TZXFileData ()
			: _header (), _dataBlocks { }, _attributes { }
							{ }

		// From a memory data block...
		TZXFileData (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
			: _header (), _dataBlocks { }, _attributes { }
							{ addFromMemoryBlock (dMB); }

		~TZXFileData ();

		bool isSamplingInfo () const;
		bool isLikeTAPInfo () const;

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		/** Adds all info possible from a ExtendedDataMemoryBlock. */
		void addFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
							{ addHeaderFromMemoryBlock (dMB); addDataBlockFromMemoryBlock (dMB); }
		/** Adds the header from a ExtendedDataMemoryBlock. */
		void addHeaderFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB);
		/** Creates and adds (if possible) the dataBlocks from a ExtendedDataMemoryBlock info received as parameter. \n
			If not possible, a nullptr is returned. \n
			The method behaves as a factory method. */
		void addDataBlockFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB);
		/** Create a new object from a ExtendedDataMemoryBlock...if possible. */
		static TZXFileData* createFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
							{ return (new TZXFileData (dMB)); }

		virtual std::string asString () const override;

		/** The info contained in the file, 
			is just the header and a collection of data blocks. */
		Header _header;
		std::vector <DataBlock*> _dataBlocks;
		MCHEmul::Attributes _attributes;
	};

	/** To read/wrote TZX files. */
	class TZXFileTypeIO final : public MCHEmul::FileTypeIO
	{
		public:
		TZXFileTypeIO ()
			: MCHEmul::FileTypeIO ()
							{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (MCHEmul::FileData* fD) const override
							{ return (dynamic_cast <TZXFileData*> (fD) != nullptr); }
		/** This info can be written back to the file, because it is the one managed by the datsette. */
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override;

		private:
		// Implementation
		// The read/write the different type of blocks.
		// The methods returns a reference to a new block of data if everything went ok and nullptr in other case.
		/** The block type 10, is the most standard one. */
		TZXFileData::StandardSpeedDataBlock* readBlockType10 (std::ifstream& f) const;
		void writeBlockType10 (std::ofstream& f, TZXFileData::StandardSpeedDataBlock* b) const;
		/** The block type 15 is where the direct information is kept. */
		TZXFileData::DirectRecordinDataBlock* readBlockType15 (std::ifstream& f) const;
		void writeBlockType15 (std::ofstream& f, TZXFileData::DirectRecordinDataBlock* b) const;
		/** The block yype 20 is the one with the information about the program. */
		TZXFileData::InformationDataBlock* readBlockType20 (std::ifstream& f) const;
		void writeBlockType20 (std::ofstream& f, TZXFileData::InformationDataBlock* b) const;
	};

	/** In a ZXSpectrum the way the tokens are read and 
		the keystrokes associated are managed is totally different. */
	class KeystrokeTypeIO final : public SINCLAIR::KeystrokeTypeIO
	{
		public:
		KeystrokeTypeIO ()
			: SINCLAIR::KeystrokeTypeIO (_ZXSPECTRUMTOKENS, _ZXSPECTRUMKEYSTROKES)
							{ }

		private:
		virtual bool isARealToken (char u, const std::string& l, size_t p) const override;

		private:
		static const MCHEmul::Strings _ZXSPECTRUMTOKENS;
		static const std::map <std::string, MCHEmul::Strings> _ZXSPECTRUMKEYSTROKES;
	};
}

#endif
  
// End of the file
/*@}*/
