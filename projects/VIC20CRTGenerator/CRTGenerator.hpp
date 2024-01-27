/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: CRTGenerator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 20/01/2024 \n
 *	Description: Class to generate CRT VIC20 files.
 *	Versions: 1.0 Initial
 */

#ifndef __CRTGENERATOR__
#define __CRTGENERATOR__

#include "Parameters.hpp"
#include "CRTData.hpp"

namespace VIC20
{
	class CRTGenerator final
	{
		public:
		CRTGenerator ();

		// Errors?
		bool error () const
							{ return (_error); }
		const MCHEmul::Strings& errorNames () const
							{ return (_errorNames); }

		/** Every time this method is run,
			error and errorNames are set back to initial values.
			Return true if everything was ok and false in other case. */
		bool run (int argc, char** argv);

		bool operator ! () const
							{ return (_error); }

		private:
		inline void printOutHelp ();

		// Implementation
		inline bool existsParamTemplate (const std::string& n) const
							{ return (_templates.find (n) != _templates.end ()); }
		inline const std::shared_ptr <ParameterTemplate>& paramTemplate (const std::string& n) const
							{ return ((*_templates.find (n)).second); }
		inline std::shared_ptr <ParameterTemplate> paramTemplate (const std::string& n)
							{ return ((*_templates.find (n)).second); }
		inline bool existsParameter (const std::string& id) const
							{ return (_parameters.exists (id)); }
		inline const Parameter& parameter (const std::string& id) const
							{ return (_parameters.parameter (id)); }

		// To simplify the main code!
		void readParametersFrom (int argc, char** argv);
		CRTData createCRTStructure () const;

		private:
		ParameterTemplates _templates;
		Parameters _parameters;
		mutable bool _error;
		mutable MCHEmul::Strings _errorNames;
	};

	// ---
	inline void CRTGenerator::printOutHelp ()
	{
		std::cout << "Help:" << std::endl;
		for (auto i : _templates)
			std::cout << *(i.second) << std::endl;
	}
}

#endif
  
// End of the file
/*@}*/