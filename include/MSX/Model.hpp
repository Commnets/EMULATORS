/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: Model.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: This class represents the "real" computer this emulation is working for...
 *				 There had been many MSXs types along history!
 *	Versions: 1.0 Initial
 *	Based on: https://sinclair.wiki.zxnet.co.uk/wiki/ROM_images and 
 *			  https://www.msx.org/wiki/Category:MSX_systems
 */

#ifndef __MSX_MODEL__
#define __MSX_MODEL__

#include <CORE/incs.hpp>
#include <MSX/VDP.hpp>
#include <MSX/PSG.hpp>
#include <MSX/OSIO.hpp>

namespace MSX
{
	class Memory;

	/** There are many types of MSX computers and 
		with different behaviour and characteristicis. \n
		The Model is not the owner of any element that creates! */
	class MSXModel
	{
		public:
		friend Memory;

		enum class VisualSystem { _NTSC, _PAL };

		enum class Generation 
		{ 
			MSX1 = 0,	// First generation of MSX
			MSX2 = 1
		};

		MSXModel ()
			: _vdp (nullptr), // @see VDP class
			  _psg (nullptr), // @see PSG class
			  _chips (),
			  _ioDevices (),
			  _memory (nullptr)
							{ }

		/** The model is not the owner of any element of the machine,
			but just the VDP wrapper that is created when incoking first time the method vdp (). */
		virtual ~MSXModel ();

		virtual Generation generation () const = 0;

		virtual VisualSystem visualSystem () const = 0;

		/** Can be overloaded, 
			but the default frequency is very standard in the market. */
		virtual double screenFrequency () const
							{ return ((visualSystem () == VisualSystem::_NTSC) ? 60.03f : 50.04); }

		virtual std::string name () const = 0;

		/** To get the attributes of the model. */
		virtual MCHEmul::Attributes attributes () const = 0;

		/** To get the clock speed. */
		virtual unsigned int clockSpeed () const = 0;

		/** Related with the configurations possible for a specific model \n 
			By default only one configuration (the basic one) is allowed. \n
			That basic configuration is defined with the number 0. */
		virtual bool admitConfiguration (unsigned int cfg) const
							{ return (cfg == 0); }
		virtual unsigned int basicConfiguration () const
							{ return (0); }
		unsigned int configurationAdjusted (unsigned int cfg) const
							{ return (admitConfiguration (cfg)
								? cfg : basicConfiguration ()); }

		/** To get the VDP used in the model. \n
			First time this method is invoked the VDP is created. \n
			VDP is a wrapper over the real graphical chip. \n
			The VDP belongs to this class but not the graphical chip that is wrapped, 
			that must belong to the computer as part of the chips managed by it. \n
			So never forget to add it inside the list returned by the method chips (). @see above */
		VDP* vdp () 
							{ return ((_vdp == nullptr) ? _vdp = createVDP () : _vdp); }
		const VDP* vdp () const
							{ return (const_cast <MSXModel*> (this) -> vdp ()); }

		/** To get the PSG used in the model. \n
			First time this method is invoked the PSG is created. \n
			PSG is a wrapper over the real sound chip. \n
			The PSG belongs to this class but not the sound chip that is wrapped, 
			that must belong to the computer as part of the chips managed by it. \n
			So never forget to add it inside the list returned by the method chips (). @see above */
		PSG* psg () 
							{ return ((_psg == nullptr) ? _psg = createPSG () : _psg); }
		const PSG* psg () const
							{ return (const_cast <MSXModel*> (this) -> psg ()); }

		/** To get the chips that this model is made up of. \n
			Ths first time this routine is invoked, the chips are created. \n
			Never forget to include the graphical chip created with the VDP. */
		const MCHEmul::Chips& chips () const
							{ return (_chips.empty () ? (_chips = createChips ()) : _chips); }
		/** Same for the IO devices. */
		const MCHEmul::IODevices& ioDevices (const std::string& lang) const
							{ return (_ioDevices.empty () ? (_ioDevices = createIODevices (lang)) : _ioDevices); }
		/** To get the map of keys of the model. */
		const InputOSSystem::KeystrockesMap& keystrockedMap (const std::string& lang) const
							{ return (_keystrockesMap.empty () ? (_keystrockesMap = createKeystrockesMap (lang)) : _keystrockesMap); }

		/** To get the memory of the system. \n
			First time ois invoked the content memoty is created using the method memoryContent. */
		Memory* memory (unsigned int cfg, const std::string& lang);

		protected:
		/** This is pure virtual. Every model has to define its own. */
		virtual VDP* createVDP () const = 0;
		/** This is pure virtual. Every model has to define its own. */
		virtual PSG* createPSG () const = 0;
		/** The default version creates the basic chips: Video & Sound. \n
			It can be anyway overloaded to add specific chips per MSX model. */
		virtual MCHEmul::Chips createChips () const;
		/** The default version creates the basic io devices: Screen, Speakers and Keyboard. \n
			It can be overloaded anyway. */
		virtual MCHEmul::IODevices createIODevices (const std::string& lang) const;
		/** To create the specific keystrockes. */
		virtual InputOSSystem::KeystrockesMap createKeystrockesMap (const std::string& lang) const;

