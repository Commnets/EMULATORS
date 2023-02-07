/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: FileReader.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 28/01/2023 \n
 *	Description:	To load specific types of files and convert them into data available for the computer. \n
 *					The type of files are things either linked to the computer or to the emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_FILEREADER__
#define __MCHEMUL_FILEREADER__

#include <CORE/global.hpp>
#include <CORE/DtMemoryBlock.hpp>

namespace MCHEmul
{
	/** The struct keeping the data read. \n
		It can be extended to cover a specific format. */
	struct FileData
	{
		virtual ~FileData () // Just in case...
							{ }
	};

	/** The most common one, is just adding blocks of data memory. */
	struct StadardFileData : public FileData
	{
		DataMemoryBlocks _data;
	};

	/** The class is just to read a specific type of file,
		understanding its format. */
	class FileTypeReader
	{
		public:
		FileTypeReader ()
							{ }

		FileTypeReader (const FileTypeReader&) = delete;

		FileTypeReader& operator = (const FileTypeReader&) = delete;

		virtual ~FileTypeReader () 
							{ }

		FileTypeReader (FileTypeReader&&) = delete;

		FileTypeReader& operator = (FileTypeReader&&) = delete;

		/** To know whether the file reader could be read by this specific reader. \n
			Usually the extension of the file will be taken into account but
			other elements culs be considered. */
		virtual bool canRead (const std::string& fN) = 0;
		/** Reads the file. \n
			If the file can not be read or the internal data is wrong or inconsistent, 
			the result will be a nullptr value. */
		virtual FileData* readFile (const std::string& fN, bool bE = true) = 0;
	};

	/** The readers can be grouped in vectors. 
		This is to simplify the way they are managed. */
	using FileTypeReaderList = std::vector <FileTypeReader*>;

	/** This class is just to read a file. \n
		When a file is read, the data read is stored into the class to improve later speed. */
	class FileReader final
	{
		public:
		/** The parameters received will belong to the FileReader. */
		FileReader (const FileTypeReaderList& rL)
			: _readerList (rL),
			  _fileData ()
							{ }

		~FileReader ();

		/** Just read the file. \n
			If the file can't be read or it is inconsistent, a NODATA object reference is returned,
			but no variable related with errors is set. */
		FileData* readFile (const std::string& fN, int t, bool bE = true);

		private:
		FileTypeReaderList _readerList;

		// Implementation
		/** The data read per name of file. */
		std::map <std::string, FileData*> _fileData;
	};
}

#endif
  
// End of the file
/*@}*/
