/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: C264 Memory.
 *				 The memory is configured in C264 using a combination of PLA (7700-10) and
 *				 a set of flip-flops (74LS175) and decodifiers (74LS139)
 *				 I haven't found detail info about how they works, but "reading" the shematic-diagrams
 *				 something can be programmed!
 *				 see: https://myoldcomputer.nl/wp-content/uploads/2016/02/c16_251788_1.jpg
 *				 see: https://pdf1.alldatasheet.es/datasheet-pdf/download/5674/MOTOROLA/74LS175.html
 *				 see: https://pdf1.alldatasheet.com/datasheet-pdf/download/5657/MOTOROLA/74LS139.html
 *	Versions: 1.0 Initial
 */

#ifndef __C264_MEMORY__
#define __C264_MEMORY__

#include <COMMODORE/incs.hpp>
#include <C264/Type.hpp>
#include <C264/C6529B1Registers.hpp>
#include <C264/C6529B2Registers.hpp>
#include <C264/TEDRegisters.hpp>
#include <C264/ROMRAMSwitchRegisters.hpp>
#include <C264/C1C2SelectorRegisters.hpp>

namespace C264
{
	class TED;

	/** The memory itself for the C264 Series... 
		There will be some classes inheriting this, 
		as the memory of the different machines is not the same, 
		because they don't event have the same chips attached!.  */
	class Memory : public MCHEmul::Memory
	{
		public:
		// Phisical Storages
		static const int _RAM					= 0;
		static const int _BASICROM				= 1;
		static const int _LOWCARTRIDGEROM		= 2;
		static const int _KERNELROM				= 3;
		static const int _HIGHCARTRIDGEROM		= 4;

		// Subsets that exists from the beginning in any type of machine...
		static const int _IO7501PORT_SUBSET		= 100;
		static const int _PAGEZERO_SUBSET		= 101;
		static const int _STACK_SUBSET			= 102;
		static const int _RAM1_SUBSET			= 103;
		static const int _RAM2_SUBSET			= 104;
		static const int _EMPTYRAM2_SUBSET		= 105;
		static const int _BASICROM_SUBSET		= 106;
		static const int _RAM3_SUBSET			= 107;
		static const int _EMPTYRAM3_SUBSET		= 108;
		static const int _LOWCARTRIDGE_SUBSET	= 109;
		static const int _KERNELROM1_SUBSET		= 110;
		static const int _RAM4_SUBSET			= 111;
		static const int _EMPTYRAM4_SUBSET		= 112;	
		static const int _HIGHCARTRIDGE1_SUBSET	= 113;
		static const int _IONOMAPPED2_SUBSET	= 114;
		static const int _IONOMAPPED3_SUBSET	= 115;
		static const int _IONOMAPPED5_SUBSET	= 116;
		static const int _IOTIA8_SUBSET			= 117;
		static const int _KERNELROM2_SUBSET		= 118;
		static const int _RAM5_SUBSET			= 119;
		static const int _EMPTYRAM5_SUBSET		= 120;
		static const int _HIGHCARTRIDGE2_SUBSET	= 121;

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the configuration type. */
		Memory (const MCHEmul::Memory::Content& cnt, 
			Type t, unsigned int cfg, const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get the configuration type. */
		Type type () const
							{ return (_type); }
		/** To know the configuration of the memory. */
		unsigned int configuration () const
							{ return (_configuration); }
		/** Whether the ROM is or not active. */
		bool ROMactive () const
							{ return (_ROMActive); }
		/** The status of the C1 line. */
		bool C1Low () const
							{ return (_C1Low); }
		bool C1High () const
							{ return (_C1High); }
		/** The status of the C2 line. */
		bool C2Low () const
							{ return (_C2Low); }
		bool C2High () const
							{ return (_C2High); }

		/** To change the configuration of the memory. \n
			The way it is set will depend on the type of memory behind. \n
			Activate or desactivate too the situation of the ROM/RAM in the machine,
			and change the situation of the lines C1 and C2.*/
		virtual void setConfiguration (unsigned int cfg, bool a, 
			bool c1l, bool c1h, bool c2l, bool c2h) = 0;
		/** Just to change the configuration without affecting the status of the memory behind. */
		void setConfiguration (unsigned int cfg)
							{ setConfiguration (cfg, ROMactive (), 
								C1Low (), C1High (), C2Low (), C2High ()); }
		/** Just to activate or desactivate the ROM, 
			maintaining the same configuration. */
		void setROMactive (bool a)
							{ setConfiguration (configuration (), a, 
								C1Low (), C1High (), C2Low (), C2High ()); }
		/** Just to configure the status of the two external lines C1 & C2. */
		void setC1C2Lines (bool c1l, bool c1h, bool c2l, bool c2h)
							{ setConfiguration (configuration (), ROMactive (), 
								c1l, c1h, c2l, c2h); }

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		/** The very basic memory for the C16 and C116 models. \n
			The CPLUS4 is a little bit more complicated, so additional zones are required in the IO zone,
			that in the C16/C166 are not mapped: ACIA and C6529B2. */
		static MCHEmul::Memory::Content standardMemoryContent ();

		protected:
		/** The type of computer this memory is for. */
		Type _type;
		/** The configuration of the memory. */
		unsigned int _configuration;
		/** To know whether the ROM is or not active. 
			The original value is also kept in the chip emulating the ROM/RAM switching. */
		bool _ROMActive;
		/** To know which is the status of the C1 and C2 line. 
			It is a copy of the status in the chip emulating the conexión if the cartridges. */
		bool _C1Low, _C1High;
		bool _C2Low, _C2High;

		// Implementation
		MCHEmul::PhysicalStorageSubset*			_RAM1;
		MCHEmul::PhysicalStorageSubset*			_RAM2;
		MCHEmul::EmptyPhysicalStorageSubset*	_EMPTYRAM2;
		MCHEmul::PhysicalStorageSubset*			_basicROM;
		MCHEmul::PhysicalStorageSubset*			_RAM3;
		MCHEmul::EmptyPhysicalStorageSubset*	_EMPTYRAM3;
		MCHEmul::EmptyPhysicalStorageSubset*	_lowCartridge; // When something is plugged in...
		MCHEmul::PhysicalStorageSubset*			_kernelROM1;
		MCHEmul::PhysicalStorageSubset*			_RAM4;
		MCHEmul::EmptyPhysicalStorageSubset*	_EMPTYRAM4;
		MCHEmul::EmptyPhysicalStorageSubset*	_highCartridge1; // When something is plugged in...
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped2;
		C6529B1Registers*						_IO6529B1;		// This is the one controlling the keyboard...
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped3;
		C1C2SelectorRegisters*					_IOC1C2Selector; // To manage the C1 and C2 lines...
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped5;
		COMMODORE::TIARegisters*				_IOTIA9;
		MCHEmul::MirrorPhysicalStorageSubset*	_IOTIA8;
		TEDRegisters*							_IOTED;
		ROMRAMSwitchRegisters*					_ROMRAMSwitch;
		MCHEmul::PhysicalStorageSubset*			_kernelROM2;
		MCHEmul::PhysicalStorageSubset*			_RAM5;
		MCHEmul::EmptyPhysicalStorageSubset*	_EMPTYRAM5;
		MCHEmul::EmptyPhysicalStorageSubset*	_highCartridge2; // When something is plugged in...
	};

