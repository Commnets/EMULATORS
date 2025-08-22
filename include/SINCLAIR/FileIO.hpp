/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/08/2025 \n
 *	Description:	The specific file readers for SINCLAIR emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_FILEIO__
#define __SINCLAIR_FILEIO__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
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
			/** Notice that the name of the header is constant!. */
			Header ()
				: _name ("ZXTape!"), _majorVersion (1), _minorVersion (0)
							{ }

			/** Notice that the name of the header is constant!. */
			Header (unsigned char mav, unsigned char miv)
				: _name ("ZXTape!"), _majorVersion (mav), _minorVersion (miv)
							{ }

			std::string _name;
			// There is 1 byte between the name and the versions...
			unsigned char _majorVersion, _minorVersion;

			std::string asString () const;
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

			// To determine how portable the information of the block is between type of Peripherals.
			/** To know whether the block has information that can be sampled. \n
				This is typical of TurboSpeedDataBlock e.g. In DirectRecording the information was already sampled. \n
				By default none has. */
			virtual bool canBeSampled () const
							{ return (false); }
			/** To know whether the block contains information like TAP format. \n
				By default none is. */
			virtual bool isLikeTAPFormat () const
							{ return (false); }
			/** To know whether it is just information about the whole file. \n
				Usually it is true in blocks like InformationDataBlock and HardwareDataBlock. \n
				By default it is false. */
			virtual bool isJustInfo () const
							{ return (false); }

			virtual std::string asString () const;

			/** To returns the data of the block adapted to MCHEmul framework. */
			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const;
			/** Same that before but with sampling the information if possible. 
				If were not possible the byte list would be empty. 
				Additionally when sampled the attribute "SAMPLED" appears and becomes "true". */
			MCHEmul::DataMemoryBlock asSampledDataMemoryBlock () const;

			unsigned char _type;
			std::vector <MCHEmul::UByte> _data;

			protected:
			/** Mut be overloaded to get the data sampled when maked sense. 
				This method is invoked from the previous one, but just when the method canBeSampled returned true. */
			virtual std::vector <MCHEmul::UByte> dataAsSampledBytes () const
							{ return (std::vector <MCHEmul::UByte> { }); }
		};

		/** The most standard data block in TZX.
			The info is saved like in TAP format, plus the length of the pause after the block. \n
			Additional info is kept also to transform, if needed, the bytes into direct-recording. */
		struct StandardSpeedDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 0x10;

			StandardSpeedDataBlock ()
				: DataBlock (_ID, { }), 
				  // The values are the standard ones...
				  _blockLapse (1000) // miliseconds...
							{ }

			/** A basic constructor from data.
				The rest of the attributes must be set one by one or from initializer list. */
			StandardSpeedDataBlock (const std::vector <MCHEmul::UByte>& d)
				: DataBlock (_ID, d),
				  _blockLapse (1000)
							{ }

			/** The constructor from a DatamemoryBlock. \n
				It might be used from a peripheral. */
			StandardSpeedDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			virtual bool isLikeTAPFormat () const override
							{ return (true); }

			virtual std::string asString () const override;

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			/** The duration in miliseconds of the lapse time after the block.
				usually 1000 miliseconds. */
			unsigned short _blockLapse;
		};

		/** A version of the previous one but from a TURBO loader format. */
		struct TurboSpeedDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 0x11;

			TurboSpeedDataBlock ()
				: DataBlock (_ID, { }),
				  _pilotPulse (2168), 
				  _pilotNumberPulses (8063),
				  _syncFirstPulse (667), 
				  _syncSecondPulse (735),
				  _zeroPulse (855), 
				  _onePulse (1710),
				  _bitsUsedLast (8), 
				  _blockLapse (1000) // miliseconds..
							{ }

			/** A basic constructor from data. \n
				The resto of the attributes mut be set one by one. */
			TurboSpeedDataBlock (const std::vector <MCHEmul::UByte>& d)
				: DataBlock (_ID, d),
				  _pilotPulse (2168), 
				  _pilotNumberPulses (8063),
				  _syncFirstPulse (667), 
				  _syncSecondPulse (735),
				  _zeroPulse (855), 
				  _onePulse (1710),
				  _bitsUsedLast (8), 
				  _blockLapse (1000) // miliseconds..
							{ }

			TurboSpeedDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			virtual bool canBeSampled () const override
							{ return (true); }

			virtual std::string asString () const override;

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			/** The duration in miliseconds of the lapse time after the block.
				usually 1000 miliseconds. */
			unsigned short _pilotPulse, _pilotNumberPulses, _syncFirstPulse, _syncSecondPulse;
			unsigned short _zeroPulse, _onePulse;
			unsigned char _bitsUsedLast;
			unsigned short _blockLapse;

			private:
			virtual std::vector <MCHEmul::UByte> dataAsSampledBytes () const override;
		};

		/** This block represents Direct Recording. \n
			In this fomat every bit in every byte represente or a 1 or a 0 in the EAR (bit 6 port 0xfe)
			The bits are scanned from the MSB to the LSB every x T-States. \n
			Those T-States are also defined in the parameters of the data block. */
		struct DirectRecordingDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 0x15;

			DirectRecordingDataBlock ()
				: DataBlock (_ID, { }),
				  _tstates (0), _blockLapse (0), _bitsUsedLast (8)
							{ }

			/** A basic constructor from data. \n
				The rest of the attributes must be set one by one. */
			DirectRecordingDataBlock (const std::vector <MCHEmul::UByte>& d)
				: DataBlock (_ID, d),
				  _tstates (0), 
				  _blockLapse (0), 
				  _bitsUsedLast (8)
							{ }

			DirectRecordingDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			/** This one is smapling info. */
			virtual bool canBeSampled () const override
							{ return (true); }

			virtual std::string asString () const override;

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			unsigned short _tstates;
			unsigned short _blockLapse;
			unsigned short _bitsUsedLast;

			private:
			/** In this block the information is already sampled. */
			virtual std::vector <MCHEmul::UByte> dataAsSampledBytes () const override
							{ return (_data); }
		};

		/** The block with information about the program. */
		struct InformationDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 0x32;

			InformationDataBlock ()
				: DataBlock (_ID, { }),
				  _title (""), 
				  _house (""), 
				  _author (""), 
				  _year (""), 
				  _language (""), 
				  _game (""), 
				  _price (""), 
				  _protection (""), 
				  _origin (""),
				  _comments { }
							{  }

			InformationDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			virtual bool isJustInfo () const override
							{ return (true); }

			virtual std::string asString () const override;

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

		/** The block with information about the hardware where the program runs. */
		struct HardwareDataBlock final : public DataBlock
		{
			static const unsigned char _ID = 0x33;
		
			HardwareDataBlock ()
				: DataBlock (_ID, { }),
				  _machines { }
							{  }

			HardwareDataBlock (const MCHEmul::DataMemoryBlock& dtblk);

			virtual bool isJustInfo () const override
							{ return (true); }

			virtual std::string asString () const override;

			virtual MCHEmul::DataMemoryBlock asDataMemoryBlock () const override;

			/** This block is neither sampling not normal info. 
				It might be excluded from the checks about that behaviour. \n
				However, by default, the method returns false. */

			/** Every machine info is maintained in a very simple info structure. */
			struct MachineInfo final
			{
				MachineInfo (unsigned char t, unsigned char i, unsigned char inf)
					: _type (t), _id (i), _info (inf)
							{ }

				// These three variables are really a enum class
				// Every value means something. e.g. 0 in type means "Compuer" and
				// one 4 in _id means "Spectrum"...
				// However it is not woth to invest too much time in the details...
				unsigned char _type;
				unsigned char _id;
				unsigned char _info;

				std::string asString () const
							{ return ("HW Type:" + std::to_string (_type) + "," +
								"HW Id:" + std::to_string (_id) + "," +
								"HW Info:" + std::to_string (_info)); }

				std::string justAttributes () const
							{ return (std::to_string (_type) + "," + 
									  std::to_string (_id) + "," + 
									  std::to_string (_info)); }
			};

			std::vector <MachineInfo> _machines;
		};

		TZXFileData ()
			: _header (), _dataBlocks { }, _attributes { }
							{ }

		// From a memory data block...
		TZXFileData (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
			: _header (), _dataBlocks { }, _attributes { }
							{ addFromExtendedDataMemoryBlock (dMB); }

		~TZXFileData ();

		/** The full block can be sampled if all not info blocks can be sampled. */
		bool canBeSampled () const;
		/** The full block is like TAP if all not info blocks are like TAP. */
		bool isOnlyLikeTAPInfo () const;

		virtual MCHEmul::ExtendedDataMemoryBlocks asMemoryBlocks () const override;
		/** Returns the same information, but with the data of everyblock sampled. \n
			Takes care because some blocks can not be sampled and they will content no data 
			after this method, even if they have readable information. \n
			Before executing it be sure that canBeSampled returns true. */
		MCHEmul::ExtendedDataMemoryBlocks asSampledMemoryBlocks () const;

		/** Adds all info possible from a ExtendedDataMemoryBlock. */
		void addFromExtendedDataMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
							{ addHeaderFromExtendeDataMemoryBlock (dMB); addDataBlocksFromExtendedDataMemoryBlock (dMB); }
		/** Adds the header from a ExtendedDataMemoryBlock. */
		void addHeaderFromExtendeDataMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB);
		/** Creates and adds (if possible) the dataBlocks from a ExtendedDataMemoryBlock info received as parameter. \n
			If not possible, a nullptr is returned. \n
			The method behaves as a factory method. */
		void addDataBlocksFromExtendedDataMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
							{ for (const auto& i : dMB._data) addDataBlockFromDataMemoryBlock (i); }
		/** To add particular DataMemoryBlock. 
			It is added at the end. */
		void addDataBlockFromDataMemoryBlock (const MCHEmul::DataMemoryBlock& dtblk);
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
		/** The block type 0x10, is the most standard one. */
		TZXFileData::StandardSpeedDataBlock* readBlockType0x10 (std::ifstream& f) const;
		void writeBlockType0x10 (std::ofstream& f, TZXFileData::StandardSpeedDataBlock* b) const;
		/** The block type 0x11, is like 0x10 but originally saved at TURBO apeed. */
		TZXFileData::TurboSpeedDataBlock* readBlockType0x11 (std::ifstream& f) const;
		void writeBlockType0x11 (std::ofstream& f, TZXFileData::TurboSpeedDataBlock* b) const;
		/** The block type 0x15 is where the direct information is kept. */
		TZXFileData::DirectRecordingDataBlock* readBlockType0x15 (std::ifstream& f) const;
		void writeBlockType0x15 (std::ofstream& f, TZXFileData::DirectRecordingDataBlock* b) const;
		/** The block type 0x32 is the one with the information about the program. */
		TZXFileData::InformationDataBlock* readBlockType0x32 (std::ifstream& f) const;
		void writeBlockType0x32 (std::ofstream& f, TZXFileData::InformationDataBlock* b) const;
		/** The block type 0x33 is the one with the information about the hardware where the program runs. 
			It could be user to verify whether the system can run or not the code. */
		TZXFileData::HardwareDataBlock* readBlockType0x33 (std::ifstream& f) const;
		void writeBlockType0x33 (std::ofstream& f, TZXFileData::HardwareDataBlock* b) const;
	};

	/** In a SINCLAIR machine the way the tokens are read and 
		the keystrokes associated are managed is totally different,
		but those TOKENS vary per type of machine, and also the key to press to generate them. */
	class KeystrokeTypeIO : public MCHEmul::KeystrokeTypeIO
	{
		public:
		KeystrokeTypeIO (const MCHEmul::Strings& t,
			const std::map <std::string, MCHEmul::Strings>& k)
			: MCHEmul::KeystrokeTypeIO (),
			  _TOKENS (t), _KEYSTROKES (k)
							{ }

		protected:
		virtual MCHEmul::Strings generateTokensFor (const std::string& str) const override;
		virtual MCHEmul::Strings generateKeystrokeForToken (const std::string& t) const override;

		protected:
		virtual bool isARealToken (char u, const std::string& l, size_t i) const = 0;
		
		protected:
		/** Received as parameter, at construction time. */
		const MCHEmul::Strings _TOKENS;
		const std::map <std::string, MCHEmul::Strings> _KEYSTROKES;
	};
}

#endif
  
// End of the file
/*@}*/
