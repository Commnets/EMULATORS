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
			virtual MCHEmul::DataMemoryBlock asMemoryBlock () const;

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

			virtual MCHEmul::DataMemoryBlock asMemoryBlock () const override;

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
		virtual bool writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE = true) const override;
	};
}

#endif
  
// End of the file
/*@}*/
