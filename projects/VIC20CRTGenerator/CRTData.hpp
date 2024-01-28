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
#include <CORE/UInt.hpp>
#include <CORE/Address.hpp>
#include <CORE/DtMemoryBlock.hpp>

namespace VIC20
{
	/** To define the data kept into the CRT file. \n
		That data is made of a header and a set of data blocks. */
	class CRTData final
	{
		public:
		/** The header of the CRT file. */
		class Header final
		{
			public:
			static const size_t _SIGNATUREMAXLEN = 16;
			static const size_t _NAMEMAXLEN = 32;

			Header ();

			Header (const std::string& s, const std::string& n, unsigned int l, unsigned short v, unsigned short t);

			const std::string& signature () const
							{ return (_signature); }
			void setSignature (const std::string& n)
							{ _signature = // Always 20 characters. No more no less...
									(n.length () > _SIGNATUREMAXLEN
										? n.substr (_SIGNATUREMAXLEN)
										: n + std::string (_SIGNATUREMAXLEN, '\0').substr (0, _SIGNATUREMAXLEN - n.length ())); }

			const std::string& name () const
							{ return (_name); }
			void setName (const std::string& n)
							{ _name = // Always 32 characters. No more no less...
									(n.length () > _NAMEMAXLEN
										? n.substr (_NAMEMAXLEN)
										: n + std::string (_NAMEMAXLEN, '\0').substr (0, _NAMEMAXLEN - n.length ())); }
			
			const MCHEmul::UBytes& length () const
							{ return (_length);	}
			void setLength (unsigned int l)
							{ setLength (MCHEmul::UInt::fromUnsignedInt (l).bytes ()); }
			void setLength (const MCHEmul::UBytes& l)
							{ _length = l; _length.setMinLength (4, false /** at the beginnig. */); }
			
			const MCHEmul::UBytes& version () const
							{ return (_version); }
			void setVersion (unsigned short v)
							{ setVersion (MCHEmul::UInt::fromUnsignedInt (v).bytes ()); }
			void setVersion (const MCHEmul::UBytes& v)
							{ _version = v; _version.setMinLength (2); }

			// The type will reflect wich banks of memory are on!
			// bit 1 = Block 1, bit 2 = block 2...
			// so e.g. type = 13 => Block 1, 2 & 3 active.
			// Usually autorun will have bit 5 on
			// and there is no possible a type about >= 64! (bit 6 on...)
			const MCHEmul::UBytes& type () const
							{ return (_type); }
			void setType (unsigned short t)
							{ setType (MCHEmul::UInt::fromUnsignedInt (t).bytes ()); }
			void setType (const MCHEmul::UBytes& t)
							{ _type = t; _type.setMinLength (2, false /** at the beginning. */); }

			// To get the header as UBytes...
			MCHEmul::UBytes bytes () const;

			private:
			std::string _signature, /** 20 chars max long. */ _name /** 32 chars max long. */; 
			MCHEmul::UBytes _length /** 4 */, _version /** 2 */, _type /** 2*/;
		};

		/** Every block of data of the cartridge. */
		class Chip final
		{
			public:
			static const size_t _SIGNATUREMAXLEN = 4;

			Chip ()
				: _signature ("CHIP"), // 4 length...
				  _length (MCHEmul::UBytes ({ 0x00, 0x00, 0x00, 0x10 }, true)), // 16 min...when there is no data...
				  _type (MCHEmul::UBytes ({ 0x00, 0x00 }, true)),
				  _data ()
							{ }

			Chip (const std::string& n,
				const MCHEmul::DataMemoryBlock& dt,
				const MCHEmul::UBytes& l = 
					MCHEmul::UBytes ({ 0x00, 0x00, 0x00, 0x10 }, true /** high - low format. */) /** = 0x00000016 */,
				const MCHEmul::UBytes& t =
					MCHEmul::UBytes ({ 0x00, 0x00 }, true /** high - low format */) /** = 0x00000 */);

			Chip (const std::string& n,
				MCHEmul::DataMemoryBlock&& dt,
				const MCHEmul::UBytes& l = 
					MCHEmul::UBytes ({ 0x00, 0x00, 0x00, 0x10 }, true /** high - low format. */) /** = 0x00000016 */,
				const MCHEmul::UBytes& t =
					MCHEmul::UBytes ({ 0x00, 0x00 }, true /** high - low format */) /** = 0x00000 */);

			const std::string& signature () const
							{ return (_signature); }
			void setSignature (const std::string& n)
							{ _signature = // Always 20 characters. No more no less...
									(n.length () > _SIGNATUREMAXLEN
										? n.substr (_SIGNATUREMAXLEN)
										: n + std::string (_SIGNATUREMAXLEN, '\0').substr (0, _SIGNATUREMAXLEN - n.length ())); }

			const MCHEmul::UBytes& length () const
							{ return (_length);	}
			void setLength (unsigned int l)
							{ setLength (MCHEmul::UInt::fromUnsignedInt (l).bytes ()); }
			void setLength (const MCHEmul::UBytes& l)
							{ _length = l; _length.setMinLength (4, false /** at the beginning. */); }

			const MCHEmul::UBytes& type () const
							{ return (_type); }
			void setType (unsigned short t)
							{ setType (MCHEmul::UInt::fromUnsignedInt (t).bytes ()); }
			void setType (const MCHEmul::UBytes& t)
							{ _type = t; _type.setMinLength (2); }
			
			const MCHEmul::DataMemoryBlock& data () const
							{ return (_data); }
			void setData (const MCHEmul::DataMemoryBlock& d)
							{ _data = d; }

			// To get the chip data as UBytes...
			MCHEmul::UBytes bytes () const;

			private:
			std::string _signature;
			MCHEmul::UBytes _length, _type;
			MCHEmul::DataMemoryBlock _data;
		};

		using Chips = std::vector <Chip>;

		CRTData ()
			: _header (),
			  _chips ({ })
							{ }

		const Header& header () const
							{ return (_header); }
		Header& header ()
							{ return (_header); }
		void setHeader (const Header& h)
							{ _header = h; }

		size_t numberChips () const
							{ return (_chips.size ()); }
		const Chips& chips () const
							{ return (_chips); }
		const Chip& chip (size_t n) const
							{ return (_chips [n]); }
		void addChip (const Chip& c)
							{ _chips.emplace_back (c); }
		void addChip (Chip&& c)
							{ _chips.emplace_back (std::move (c)); }
		inline void addChipFrom (const MCHEmul::DataMemoryBlock& blk);
		inline void addChipFrom (MCHEmul::DataMemoryBlock&& blk);

		bool saveTo (const std::string& fN, const Parameters& prms, bool& e);

		private:
		Header _header;
		Chips _chips;
	};

	// ---
	void CRTData::addChipFrom (const MCHEmul::DataMemoryBlock& blk)
	{ 
		MCHEmul::UInt l = MCHEmul::UInt::fromUnsignedInt 
			((unsigned int) (blk.size () + 16 /** length of the header. */));
		l.setMinLength (2 /** at the beginning. */);
							  
		addChip (Chip ("CHIP", blk, l.bytes ())); 
		// The rest of data in the chip is left in default values!
	}

	// ---
	void CRTData::addChipFrom (MCHEmul::DataMemoryBlock&& blk)
	{ 
		MCHEmul::UInt l = MCHEmul::UInt::fromUnsignedInt 
			((unsigned int) (blk.size () + 16));
		l.setMinLength (2);
							  
		addChip (Chip ("CHIP", std::move (blk), l.bytes ())); 
	}
}

#endif
  
// End of the file
/*@}*/