		// All these methods are invoked from the construction of the memory element (above)...
		/** Gets the structure of the memory. \n
			It can be overloaded, but the default basic standard structure is createde here. \n
			The basic memory structure is made up of 4 slots witch 4 subslots each and 4 different banks each.
			All banks will contain "empty RAM", except, slot 0, subslot 0, bank 0 & 1 that will contain ROM,
			and slot 0, subslot 2 that will contain RAM (the minumum that the standard accepts). \n
			Other models could add additional components. */
		virtual MCHEmul::Memory::Content memoryContent () const;
		/** Load the ROM with the right data, considering the language. \n
			If it were not possible false should be returned, and true if everythings went ok. */
		virtual bool loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs, 
			const std::string& lang) = 0;
		/** To configure the memory. */
		virtual void configureMemory (Memory* m, unsigned int cfg) = 0;

		protected:
		// Implementation
		mutable VDP* _vdp;
		mutable PSG* _psg;
		mutable MCHEmul::Chips _chips;
		mutable MCHEmul::IODevices _ioDevices;
		mutable InputOSSystem::KeystrockesMap _keystrockesMap;
		mutable Memory* _memory;
	};

	/** The first generation of MSX standard. */
	class MSX1Model : public MSXModel
	{
		public:
		static const unsigned int _CLOCKSPEED = 3580000; // 3,58MHz

		virtual Generation generation () const override
							{ return (Generation::MSX1); }

		virtual unsigned int clockSpeed () const override
							{ return (_CLOCKSPEED); }
	};

	/** The very standard model. 
		It is used when the right parameters are not passed to the program. */
	class MSXStdModel final : public MSX1Model
	{
		public:
		virtual VisualSystem visualSystem () const override
							{ return (VisualSystem::_PAL); }

		virtual std::string name () const override
							{ return ("Standard"); }

		virtual MCHEmul::Attributes attributes () const;

		private:
		virtual VDP* createVDP () const override;
		virtual PSG* createPSG () const override;
		// The standard model doesn't add any additional chip to the basic system...
		// The standard model doesn't add any additional iodevice to the basic system...

		// No need to create a memory structure different that the basic one!
		virtual bool loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs, 
			const std::string& lang) override;
		virtual void configureMemory (Memory* m, unsigned int cfg) override;
	};

	/** Spectravideo 728. */
	class SVI728 final : public MSX1Model
	{
		public:
		SVI728 (VisualSystem vs)
			: MSX1Model (),
			  _visualSystem (vs)
							{ }

		virtual VisualSystem visualSystem () const override
							{ return (_visualSystem); }

		virtual std::string name () const override
							{ return ("SVI728"); }

		virtual MCHEmul::Attributes attributes () const;

		private:
		virtual VDP* createVDP () const override;
		virtual PSG* createPSG () const override;
		virtual MCHEmul::Chips createChips () const override;
		virtual MCHEmul::IODevices createIODevices (const std::string& lang) const override;
		virtual InputOSSystem::KeystrockesMap createKeystrockesMap (const std::string& lang) const override;
		
		// No need to create a memory structure different that the basic one!
		virtual bool loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs, 
			const std::string& lang) override;
		virtual void configureMemory (Memory* m, unsigned int cfg) override;

		private:
		VisualSystem _visualSystem;
	};

	/** Spectravideo 738. */
	class SVI738 final : public MSX1Model
	{
		public:
		SVI738 (VisualSystem vs)
			: MSX1Model (),
			  _visualSystem (vs)
							{ }

		virtual VisualSystem visualSystem () const override
							{ return (_visualSystem); }

		virtual std::string name () const override
							{ return ("SVI738"); }

		virtual MCHEmul::Attributes attributes () const;

		private:
		virtual VDP* createVDP () const override;
		virtual PSG* createPSG () const override;
		virtual MCHEmul::Chips createChips () const override;
		virtual MCHEmul::IODevices createIODevices (const std::string& lang) const override;
		virtual InputOSSystem::KeystrockesMap createKeystrockesMap (const std::string& lang) const override;
	
		virtual MCHEmul::Memory::Content memoryContent () const override;
		virtual bool loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs, 
			const std::string& lang) override;
		virtual void configureMemory (Memory* m, unsigned int cfg) override;

		private:
		VisualSystem _visualSystem;
	};

	/** The second generation of MSX standard. */
	class MSX2Model : public MSXModel
	{
		public:
		static const unsigned int _CLOCKSPEED = 3580000; // 3,58MHz

		virtual Generation generation () const override
							{ return (Generation::MSX2); }

		virtual unsigned int clockSpeed () const override
							{ return (_CLOCKSPEED); }
	};
}

#endif
  
// End of the file
/*@}*/