	/** The memory for the C16/116. */
	class C16_116Memory final : public Memory
	{
		public:
		// Subsets that only exists in the C16/C116 type of machine 
		static const int _IONOMAPPED0_SUBSET	= 150;		// Instead of ACIA
		static const int _IONOMAPPED1_SUBSET	= 151;		// Instead of C65529B2

		C16_116Memory (unsigned int cfg, const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		virtual void setConfiguration (unsigned int cfg, bool a, 
			bool c1l, bool c1h, bool c2l, bool c2h) override;

		private:
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped0; // Instead of ACIA
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped1; // Instead of C6529B2

		/** Adding the no mapped positions instead ACIA y C6529B2. */
		static MCHEmul::Memory::Content standardMemoryContent ();
	};

	/** And the one for the powerful machine C264. */
	class CPlus4Memory final : public Memory
	{
		public:
		// More physical storage with 4 predefined programs...
		static const int _FUNCTIONLOW			= 3;
		static const int _FUNCTIONHIGH			= 4;

		// ...and the correspondant subsets...
		static const int _FUNCTIONLOW_SUBSET	= 160;
		static const int _FUNCTIONHIGH1_SUBSET	= 161;
		static const int _FUNCTIONHIGH2_SUBSET	= 161;
		// The physical subsets id for the new IO positions are defined in the specific classes...

		CPlus4Memory (unsigned int cfg, const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		virtual void setConfiguration (unsigned int cfg, bool a, 
			bool c1l, bool c1h, bool c2l, bool c2h) override;

		private:
		COMMODORE::ACIARegisters*				_IOACIA;
		C264::C6529B2Registers*					_IO6529B2;

		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		MCHEmul::PhysicalStorageSubset* _functionLow;
		MCHEmul::PhysicalStorageSubset* _functionHigh1;
		MCHEmul::PhysicalStorageSubset* _functionHigh2;
	};
}

#endif
  
// End of the file
/*@}*/
