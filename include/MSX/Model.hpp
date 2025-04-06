/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: Model.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: Type: The "real" computer this emulation is working for...There are many MSXs!
 *	Versions: 1.0 Initial
 *	Based on: https://sinclair.wiki.zxnet.co.uk/wiki/ROM_images and 
 *			  https://www.msx.org/wiki/Category:MSX_systems
 */

#ifndef __MSX_MODEL__
#define __MSX_MODEL__

#include <CORE/incs.hpp>
#include <MSX/VDP.hpp>

namespace MSX
{
	class Memory;

	/** There are many types of MSX computers and 
		with different behaviour and characteristicis. \n
		The Model is not the owner of any element that creates. */
	class MSXModel
	{
		public:
		friend Memory;

		enum class VisualSystem { _NTSC, _PAL };

		enum class Generation 
		{ 
			MSX1 = 0,	// First generation of MSX standard...
			MSX2 = 1
		};

		MSXModel ()
			: _vdp (nullptr),
			  _chips (),
			  _ioDevices (),
			  _memory (nullptr)
							{ }

		// The model is not the owner of any element of the machine, 
		// but just the machine itself. So the destructor is empty.

		virtual Generation generation () const = 0;

		virtual VisualSystem visualSystem () const = 0;

		/** Can be overloaded, but those are the ones by default. */
		virtual double screenFrequency () const
							{ return ((visualSystem () == VisualSystem::_NTSC) ? 60.03f : 50.04); }

		virtual std::string name () const = 0;

		/** To get the attributes of the model. */
		virtual MCHEmul::Attributes attributes () const = 0;

		/** To get the clock speed. */
		virtual unsigned int clockSpeed () const = 0;

		/** Related with the configurations possible. 
			By default only one configuration (the basic one) is allowed. */
		virtual bool admitConfiguration (unsigned int cfg) const
							{ return (cfg == 0); }
		virtual unsigned int basicConfiguration () const
							{ return (0); }
		unsigned int configurationAdjusted (unsigned int cfg) const
							{ return (admitConfiguration (cfg)
								? cfg : basicConfiguration ()); }

		/** To get the right used as VDP in the model. \n
			First time this method is invoked the VDP is created. \n
			But remember that the VDP doesn't own to this class but the Computer. */
		VDP* vdp () 
							{ return ((_vdp == nullptr) ? _vdp = createVDP () : _vdp); }
		const VDP* vdp () const
							{ return (const_cast <MSXModel*> (this) -> vdp ()); }

		/** To get the chips that this model is made up of. \n
			Ths first time this routine is invoked, the chips are created. */
		const MCHEmul::Chips& chips () const
							{ return (_chips.empty () ? (_chips = createChips ()) : _chips); }
		/** Same for the IO devices. */
		const MCHEmul::IODevices& ioDevices () const
							{ return (_ioDevices.empty () ? (_ioDevices = createIODevices ()) : _ioDevices); }

		/** To get the memory. \n
			This will invoke the methods below. */
		Memory* memory (unsigned int cfg, const std::string& lang);

		protected:
		virtual VDP* createVDP () const = 0;
		virtual MCHEmul::Chips createChips () const;
		virtual MCHEmul::IODevices createIODevices () const;

		// This method are invoked from the construction of the memory element (above)...
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
		mutable MCHEmul::Chips _chips;
		mutable MCHEmul::IODevices _ioDevices;
		mutable Memory* _memory;
	};

	/** The first generation of MSX standard. */
	class MSXModel1 : public MSXModel
	{
		public:
		static const unsigned int _CLOCKSPEED = 3580000000; // 3,58MHz

		virtual Generation generation () const override
							{ return (Generation::MSX1); }

		virtual unsigned int clockSpeed () const override
							{ return (_CLOCKSPEED); }
	};

	/** The very standard model. 
		It is used when the right parameters are not passed to the program. */
	class MSXStdModel final : public MSXModel1
	{
		public:
		virtual VisualSystem visualSystem () const override
							{ return (VisualSystem::_PAL); }

		virtual std::string name () const override
							{ return ("Standard"); }

		virtual MCHEmul::Attributes attributes () const;

		private:
		virtual VDP* createVDP () const override;
		// The standard model doesn't add any additional chip to the basic system...
		// The standard model doesn't add any additional iodevice to the basic system...

		// No need to create a memory structure different that the basic one!
		virtual bool loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs, 
			const std::string& lang) override;
		virtual void configureMemory (Memory* m, unsigned int cfg) override;
	};

	/** Spectravideo 728. */
	class SVI728 final : public MSXModel1
	{
		public:
		SVI728 (VisualSystem vs)
			: MSXModel1 (),
			  _visualSystem (vs)
							{ }

		virtual VisualSystem visualSystem () const override
							{ return (_visualSystem); }

		virtual std::string name () const override
							{ return ("SVI728"); }

		virtual MCHEmul::Attributes attributes () const;

		private:
		virtual VDP* createVDP () const override;
		virtual MCHEmul::Chips createChips () const override;
		virtual MCHEmul::IODevices createIODevices () const override;
		
		// No need to create a memory structure different that the basic one!
		virtual bool loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs, 
			const std::string& lang) override;
		virtual void configureMemory (Memory* m, unsigned int cfg) override;

		private:
		VisualSystem _visualSystem;
	};

	/** Spectravideo 738. */
	class SVI738 final : public MSXModel1
	{
		public:
		SVI738 (VisualSystem vs)
			: MSXModel1 (),
			  _visualSystem (vs)
							{ }

		virtual VisualSystem visualSystem () const override
							{ return (_visualSystem); }

		virtual std::string name () const override
							{ return ("SVI738"); }

		virtual MCHEmul::Attributes attributes () const;

		private:
		virtual VDP* createVDP () const override;
		virtual MCHEmul::Chips createChips () const override;
		virtual MCHEmul::IODevices createIODevices () const override;
		
		virtual MCHEmul::Memory::Content memoryContent () const override;
		virtual bool loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs, 
			const std::string& lang) override;
		virtual void configureMemory (Memory* m, unsigned int cfg) override;

		private:
		VisualSystem _visualSystem;
	};

	/** The second generation of MSX standard. */
	class MSXModel2 : public MSXModel
	{
		public:
		static const unsigned int _CLOCKSPEED = 3580000000; // 3,58MHz

		virtual Generation generation () const override
							{ return (Generation::MSX2); }

		virtual unsigned int clockSpeed () const override
							{ return (_CLOCKSPEED); }
	};
}

#endif
  
// End of the file
/*@}*/
