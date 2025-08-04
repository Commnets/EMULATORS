/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 28/01/2023 \n
 *	Description:	To load specific types of files and convert them into data available for the computer. \n
 *					The type of files are things either linked to the computer or to the emulator. \n
 *					To save data into file in a specific format. \n
 *					The format in this case is choosen from the peripheral (it will use the standard).
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_FILEIO__
#define __MCHEMUL_FILEIO__

#include <CORE/global.hpp>
#include <CORE/DtMemoryBlock.hpp>
#include <fstream>

namespace MCHEmul
{
	/** The struct keeping the data read/written. \n
		It can be extended to cover a specific format. */
	struct FileData
	{
		virtual ~FileData () // Just in case...
							{ }

		/** To get the info of the file as memory blocks,
			that is mainly the standard format understood in the platform to move data into/from memory. */
		virtual ExtendedDataMemoryBlocks asMemoryBlocks () const = 0;

		/** To print out the result. It is used from the commands mainly. */
		virtual std::string asString () const
							{ return (""); }

		friend std::ostream& operator << (std::ostream& o, const FileData& fD)
							{ return (o << fD.asString ()); }
	};

	/** The most common one, is just adding blocks of data memory. */
	struct StadardFileData : public FileData
	{
		DataMemoryBlocks _data;
	};

	/** The class is just to read/write a specific type of file,
		understanding its format. */
	class FileTypeIO
	{
		public:
		FileTypeIO ()
							{ }

		FileTypeIO (const FileTypeIO&) = delete;

		FileTypeIO& operator = (const FileTypeIO&) = delete;

		virtual ~FileTypeIO () 
							{ }

		FileTypeIO (FileTypeIO&&) = delete;

		FileTypeIO& operator = (FileTypeIO&&) = delete;

		/** To verify whether a file exists or not. */
		bool exists (const std::string& fN) const
							{ return (!std::ifstream (fN).fail ()); }

		/** To know whether the file could be read by this specific reader. \n
			Usually the extension of the file will be taken into account but
			other elements culs be considered. */
		virtual bool canRead (const std::string& fN) const = 0;
		/** Reads the file. \n
			If the file can not be read or the internal data is wrong or inconsistent, 
			the result will be a nullptr value. */
		virtual FileData* readFile (const std::string& fN, bool bE = true) const = 0;

		/** To know whether the filedata could be written by this specific output manager. \n
			Usually the type of data is taken into account and also the type of this class. */
		virtual bool canWrite (FileData* fD) const = 0;
		/** Write the file. \n
			If the file might not be written a false would be returned. */
		virtual bool writeFile (FileData*fD, const std::string& fN, bool bE = true) const = 0;
	};

	/** The readers/writters can be grouped in vectors. 
		This is to simplify the way they are managed. */
	using FileTypeIOList = std::vector <FileTypeIO*>;

	/** This class is just to read/write a file. \n
		When a file is read, the data read is stored into the class to improve later speed. */
	class FileIO final
	{
		public:
		/** The parameters received will belong to the FileIO. */
		FileIO (const FileTypeIOList& rL)
			: _IOList (rL),
			  _fileData ()
							{ }

		~FileIO ();

		/** Just read the file. \n
			If the file could not be read or it is inconsistent, a NODATA object reference is returned,
			but no variable related with errors is set. */
		FileData* readFile (const std::string& fN, bool bE = true) const;

		/** Just save a file. \n
			If the file could not be saved (will depend on the type) false it is returned and true in other case. */
		bool saveFile (FileData* fD, const std::string& fN, bool bE = true) const;

		private:
		FileTypeIOList _IOList;

		// Implementation
		/** The data read per name of file. */
		mutable std::map <std::string, FileData*> _fileData;
	};

	/** Struct to content a very basic raw data. \n
		This structure is (C) Ignacio Cea Fornies. \n 
		Using specific programs can be converted into other ones. 
		It is quite similar to the standard format TAP with some simplifications. \n
		16 bytes with the name, \n
		4 bytes with the size, \n
		bytes with tha data (that can be empty). */
	struct RawFileData final : public FileData
	{
		struct Block
		{
			Block ()
				: _name (""),
				  _dataSize (0),
				  _bytes ()
							{ }

