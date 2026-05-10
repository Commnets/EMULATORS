/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: 1540SeriesDisk.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/01/2026 \n
 *	Description: To emulate the behaviour of the 1540 Disk Series Unit (1540,1541,...) \n
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_1540SERIESDISK__
#define __COMMODORE_1540SERIESDISK__

#include <CORE/incs.hpp>
#include <COMMODORE/FileIO.hpp>
#include <COMMODORE/SerialIOPeripherals.hpp>

namespace COMMODORE
{
	/** To emulate the behaviour of the 1540 Series Disk Unit (1540,1541,...). \n
		It is mainly used to load data into the memory of the computer. */
	class Disk1540SeriesSimulation : public SerialIOPeripheralSimulation
	{
		public:
		static const MCHEmul::Attributes _ATTRIBUTES;

		Disk1540SeriesSimulation (
			int id, unsigned char dN,
			const MCHEmul::ASCIIToCodeConverter* cnv, // This class is not the owner of the object but ther Emulation instead!
			const SerialIOPeripheralSimulation::Definition& dt);

		virtual bool initialize () override
							{ return (true); }
		virtual bool finalize () override
							{ return (true); }

		virtual bool connectData (MCHEmul::FileData* dt) override;
		/** No data can be retrieved using this device. */
		virtual MCHEmul::FileData* retrieveData () const override
							{ return (nullptr); }

		/** 
		  * The info included is the one of the parent, plus: \n
		  * DATA	= InfoStructuer: The data kept in the disk. Just the first 256.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** What every device does, depends on the type of the device and their specific KERNEL. \n
			So, these methods must be overloaded per tyep of device. \n
			All they return the status code of the execution. */
		virtual unsigned char listen (MCHEmul::CPU* cpu, const MCHEmul::UByte& b);
		virtual unsigned char unlisten (MCHEmul::CPU* cpu, const MCHEmul::UByte& b);
		virtual unsigned char talk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b);
		virtual unsigned char untalk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b);
		virtual unsigned char openChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn);
		virtual unsigned char closeChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn);
		virtual unsigned char sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		virtual unsigned char receiveByte (MCHEmul::CPU* cpu, MCHEmul::UByte& b) override;

		// To get the information of the disk attending the tracks and sector structure...
		// ...structure that was given through the connectData method...
		/** To get a copy of a DataMemoryBlock for a specific track and sector,
			from the data received when the disk were connected. Empty if it didn't exit. \n
			It returns a single DataMemoryBlock. */
		MCHEmul::DataMemoryBlock dataBlockPerTrackAndSector (size_t track, size_t sector) const;
		/** To get a DataMemoryBlocks structure with the data for a specific track. \n
			If the track doesn't exist, an empty structure is returned. 
			It returns a DataMemoryBlock per sector within the track. \n
			It is supossed that the content is returned clasified pero sector. */
		MCHEmul::DataMemoryBlocks dataBlocksPerTrack (size_t track) const;

		// Methods to build blocks of bytes to answer, depending in the command received...
		/** To get the name of all files in the directory, 
			its position in the disk and its size. */

		/** To determine whetehr a name matches or not wit a patterm
			The pattern might be made up of wildcards '?', '*'. */
		bool nameMatchesWithPattern (const std::string& n, const std::string& p) const;
		/** To split a command into a pre and a post data, separated both for a ":" */
		inline std::tuple <std::string, std::string> splitCommandIntoPreAndPostData (const std::string& c) const;
		/** When the directory is requested. 
			The filter is received as parameter. */
		std::vector <MCHEmul::UByte> buildAnswerToDirCommand 
			(const std::tuple <const std::string, const std::string>& prm) const;
		/** When another file is requested. */
		std::vector <MCHEmul::UByte> buildAnswerToFileCommand
			(const std::tuple <const std::string, const std::string>& prm) const;
		/** To get the intial sector annd track of a file name. \n
			wildcards might be used to specific the file name (in PETSCII). \n 
			When the wildcard * is at the beginning the info returned will allow to the first file. */
		std::tuple <size_t, size_t> getInitialTrackAndSectorOfFile (const std::string& fN) const;
		/** To get the data of a file from a sector and track. 
			It might be recursive, until the end of the file was found. */
		std::vector <MCHEmul::UByte> getDataOfFileFromTrackAndSector (size_t t, size_t s) const;

		protected:
		/** The ascii conversor. */
		const MCHEmul::ASCIIToCodeConverter* _asciiConverter;
		/** The data of the disk, given through the connectData method. */
		MCHEmul::ExtendedDataMemoryBlocks _data;
		/** The original connected data in the original format allowed. */
		D64FileData* _d64FileData;

		/** Different statusd of what the disk is answering... */
		enum class DiskFirmwareStatus
		{
			// Not doing anything so far...
			_IDLE, 
			// The disk is receiving a command, 
			// so it is receiving data from the computer.
			_RECEIVINGCOMMAND,	
			// The disk is answering a command, 
			// so it is transmitting data to the computer.
			_ANSWERINGCOMMAND,	
		};

		// Immplementation
		/** The channel to be used in the current command. */
		unsigned char _commandChannel;
		/** But also the channel that was used last time. */
		unsigned char _lastCommandChannel;
		/** Just to keep the command being transmited to the 1541 unit. */
		std::string _currentCommand;
		/** The status of the firmware. */
		enum DiskFirmwareStatus _firmwareStatus;
		/** Block of bytes to be sent back to the computer 
			when a specific command is received. \n
			The content of the block will depend on the command but is built first time the command
			has to be answered anytime. */
		std::vector <MCHEmul::UByte> _blockToAnswer;
		/** The byte of the previous block that is being answered. */
		unsigned short _byteFromBlockToAnswerToSend;
	};

	// ---
	inline std::tuple <std::string, std::string> 
		Disk1540SeriesSimulation::splitCommandIntoPreAndPostData (const std::string& c) const
	{
		return (std::make_tuple 
			((c.find (':') != std::string::npos) ? c.substr (0, c.find (':')) : c, 
			 (c.find (':') != std::string::npos) ? c.substr (c.find (':') + 1) : ""));
	}
}

#endif
  
// End of the file
/*@}*/
