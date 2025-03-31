/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: MSXEmulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2024 \n
 *	Description: The root class to define any MSX emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_MSXEMULATOR__
#define __MSX_MSXEMULATOR__

#include <EMULATORS/Emulator.hpp>
#include <MSX/Model.hpp>
#include <MSX/MSX.hpp>
#include <MSX/FileIO.hpp>
#include <MSX/IOPBuilder.hpp>

namespace MSX
{
	/** The MSX emulator admits some additional parameters. */
	class MSXEmulator : public MCHEmul::Emulator
	{
		public:
		/** The possible additional parameters of the ZX81 Emulator. */
		static const unsigned char _PARAMNTSC;
		static const unsigned char _PARAMBORDER;
		static const unsigned char _PARAMCONFIGURATION;
		static const unsigned char _PARAMMACHINE;

		/**
		  * Constructor:
		  * @param argv		: The parameters in the form of Strings.\n
		  *	The basic parameters for any MSX emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default. \n
		  *	/b[COLOR]		: To draw a black box around the writable part of the screen. The color is optional. \n
		  * /w[CONF]		: To define the configuration of the machine when starting: 0 (16K RAM), 1 (32K RAM). \n
		  * /m[MACHINE]		: To define the type of ROM loaded: SVI728 (MSX1), SVI738 (MSX2). \n
		  * @param cS		: A reference to the communication system. It can be nullptr if no required.
		  */
		MSXEmulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS = nullptr)
			: MCHEmul::Emulator (args, cS),
			  _model (nullptr)
							{ }

		/** The model object belong to this class. */
		~MSXEmulator ();

		virtual void printOutParameters (std::ostream& o = std::cout) const override;

		/** To know whether the visualizacion system is or not NTSC. */
		bool NTSCSystem () const
							{ return (cmdlineArguments ().existsArgument (_PARAMNTSC)); }

		/** To know whether the border has or not to be drawn. */
		bool drawBorder () const
							{ return (cmdlineArguments ().existsArgument (_PARAMBORDER)); }
		inline unsigned int borderColor () const; // Black by default!. From 0 to 4 max. Above 4 = 4
	
		/** To know the parameter related with the type of computer configuration. \n
			The configuration selected could not be possible under a specific model. \n
			That is controlled within the Computer constructor method only. */
		bool configuration () const
							{ return (cmdlineArguments ().existsArgument (_PARAMCONFIGURATION)); }
		inline unsigned int configurationMode () const
							{ return (cmdlineArguments ().existsArgument (_PARAMCONFIGURATION)
										? cmdlineArguments ().argumentAsInt (_PARAMCONFIGURATION) : 0); }

		/** To know which is the type of computer to be emulated:
			It works like a factory method. \n
			First time it is invoked it calculates the model simulated, but next times just get it. \n
			A default model is always returned. */
		MSXModel* model () const
							{ return ((_model == nullptr) 
								? (_model = createModel ()) : _model); }

		/** To add the peripherals linked to the computer, according to the parameters. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Computer* createComputer () const override
							{ return (new MSX::MSXComputer (
								model (), configurationMode (), computerLanguage ())); }
		virtual MCHEmul::IOPeripheralBuilder* createPeripheralBuilder () const override
							{ return (new IOPeripheralBuilder); }
		virtual MCHEmul::FileIO* createFileReader () const override
							{ return (new MCHEmul::FileIO 
								(MCHEmul::FileTypeIOList (
									{ /** TODO. */ }))); }

		private:
		/** Factory method to create the right type of MSX. 
			All models are create here. Thjis is critical to run the emulator. */
		MSXModel* createModel () const;

		private:
		mutable MSXModel* _model;
	};

	// ---
	inline unsigned int MSXEmulator::borderColor () const // Black by default!...
	{ 
		int bC = drawBorder () 
			? cmdlineArguments ().argumentAsInt (_PARAMBORDER) : 0;

		return (bC > 3 ? 3 : bC); 
	}
}

#endif
  
// End of the file
/*@}*/
