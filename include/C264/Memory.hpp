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
		static const int _RAM								= 0;
		static const int _BASICROM							= 1;
		static const int _3PLUS1ROM1						= 2;
		static const int _CARTRIDGE1LOWROM					= 3;
		static const int _CARTRIDGE2LOWROM					= 4;
		static const int _KERNELROM							= 5;
		static const int _3PLUS1ROM2						= 6;
		static const int _CARTRIDGE1HIGHROM					= 7;
		static const int _CARTRIDGE2HIGHROM					= 8;

		// Subsets that exists from the beginning in any type of machine...
		// B1
		static const int _IO7501PORT_SUBSET					= 100;
		static const int _PAGEZERO_SUBSET					= 101;
		static const int _STACK_SUBSET						= 102;
		static const int _RAM1_SUBSET						= 103;
		// B2
		static const int _RAM2_SUBSET						= 104;
		static const int _RAM2MIRRORIO7501PORT_SUBSET		= 105;
		static const int _RAM2MIRRORPAGEZERO_SUBSET			= 106;
		static const int _RAM2MIRRORSTACK_SUBSET			= 107;
		static const int _RAM2MIRRORRAM1_SUBSET				= 108;
		// B3
		static const int _BASICROM_SUBSET					= 109;
		static const int _3PLUS1ROM1_SUBSET					= 110;
		static const int _CARTRIDGE1LOW_SUBSET				= 111;
		static const int _CARTRIDGE2LOW_SUBSET				= 112;
		static const int _NOEXTENSION_SUBSET				= 113;
		static const int _RAM3_SUBSET						= 114;
		static const int _RAM3MIRRORIO7501PORT_SUBSET		= 115;
		static const int _RAM3MIRRORPAGEZERO_SUBSET			= 116;
		static const int _RAM3MIRRORSTACK_SUBSET			= 117;
		static const int _RAM3MIRRORRAM1_SUBSET				= 118;
		// B4 (Part 1)
		static const int _KERNELROM1_SUBSET					= 119;
		static const int _3PLUS1ROM21_SUBSET				= 120;
		static const int _CARTRIDGE1HIGH1_SUBSET			= 121;
		static const int _CARTRIDGE2HIGH1_SUBSET			= 122;
		static const int _RAM41_SUBSET						= 123;
		static const int _RAM41MIRRORIO7501PORT_SUBSET		= 124;
		static const int _RAM41MIRRORPAGEZERO_SUBSET		= 125;
		static const int _RAM41MIRRORSTACK_SUBSET			= 126;
		static const int _RAM41MIRRORRAM1_SUBSET			= 127;
		static const int _RAM41MIRRORRAM2_SUBSET			= 128;
		// B4 (Part IO)
		static const int _IONOMAPPED2_SUBSET				= 129;
		static const int _IONOMAPPED3_SUBSET				= 130;
		static const int _IONOMAPPED5_SUBSET				= 131;
		static const int _IOTIA8_SUBSET						= 132;
		// B4 (Part 2)
		static const int _KERNELROM2_SUBSET					= 133;
		static const int _3PLUS1ROM22_SUBSET				= 134;
		static const int _CARTRIDGE1HIGH2_SUBSET			= 135;
		static const int _CARTRIDGE2HIGH2_SUBSET			= 136;
		static const int _RAM42_SUBSET						= 137;
		static const int _RAM42MIRRORRAM1_SUBSET			= 138;
		static const int _RAM42MIRRORRAM2_SUBSET			= 139;

		// Views
		static const int _CPU_VIEW							= 0;

		/** The constructor receives the configuration type. */
		Memory (const MCHEmul::Memory::Content& cnt, 
			Type t, unsigned int cfg, const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get the configuration type. */
		Type type () const
							{ return (_type); }
		/** To know the configuration of the computer. */
		unsigned char configuration () const
							{ return (_configuration); }
		/** To get the configuration of the memory. */
		unsigned char memoryConfiguration () const
							{ return (_memoryConfiguration); }
		/** Whether the ROM is or not active. */
		bool ROMactive () const
							{ return (_ROMActive); }

		// Managing the configuration...
		/** To change the configuration of the memory. \n
			The way it is set will depend on the type of memory behind. \n
			Activate or desactivate too the situation of the ROM/RAM in the machine. */
		virtual void setConfiguration (unsigned int cfg, bool a, unsigned char mcfg);
		/** Just to change the configuration without affecting the status of the memory behind. */
		void setConfiguration (unsigned char cfg)
							{ setConfiguration (cfg, ROMactive (), memoryConfiguration ()); } 
		/** 
			Just to change the configuration of the memory. 
			Possible memory configurations:
			Memory
			Conf	B1:$0000	B2:$4000	B3:$8000						B4:$C000
			--------------------------------------------------------------------------------
			C16/C166:
			Configuration 0x00: 16k de RAM
			0x00:	16k RAM		+ Mirror 0	+ BASIC ROM/Mirror 0			+ KERNAL ROM/Mirror
			Configuración 0x01: 32k de RAM
			0x00:	16k RAM		+ 16k RAM	+ BASIC ROM/Mirror 0			+ KERNEL ROM/Mirror 1
			Configuración 0x02: 64k de RAM
			0x02:	16k RAM		+ 16k RAM	+ BASIC ROM/16k RAM				+ KERNEL ROM/16k RAM
			Plus/4:
			Confguración 0x00: 64k de RAM + ROMs
			By default, the system is 3+1 ROM active, cartridges not present.
			0x00:	16k RAM		+ 16k RAM	+ BASIC ROM/16k RAM				+ KERNEL ROM/16k RAM
			0x01:	16k RAM		+ 16k RAM	+ 3+1 ROM/16k RAM				+ KERNEL ROM/16k RAM
			0x02:	16k RAM		+ 16k RAM	+ CARTRIDGE1 LOW ROM/16k RAM	+ KERNEL ROM/16k RAM
			0x03:	16k RAM		+ 16k RAM	+ CARTRIDGE2 LOW ROM/16k RAM	+ KERNEL ROM/16k RAM
			0x04:	16k RAM		+ 16k RAM	+ BASIC ROM/16k RAM				+ 3+1 ROM/16k RAM
			0x05:	16k RAM		+ 16k RAM	+ 3+1 ROM/16k RAM				+ 3+1 ROM/16k RAM
			0x06:	16k RAM		+ 16k RAM	+ CARTRIDGE1 LOW ROM/16k RAM	+ 3+1 ROM/16k RAM
			0x07:	16k RAM		+ 16k RAM	+ CARTRIDGE2 LOW ROM/16k RAM	+ 3+1 ROM/16k RAM
			0x08:	16k RAM		+ 16k RAM	+ BASIC ROM/16k RAM				+ CARTRIDGE1 HIGH ROM/16k RAM
			0x09:	16k RAM		+ 16k RAM	+ 3+1 ROM/16k RAM				+ CARTRIDGE1 HIGH ROM/16k RAM
			0x0a:	16k RAM		+ 16k RAM	+ CARTRIDGE1 LOW ROM/16k RAM	+ CARTRIDGE1 HIGH ROM/16k RAM
			0x0b:	16k RAM		+ 16k RAM	+ CARTRIDGE2 LOW ROM/16k RAM	+ CARTRIDGE1 HIGH ROM/16k RAM
			0x0c:	16k RAM		+ 16k RAM	+ BASIC ROM/16k RAM				+ CARTRIDGE2 HIGH ROM/16k RAM
			0x0d:	16k RAM		+ 16k RAM	+ 3+1 ROM/16k RAM				+ CARTRIDGE2 HIGH ROM/16k RAM
			0x0e:	16k RAM		+ 16k RAM	+ CARTRIDGE1 LOW ROM/16k RAM	+ CARTRIDGE2 HIGH ROM/16k RAM
			0x0f:	16k RAM		+ 16k RAM	+ CARTRIDGE2 LOW ROM/16k RAM	+ CARTRIDGE2 HIGH ROM/16k RAM
		*/
		void setMemoryConfiguration (unsigned char mcfg)
							{ setConfiguration (configuration (), ROMactive (), mcfg); }
		/** Just to activate or desactivate the ROM, 
			maintaining the same configuration. */
		void setROMactive (bool a)
							{ setConfiguration (configuration (), a, memoryConfiguration ()); }

		// Knowing different memory configurations...
		/** To determine whether the configuration for the low rom is basic or not. */
		bool isMemoryConfigurationLowROMBasic () const
							{ return (_memoryConfiguration == 0x00 || 
									  _memoryConfiguration == 0x04 || 
									  _memoryConfiguration == 0x08 || 
									  _memoryConfiguration == 0x0c); }
		/** To determine whether the configuration for the low rom is 3+1 or not. */
		bool isMemoryConfigurationLowROM3plus1 () const
							{ return (_memoryConfiguration == 0x01 || 
									  _memoryConfiguration == 0x05 || 
									  _memoryConfiguration == 0x09 || 
									  _memoryConfiguration == 0x0d); }
		/** To determine whether the configuration for the low rom is cartridge 1 or not. */
		bool isMemoryConfigurationLowROMCartridge1 () const
							{ return (_memoryConfiguration == 0x02 || 
									  _memoryConfiguration == 0x06 || 
									  _memoryConfiguration == 0x0a || 
									  _memoryConfiguration == 0x0e); }
		/** To determine whether the configuration for the low rom is cartridge 2 or not. */
		bool isMemoryConfigurationLowROMCartridge2 () const
							{ return (_memoryConfiguration == 0x03 || 
									  _memoryConfiguration == 0x07 || 
									  _memoryConfiguration == 0x0b || 
									  _memoryConfiguration == 0x0f); }
		/** To determine whether the configuration for the high rom is basic or not. */
		bool isMemoryConfigurationHighROMKernel () const
							{ return (_memoryConfiguration == 0x00 || 
									  _memoryConfiguration == 0x01 || 
									  _memoryConfiguration == 0x02 || 
									  _memoryConfiguration == 0x03); }
		/** To determine whether the configuration for the high rom is 3+1 or not. */
		bool isMemoryConfigurationHighROM3plus1 () const
							{ return (_memoryConfiguration == 0x04 || 
									  _memoryConfiguration == 0x05 || 
									  _memoryConfiguration == 0x06 || 
									  _memoryConfiguration == 0x07); }
		/** To determine whether the configuration for the high rom is cartridge 1 or not. */
		bool isMemoryConfigurationHighROMCartridge1 () const
							{ return (_memoryConfiguration == 0x08 || 
									  _memoryConfiguration == 0x09 || 
									  _memoryConfiguration == 0x0a || 
									  _memoryConfiguration == 0x0b); }
		/** To determine whether the configuration for the high rom is cartridge 2 or not. */
		bool isMemoryConfigurationHighROMCartridge2 () const
							{ return (_memoryConfiguration == 0x0c || 
									  _memoryConfiguration == 0x0d || 
									  _memoryConfiguration == 0x0e || 
									  _memoryConfiguration == 0x0f); }
		/** To know whether a cartridge is connected. */
		bool cartridgeConnected () const
							{ return (_cartridge1Connected || _cartridge2Connected); }

		// Managing the cartridges...
		void loadCartridge1 (const std::string& f1, const std::string& f2);
		/** The cartridge can be connected onle when it has been loaded. 
			Same for 1 and 2. */
		void connectCartridge1 (bool a)
							{ _cartridge1Connected = _cartridge1Loaded ? a : false; }
		void loadCartridge2 (const std::string& f1, const std::string& f2);
		void connectCartridge2 (bool a)
							{ _cartridge2Connected = _cartridge2Loaded ? a : false; }

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
		/** The configuration of the computer. */
		unsigned int _configuration;
		/** The configuration of the memory. */
		unsigned char _memoryConfiguration;
		/** To know whether the ROM is or not active. 
			The original value is also kept in the chip emulating the ROM/RAM switching. */
		bool _ROMActive;
		/** The situation of the cartridges. */
		bool _3plus1Loaded; // Never lodaed by default. Loaded in CPLUS4 type...
		bool _cartridge1Loaded, _cartridge1Connected;
		bool _cartridge2Loaded, _cartridge2Connected;

		// Implementation
		// Things that can be connected and disconnected...
		// B1
		MCHEmul::PhysicalStorageSubset*			_RAM1;
		// B2
		MCHEmul::PhysicalStorageSubset*			_RAM2;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM2MirrorIO7501Port;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM2MirrorPageZero;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM2MirrorStack;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM2MirrorRAM1;
		// B3
		MCHEmul::PhysicalStorageSubset*			_basicROM;
		MCHEmul::PhysicalStorageSubset*			_3plus1ROM1;
		MCHEmul::PhysicalStorageSubset*			_cartridge1Low;
		MCHEmul::PhysicalStorageSubset*			_cartridge2Low;
		MCHEmul::EmptyPhysicalStorageSubset*	_noExtension;
		MCHEmul::PhysicalStorageSubset*			_RAM3;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM3MirrorIO7501Port;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM3MirrorPageZero;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM3MirrorStack;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM3MirrorRAM1;
		// B4 (Part 1)
		MCHEmul::PhysicalStorageSubset*			_kernelROM1;
		MCHEmul::PhysicalStorageSubset*			_3plus1ROM21;
		MCHEmul::PhysicalStorageSubset*			_cartridge1High1;
		MCHEmul::PhysicalStorageSubset*			_cartridge2High1;
		MCHEmul::PhysicalStorageSubset*			_RAM41;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM41MirrorIO7501Port;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM41MirrorPageZero;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM41MirrorStack;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM41MirrorRAM1;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM41MirrorRAM2;
		// B4 (Part IO)
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped2;
		C6529B1Registers*						_IO6529B1;		// This is the one controlling the keyboard...
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped3;
		C1C2SelectorRegisters*					_IOC1C2Selector; // To manage the C1 and C2 lines...
		MCHEmul::EmptyPhysicalStorageSubset*	_IOnomapped5;
		COMMODORE::TIARegisters*				_IOTIA9;
		MCHEmul::MirrorPhysicalStorageSubset*	_IOTIA8;
		TEDRegisters*							_IOTED;
		ROMRAMSwitchRegisters*					_ROMRAMSwitch;
		// B4 (Part 2)
		MCHEmul::PhysicalStorageSubset*			_kernelROM2;
		MCHEmul::PhysicalStorageSubset*			_3plus1ROM22;
		MCHEmul::PhysicalStorageSubset*			_cartridge1High2;
		MCHEmul::PhysicalStorageSubset*			_cartridge2High2;
		MCHEmul::PhysicalStorageSubset*			_RAM42;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM42MirrorRAM1;
		MCHEmul::MirrorPhysicalStorageSubset*	_RAM42MirrorRAM2;
	};

	/** The memory for the C16/116. */
	class C16_116Memory final : public Memory
	{
		public:
		// Subsets that only exists in the C16/C116 type of machine 
		static const int _IONOMAPPED0_SUBSET	= 150;		// Instead of ACIA
		static const int _IONOMAPPED1_SUBSET	= 151;		// Instead of C65529B2

		C16_116Memory (unsigned int cfg, const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		virtual void setConfiguration (unsigned int cfg, bool a, unsigned char mcfg) override;

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
		CPlus4Memory (unsigned int cfg, const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		virtual void setConfiguration (unsigned int cfg, bool a, unsigned char mcfg) override;

		private:
		COMMODORE::ACIARegisters*				_IOACIA;
		C264::C6529B2Registers*					_IO6529B2;

		static MCHEmul::Memory::Content standardMemoryContent ();
	};
}

#endif
  
// End of the file
/*@}*/
