/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SerialPrinter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/01/2026 \n
 *	Description: To emulate the behaviour of a serial printer. \n
 *				 The way the serial printers work in the COMMODORE world are all matrix dot printers. \n
 *				 The most standard ones are declared.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SERIALPRINTERPERIPHERALS__
#define __COMMODORE_SERIALPRINTERPERIPHERALS__

#include <CORE/incs.hpp>
#include <COMMODORE/SerialIOPeripherals.hpp>
// The type of printer emulated by default...
#include <COMMODORE/SerialPrinterMPS801.hpp> 
 
namespace COMMODORE
{
	/** To emulate a serial printer.
		The name of the file created to keep the info is received as parameter, but there is a default value. \n
		The object is owner of the way the printer is emulated (see the destructor). */
	class SerialPrinterPeripheralSimulation : public SerialIOPeripheralSimulation
	{
		public:
		SerialPrinterPeripheralSimulation 
			(int id, unsigned char dN, 
			 const Definition& dt,
			 const MCHEmul::Attributes & attrs,
			 MCHEmul::MatrixPrinterEmulation* mPE = 
				new MPS801BasicMatrixPrinterEmulation ("Printer.txt"));

		virtual ~SerialPrinterPeripheralSimulation () override;

		const MCHEmul::MatrixPrinterEmulation* emulation () const
							{ return (_emulation); }
		MCHEmul::MatrixPrinterEmulation* emulation ()
							{ return (_emulation); }

		/** Open & close the output file. */
		virtual bool initialize () override
							{ return (_emulation -> initialize ()); }
		virtual bool finalize () override
							{ return (_emulation -> finalize ()); }

		/** To execute a command.
			The commands are:
			1: FLUSH, to send the content still kept in the buffer to the printer file. */
		virtual bool executeCommand (int cId, const MCHEmul::Strings& prms) override;
		/** To get the commands to manage the device, if any. */
		virtual MCHEmul::Strings commandDescriptions () const
							{ return (MCHEmul::Strings (
								{ "1:FLUSH",
								  "2:NEW PAGE" })); }

		/** The info returned is the one from the parent class, plus:
			MatrixPrinterEmulation	= InfoStructure : Information about the emulation. */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** Sending the byte to the printer emulation to treat it.
			Depends on the type of emulation linked, what it is done will be different and more or less complex. */
		virtual unsigned char sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b) override;
		/** Thes methods are invoked when secondary address is selected, 
			and teay are linked with the functions of the emulation. */
		virtual unsigned char openChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn) override;
		virtual unsigned char closeChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn) override;

		protected:
		/** The emulation used. */
		MCHEmul::MatrixPrinterEmulation* _emulation;
	};
}

#endif
  
// End of the file
/*@}*/
