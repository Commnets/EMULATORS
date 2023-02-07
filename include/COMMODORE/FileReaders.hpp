/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: FileReaders.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 31/01/2023 \n
 *	Description:	The specific file readers for commodore emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_FILEREADERS__
#define __MCHEMUL_FILEREADERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	// Struct to content the cartrige info...
	// It has been extracted from:
	// https://vice-emu.sourceforge.io/vice_17.html
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

		std::string _signature;						// $00 - $0f :	No more than 16 chars...
		unsigned int _headerSize;					// $10 - $13 :	Usually $40 = 64.
		unsigned short _cartridgeVersion;			// $14 - $15
		unsigned short _cartridgeType;				// $16 - $17
		bool _EXROMActive;							// $18
		bool _GAMEActive;							// $19
		// $20 - $23 Reserved for future used...
		std::string _name;							// $20 - $3f :	No more than 32 chars...
		ChipsData _chipsData;						// $40 - ...
	};

	/** To read the CRT type files. */
	class CRTFileTypeReader final : public MCHEmul::FileTypeReader
	{
		public:
		CRTFileTypeReader ()
			: MCHEmul::FileTypeReader ()
									{ }

		virtual bool canRead (const std::string& fN) override;
		virtual MCHEmul::FileData* readFile (const std::string& fN, bool bE = true) override;
	};
}

#endif
  
// End of the file
/*@}*/
