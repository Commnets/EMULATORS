/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: CRTData.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 20/01/2024 \n
 *	Description: Represents the structure of the CRT file to generate.
 *	Versions: 1.0 Initial
 */


#ifndef __CRTDATA__
#define __CRTDATA__

#include "Parameters.hpp"
#include <CORE/global.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/Address.hpp>
#include <CORE/DtMemoryBlock.hpp>

namespace VIC20
{
	/** To define te data kept into the file. \n
		That data is made of a header and a set of data blocks. */
	class CRTData final
	{
		public:
		/** The header of the CRTFile. */
		class Header final
		{
			public:
			static const size_t _SIGNATUREMAXLEN = 16;

			Header ()
				: _CRTSignature ("VIC20 Cartridge"), // 16 length including the 0 at the end...
				  _length (MCHEmul::UBytes ({ 0x00, 0x00, 0x00, 0x28 }, true)),
				  _version (MCHEmul::UBytes ({ 0x20, 0x00 }, true)),
				  _type (MCHEmul::UBytes ({ 0x00, 0x00 }, true))
							{ }

			inline Header (const std::string& n,
				const MCHEmul::UBytes& l = 
					MCHEmul::UBytes ({ 0x00, 0x00, 0x00, 0x28 }, true /** high - low format. */) /** = 0x00000040 */,
				const MCHEmul::UBytes& v = 
					MCHEmul::UBytes ({ 0x20, 0x00 }, true /** high - low format */) /** = 20.00 */,
				const MCHEmul::UBytes& t =
					MCHEmul::UBytes ({ 0x00, 0x00 }, true /** high - low format */) /** = 0x00000 */);

			const std::string& CRTSignature () const
							{ return (_CRTSignature); }
			void setCRTSignature (const std::string& n)
							{ _CRTSignature = // Always 20 characters. No more no less...
									(n.length () > _SIGNATUREMAXLEN
										? n.substr (_SIGNATUREMAXLEN)
										: n + std::string (20, '\0').substr (0, _SIGNATUREMAXLEN - n.length ())); }
			const MCHEmul::UBytes& length () const
							{ return (_length);	}
			void setLength (const MCHEmul::UBytes& l)
							{ _length = l; _length.setMinLength (4); }
			const MCHEmul::UBytes& version () const
							{ return (_version); }
			void setVersion (const MCHEmul::UBytes& v)
							{ _version = v; _version.setMinLength (2); }
			const MCHEmul::UBytes& type () const
							{ return (_type); }
			void setType (const MCHEmul::UBytes& t)
							{ _type = t; _type.setMinLength (2); }

			// To get the header as vector of unsigned char...
			std::vector <unsigned char> rowData () const;

			private:
			std::string _CRTSignature; /** 20 chars max long. */
			MCHEmul::UBytes _length /** 4 */, _version /** 2 */, _type /** 2*/;
		};

		CRTData ()
			: _header (),
			  _dataBlocks ({ })
							{ }

		void setHeader (const Header& h)
							{ _header = h; }
		void addDataBlock (const MCHEmul::DataMemoryBlock& blk)
							{ _dataBlocks.emplace_back (blk); }
		void addDataBlock (MCHEmul::DataMemoryBlock&& blk)
							{ _dataBlocks.emplace_back (std::move (blk)); }

		bool saveTo (const std::string& fN, const Parameters& prms, bool& e);

		private:
		Header _header;
		MCHEmul::DataMemoryBlocks _dataBlocks;
	};

	// ---
	inline CRTData::Header::Header (const std::string& n,
			const MCHEmul::UBytes& l, const MCHEmul::UBytes& v,	const MCHEmul::UBytes& t)
		: _CRTSignature (""), // Assign just after this...
		  _length (l),
		  _version (v),
		  _type (t)
	{ 
		setCRTSignature (n); 
		
		_length.setMinLength (4); 
		_version.setMinLength (2); 
		_type.setMinLength (2); 
	}
}

#endif
  
// End of the file
/*@}*/
