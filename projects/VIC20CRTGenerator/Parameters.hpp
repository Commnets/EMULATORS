/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: Parameters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 20/01/2024 \n
 *	Description: To manage the parameters of the command line.
 *	Versions: 1.0 Initial
 */

#ifndef __PARAMETERS__
#define __PARAMETERS__

#include <CORE/global.hpp>
#include <functional>
#include "ParamTemplate.hpp"

namespace VIC20
{
	/** This structure keeps info about a parameter defined at command line:
		It has a name and several values. */
	class Parameter final
	{
		public:
		Parameter (const std::shared_ptr <ParameterTemplate>& pt)
			  : _template (pt),
				_values ()
								{ assert (_template != nullptr); }

		const std::string& id () const
								{ return (_template -> id ()); }
		const std::shared_ptr <ParameterTemplate>& paramTemplate () const
								{ return (_template); }
		std::shared_ptr <ParameterTemplate> paramTemplate ()
								{ return (_template); }
		/** It can be changed, but used it carefully. Never nullptr. */
		void changeParamTemplate (const std::shared_ptr <ParameterTemplate>& pt)
								{ assert (pt != nullptr); _template = pt; }
		size_t size () const
								{ return (_values.size ()); }
		const MCHEmul::Strings& values () const
								{ return (_values); }
		/** true when the parameter is valid, 
			and false in any other circunstance. */
		bool addValue (const std::string& v);
		/** To check whether in the current definition status the parameter is valid. */
		bool checkConsistency () const
								{ return (_template -> id () != "none" /** can not be nothing. */ &&
										  _template -> validValues (_values) /** and accept all values. */); }

		private:
		std::shared_ptr <ParameterTemplate> _template;
		MCHEmul::Strings _values;
	};

	/** The list of parameters. */
	class Parameters final
	{
		public:
		Parameters ()
			: _parameters ({ })
								{ }

		// Managing the parameters...
		size_t size () const
								{ return (_parameters.size ()); }
		bool exists (const std::string& prm) const
							{ return (_parameters.find (prm) != _parameters.end ()); }
		const Parameter& parameter (const std::string& prm) const
							{ return (exists (prm) ? (*_parameters.find (prm)).second : _NONE); }
		void add (const Parameter& prm) // Only if thre is no anyone with the same name...
								{ if (!exists (prm.id ()))
									_parameters.insert (std::map <std::string, Parameter>::value_type (prm.id (), prm)); }
		bool iterate (const std::function <bool(const Parameter&)>& f) const;

		private:
		std::map <std::string, Parameter> _parameters;

		// Implementation
		static const Parameter _NONE;
	};
}

#endif
  
// End of the file
/*@}*/