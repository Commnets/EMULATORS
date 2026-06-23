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

		virtual bool initialize () override;
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
		/** Sets the persistent DOS status returned through channel 15. */
		void setDOSStatus (
			unsigned char c, const std::string& m,
			unsigned char t = 0, unsigned char s = 0);
		/** Executes the command accumulated through channel 15. */
		void executeCommandChannel ();
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
			(const std::tuple <const std::string, const std::string>& prm);
		/** To get the initial sector and track of a file name. \n
			Wildcards might be used in the file name pattern (in PETSCII). \n
			The first directory entry matching the pattern is returned. */
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

		/** State independently kept for every secondary channel. */
		struct ChannelState final
		{
			ChannelState ()
				: _open (false),
				  _command (""),
				  _blockToAnswer { },
				  _answerPrepared (false),
				  _answerStatus (0),
				  _byteFromBlockToAnswerToSend (0),
				  _clearDOSStatusAfterAnswer (false)
									{ }

			/** Resets the current transmission, preserving the opened file name. */
			inline void resetAnswer (unsigned char okResult);

			/** Closes and fully resets the channel. */
			inline void reset (unsigned char okResult);

			/** Whether this secondary channel is currently open. */
			bool _open;
			/** File name, directory request or command received through this channel. */
			std::string _command;
			/** Bytes currently being returned through this channel. */
			std::vector <MCHEmul::UByte> _blockToAnswer;
			/** Whether the answer has already been built. */
			bool _answerPrepared;
			/** Serial status produced while building the answer. */
			unsigned char _answerStatus;
			/** Next byte of the answer to send. */
			size_t _byteFromBlockToAnswerToSend;
			/** Whether channel 15 must become OK after finishing its current answer. */
			bool _clearDOSStatusAfterAnswer;
		};

		// Implementation
		static const size_t _NUMBERCHANNELS = 16;
		/** State independently kept for every secondary channel. */
		std::array <ChannelState, _NUMBERCHANNELS> _channels;
		/** Secondary channel currently selected on the IEC bus. */
		unsigned char _commandChannel;
		/** The status of the firmware. */
		enum DiskFirmwareStatus _firmwareStatus;
		/** Last persistent DOS status returned through channel 15. */
		std::vector <MCHEmul::UByte> _lastDOSStatus;
	};

	// ---
	inline std::tuple <std::string, std::string> 
		Disk1540SeriesSimulation::splitCommandIntoPreAndPostData (const std::string& c) const
	{
		return (std::make_tuple 
			((c.find (':') != std::string::npos) ? c.substr (0, c.find (':')) : c, 
			 (c.find (':') != std::string::npos) ? c.substr (c.find (':') + 1) : ""));
	}

	// ---
	inline void Disk1540SeriesSimulation::ChannelState::resetAnswer (unsigned char okResult)
	{
		_blockToAnswer = { };
		_answerPrepared = false;
		_answerStatus = okResult;
		_byteFromBlockToAnswerToSend = 0;
		_clearDOSStatusAfterAnswer = false;
	}

	// ---
	inline void Disk1540SeriesSimulation::ChannelState::reset (unsigned char okResult)
	{
		_open = false;
		_command = "";
		resetAnswer (okResult);
	}
}

#endif
  
// End of the file
/*@}*/
