/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: PortManager.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: To manage all ports in a MSX.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_PORTMANAGER__
#define __MSX_PORTMANAGER__

#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>
#include <MSX/VDP.hpp>
#include <MSX/PPI8255.hpp>
#include <MSX/PSG.hpp>

namespace MSX
{
	/** Generic Port Manager to manage blocks (groups) of ports that 
		are neither still implemented nor linked to any device. */
	class GeneralPortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 0;
		static const std::string _NAME;

		GeneralPortManager ();

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override;
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override;

		/** The initialization does nothing. */
		virtual void initialize () override
							{ /** Do nothing. */ }
	};

	/** Port Manager to manage block (groups) of ports linked with the VDP. */
	class VDPPortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 1;
		static const std::string _NAME;

		VDPPortManager ();

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (_vdp -> readRegister (id - 0x98)); }
		virtual MCHEmul::UByte peekValue (unsigned short ab, unsigned char id) const override
							{ return (_vdp -> peekRegister (id - 0x98)); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override
							{ _vdp -> setRegister (id - 0x98, v); }

		/** The initialization does nothing. */
		virtual void initialize () override
							{ /** Do nothing. */ }

		/** To link it to the VDP. \n
			They are not the owner of the port manager. */
		void linkVDP (VDP* vdp)
							{ _vdp = vdp; }

		private:
		/** The VDP. */
		VDP* _vdp;
	};

	/** Port Manager to manage block (groups) of ports linked with the PPI. */
	class PPIPortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 2;
		static const std::string _NAME;

		PPIPortManager ();

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (_ppi -> readRegister (id - 0xa8)); }
		virtual MCHEmul::UByte peekValue (unsigned short ab, unsigned char id) const override
							{ return (_ppi -> peekRegister (id - 0xa8)); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override
							{ _ppi -> setRegister (id - 0xa8, v); }

		/** The initialization does nothing. */
		virtual void initialize () override
							{ /** Do nothing. */ }

		/** To link it to the PPI. \n
			They are not the owner of the port manager. */
		void linkPPI (PPI8255* ppi)
							{ _ppi = ppi; }

		private:
		/** The PPI. */
		PPI8255* _ppi;
	};

	/** Port Manager to manage block (groups) of ports linked with the sound chip. */
	class PSGPortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 3;
		static const std::string _NAME;

		PSGPortManager ();

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (_psg -> readRegister (id - 0x98)); }
		virtual MCHEmul::UByte peekValue (unsigned short ab, unsigned char id) const override
							{ return (_psg -> peekRegister (id - 0x98)); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override
							{ _psg -> setRegister (id - 0x98, v); }

		/** The initialization does nothing. */
		virtual void initialize () override
							{ /** Do nothing. */ }

		/** To link it to the PSG. \n
			They are not the owner of the port manager. */
		void linkPSG (PSG* psg)
							{ _psg = psg; }

		private:
		/** The PSG. */
		PSG* _psg;
	};
}

#endif
  
// End of the file
/*@}*/
