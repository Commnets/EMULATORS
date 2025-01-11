/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/03/2024 \n
 *	Description: Generic calss for all Sinclair Computers.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_COMPUTER__
#define __SINCLAIR_COMPUTER__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
	/** Just to recognize a Sinclair Computer. \n 
		In the Sinclair computers is very common to have defined in the RAM space 
		what is call the SYSTEM VARIABLES. \n
		A system variable is an address of the memory */
	class Computer : public MCHEmul::Computer
	{
		public:
		struct SystemVariable final 
		{
			enum class Type
			{
				_ADDRESS = 0,
				_1BYTEVALUE,
				_2BYTESVALUE,
				_BLOCK
			};

			// Get the info of the system variable...
			// The info can be got in many different formats!
			MCHEmul::Address valueAsAddress (const MCHEmul::Memory* m) const
							{ return (MCHEmul::Address (m -> values (_address, 2), false)); }
			MCHEmul::UByte valueAs1Byte (const MCHEmul::Memory* m) const
							{ return (m -> values (_address, 1)[0]); }
			MCHEmul::UBytes valueAs2Bytes (const MCHEmul::Memory* m) const
							{ return (m -> values (_address, 2)); }

			/** Gets a infraestructure with the following attributes. \n
				NAME		:	Attribute. The name of the variable. \n
				TYPE		:	Attribute. The type of the variable. \n
				ADDRESS		:	Attribute. The position of the memory where the variable is defined. \n
								It could appear in other mirrored locations, but just the main address is shown. \n
				VALUE		:	Attribute. The value of the variable. Deoending in the type the value
								will be one or another. \n
				BYTES		:	Attribute. Number of bytes that the variable ocupies. \n
				DESCRIPTION	:	Attribute. Brief description of the variable. */
			MCHEmul::InfoStructure getInfoStructure (const MCHEmul::Memory* m) const;

			std::string _name;
			Type _type;
			unsigned char _bytes;
			MCHEmul::Address _address;
			std::string _description;
		};

		// To simplify the definition of a map of them...
		using SystemVariables = std::map <std::string, SystemVariable>;

		Computer (MCHEmul::CPU* cpu, 
				  const MCHEmul::Chips& c, 
				  MCHEmul::Memory* m, 
				  const MCHEmul::IODevices& d,
				  const SystemVariables& sV,
				  unsigned int cs, 
				  const MCHEmul::Buses& bs,
				  const MCHEmul::Wires& ws,
				  const MCHEmul::Attributes& attrs)
			: MCHEmul::Computer (cpu, c, m, d, cs, bs, ws, attrs),
			  _systemVariables (sV)
							{ }
		
		// Managing SYSTEM Variables
		const SystemVariables& systemVariables () const
							{ return (_systemVariables); }
		void setSystemVariables (const SystemVariables& sV)
							{ _systemVariables = sV; }
		void setSystemVariablesFrom (const std::string& fN)
							{ setSystemVariables (loadSystemVariablesFrom (fN)); }
		bool existSystemVariable (const std::string& n) const
							{ return (_systemVariables.find (n) != _systemVariables.end ()); }
		const SystemVariable& systemVariable (const std::string& n) const
							{ return ((*_systemVariables.find (n)).second); }
		SystemVariable& systemVariable (const std::string& n)
							{ return ((*_systemVariables.find (n)).second); }
		MCHEmul::Address systemVariableValueAsAddress (const std::string& n) const
							{ return (systemVariable (n).valueAsAddress (memory ())); }
		MCHEmul::UByte systemVariableValueAs1Byte (const std::string& n) const
							{ return (systemVariable (n).valueAs1Byte (memory ())); }
		MCHEmul::UBytes systemVariableValueAs2Byte (const std::string& n) const
							{ return (systemVariable (n).valueAs2Bytes (memory ())); }

		/** Just return a INfoStructure witn a list of the SYSTEM VARIABLES. \n
			It could be integrated in any other to complete a more detailed picture. */
		MCHEmul::InfoStructure getSystemVariablesAsInfoStructure (const MCHEmul::Memory* m) const;

		protected:
		static SystemVariables loadSystemVariablesFrom (const std::string& fN);

		protected:
		SystemVariables _systemVariables;
	};
}

#endif
  
// End of the file
/*@}*/