			std::string _name;
			unsigned int _dataSize;
			UBytes _bytes;
		};

		RawFileData ()
			: _signature (""),
			  _dataBlocks (0),
			  _blocks ()
							{ }

		virtual ExtendedDataMemoryBlocks asMemoryBlocks () const override;

		virtual std::string asString () const override
							{ return (_signature + 
								"(Blocks: " + std::to_string (_blocks.size ()) + ")"); }

		std::string _signature;
		unsigned int _dataBlocks;
		std::vector <Block> _blocks;
	};

	/** To read/write raw data. \n
		As it is created from the OS. */
	class RawFileTypeIO final : public FileTypeIO
	{
		public:
		RawFileTypeIO ()
			: FileTypeIO ()
									{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (FileData* fD) const override
							{ return (dynamic_cast <RawFileData*> (fD) != nullptr); }
		virtual bool writeFile (FileData* fD, const std::string& fN, bool bE = true) const override;
	};

	/** Structure to store a set keystrokes that are sent to the simulation. \n
		The keystrockes are read from a file and stored in a standard format with in this structure. \n
		The keystrokes have to be connected to a typewriter device	
		that will translate them in specific data to be injected into the simulation. \n
		The simulation is accountable to manage those. \n
		A keystroke is basically the keyletter pressed in the keyboard,
		When that letter is a combination of two is desscribed when 
		the name of the key (e.g CTRL) the sign + and the key. */
	struct KeystrokeData final : public FileData
	{
		KeystrokeData ()
			: _keystrokes ()
							{ }

		/** This type of data has no reflection into data memory blocks. */
		virtual ExtendedDataMemoryBlocks asMemoryBlocks () const override
							{ return (ExtendedDataMemoryBlocks ()); }

		virtual std::string asString () const override
							{ return ("Keystrokes: " + std::to_string (_keystrokes.size ())); }

		/** There will be two types of keystrokes :
			1. The ones that are just the key pressed, like "A", "B", "C", etc. \n
			2. The ones that are a combination of keys, like "CTRL+A", "SHIFT+B", etc.
			3.- The control keys allowed will depend on the element receiving the data...
			The way ones and other are managed will depend on the specific device receiving them. */
		Strings _keystrokes; 
	};

	/** To read/write KeystrockData from a file. \n
		The file has to have the extension .KEY. \n
		Every line read is converted into keystrockes (@see KeystrokeData class). \n
		The way every letter o set of letters is converted in a specific keystroke might depend on the using it emulator. \n
		E.g " (quote) in COMMODORE is LSHIFT+2 (see a picture of the keyboard), but in a ZXSpectrum is LCTRL+P. \n
		\n
		The method generateTokensFor splites a line in different elements. \n
		The default implementation just generates as many tokens as letters are in the line received + new line. \n
		The method generateKeytrokeForToken generates a keystroke for a specific token. \n
		The default implementation generates the keystroke for a specific key as it is in a standard keyboard.
		\n
		In the file received there could be written directly the keystroke to generate when it is a 
		combination of keys. To do so the combination must be written between the simbols "\".
		E.g. \SHIFT+Q\ will mean pressing SHIFT and then Q key (it is the same in upper or lower case). */
	class KeystrokeTypeIO : public FileTypeIO
	{
		public:
		KeystrokeTypeIO ()
			: FileTypeIO ()
							{ }

		virtual bool canRead (const std::string& fN) const override;
		virtual FileData* readFile (const std::string& fN, bool bE = true) const override;

		virtual bool canWrite (FileData* fD) const override
							{ return (dynamic_cast <KeystrokeData*> (fD) != nullptr); }
		virtual bool writeFile (FileData* fD, const std::string& fN, bool bE = true) const override;

		protected:
		/** Identifies the tokens in a line of text. \n
			The implementation will dependn on the specific emulator,
			although the default implementation just separates the letters + end of line. */
		virtual Strings generateTokensFor (const std::string& str) const;
		/** Determine the keystroke for a specific token. \n
			Again it must be defined per emulator, although the default implementation
			identify the keystroke as it is in a standard keyboard. */
		virtual std::string generateKeystrokeForToken (const std::string& t) const;
	};
}

#endif
  
// End of the file
/*@}*/
