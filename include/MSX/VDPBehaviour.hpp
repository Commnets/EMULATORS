/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: TMS9918Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 28/03/2024 \n
 *	Description: The behaviour of the VDP will be different depending on the generaion.
 *				 However, the behaviour of the next generation is compatible with the previous one.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_VDBEHAVIOUR__
#define __MSX_VDBEHAVIOUR__

#include <CORE/incs.hpp>

namespace MSX
{
	/** Basic Behaviour. 
		Valid for all generations of MSX systems. */
	class VDPBehaviour : public MCHEmul::InfoClass
	{
		friend class VDP;

		public:
		VDPBehaviour (size_t s = 0x4000, // 16k of RAM...the minimum
			const MCHEmul::Attributes& attrs = // The basic attributes!
				{ { "Name", "TMS9918A/TMS9928A/TMS9929A" },
				  { "Manufacturer", "Toshiba" },
				  { "Year", "1982" } });
		
		// Just in case...
		virtual ~VDPBehaviour () = default;

		void initialize ();

		/** To get the attributes. */
		const MCHEmul::Attributes& attributes () const
							{ return (_attributes); }

		/** To get/set a value from the video memory. */
		size_t videoMemorySize () const
							{ return (_videoMemory.size ()); }
		const std::vector <MCHEmul::UByte>& videoMemory () const
							{ return (_videoMemory); }
		const MCHEmul::UByte& videoData (unsigned short pos) const
							{ return (_videoMemory [pos]); }
		void setVideoData (unsigned short pos, const MCHEmul::UByte& v)
							{ _videoMemory [pos] = v; }

		/** The internal behaviour of the chip is controlled using the ports. */
		virtual MCHEmul::UByte readPortValue (unsigned char prtId) const;
		virtual void setPortValue (unsigned char prtId, const MCHEmul::UByte& v);

		/**
		  *	The name of the fields are: \n
		  * Attributes	= InfoStructure: Info about the Behaviour.
		  * MemSize		= Attribute: Size of the video memory.
		  * BYTES		= Attribute: The video memory.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		void initializeInternalValues ();

		/** To get the screen memory. \n
			The position of the screen memory in the MSX is fixed. It is within the graphical chip. \n
			However this position will depend on the graphical mode and also on the MSX generation. */
		virtual MCHEmul::UBytes screenMemorySnapShot () const;
		virtual MCHEmul::UBytes colorMemorySnapShot () const;

		protected:
		/** The Video RAM is stored inside of the VDP. \n
			And its size can vary depending on the generation of MSX. \n
			The basic size is 16k. \n
			The way that amount is stored will depend on the specfic VDP manufacturer,
			but it ise not important regarding the simulation. */
		std::vector <MCHEmul::UByte> _videoMemory;

		/** The attributes that define the specific Chip. */
		MCHEmul::Attributes _attributes;

		// Implementation...
		/** Graphic mode configuration. */
		MCHEmul::UByte _register0;
		/** Screen control. */
		MCHEmul::UByte _register1;
		/** Name table address. */
		MCHEmul::Address _nameAddress;
		/** Color table address. */
		MCHEmul::Address _colorAddress;
		/** Pattern table address. */
		MCHEmul::Address _patternAddress;
		/** Sprite table address. */
		MCHEmul::Address _spriteAddress;
		/** Sprites's attributes table address. */
		MCHEmul::Address _spriteAttrsAddress;
		/** Screen & Sprites control. */
		MCHEmul::UByte _register7;
	};

	/** Extension of the registers for MSX2 systems. */
	class VDPBehaviourMSX2 : public VDPBehaviour
	{
		public:
		VDPBehaviourMSX2 ();

		virtual MCHEmul::UByte readPortValue (unsigned char prtId) const override;
		virtual void setPortValue (unsigned char prtId, const MCHEmul::UByte& v) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		void initializeInternalValues ();

		virtual MCHEmul::UBytes screenMemorySnapShot () const override;
		virtual MCHEmul::UBytes colorMemorySnapShot () const override;
	};
}

#endif
  
// End of the file
/*@}*/
