/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: ParamTemplate.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 20/01/2024 \n
 *	Description: Every Parameter must belog to a Template.
 *				 That template determines the behaviour of the parameter. 
 *	Versions: 1.0 Initial
 */

#ifndef __PARAMETERTEMPLATE__
#define __PARAMETERTEMPLATE__

#include <CORE/global.hpp>
#include <fstream>

namespace VIC20
{
	/** This represents the template of a parameter. 
		It must be extended for every specific parameter. */
	class ParameterTemplate
	{
		public:
		ParameterTemplate (const std::string& id)
			: _id (id)
								{ }

		const std::string& id () const
								{ return (_id); }

		/** None by default. */
		virtual size_t numberMinValues () const
								{ return (0); }
		/** No limit by default. */
		virtual size_t numberMaxValues () const
								{ return (std::numeric_limits <size_t>::max ()); }
		/** Returns true when the string is acceptable as value of the paameter template
			and false in any other circunstance. */
		virtual bool validValue (const std::string& v) const = 0;
		/** Returns true when the list of string are acceptable as parameters. 
			By default it verifies only that every single parameter is acceptable,
			and also the number of them. However, it might be overloaded. */
		virtual bool validValues (const MCHEmul::Strings& v) const;
		/** To get a list of the default values if any. \n
			Nothing by default. */
		virtual MCHEmul::Strings defaultValues () const
								{ return (MCHEmul::Strings ({ })); }

		/** To print out to to use the parameter. */
		virtual std::ostream& printOutHelp (std::ostream& o) const = 0;

		friend std::ostream& operator << (std::ostream& o, const ParameterTemplate& pt)
								{ pt.printOutHelp (o); return (o); }

		protected:
		const std::string _id;
	};

	// To simplify the use of a list of parameter templates...
	using ParameterTemplates = std::map <std::string, std::shared_ptr <ParameterTemplate>>;

	/** A parameter for nothing. */
	class none_ParameterTemplate final : public ParameterTemplate
	{
		public:
		none_ParameterTemplate ()
			: ParameterTemplate ("none")
								{ }

		// Nothing is allowed by this parameter...
		virtual size_t numberMinValues () const override
								{ return (0); }
		virtual size_t numberMaxValues () const override
								{ return (0); }
		virtual bool validValue (const std::string& v) const
								{ return (false); }

		// ...and noting is printed out as info!
		virtual std::ostream& printOutHelp (std::ostream& o) const override
								{ return (o); }
	};

	/** i = Input parameter: \n
		The list of the files to generate the CRT image from. */
	class i_ParameterTemplate final : public ParameterTemplate
	{
		public:
		i_ParameterTemplate ()
			: ParameterTemplate ("i")
								{ }

		/** At least 1. */
		virtual size_t numberMinValues () const override
								{ return (1); }
		/** true only when is possible to open the file. */
		virtual bool validValue (const std::string& v) const override
								{ return (std::ifstream (v.c_str ()).is_open ()); }

		virtual std::ostream& printOutHelp (std::ostream& o) const override;

		protected:
		const std::string _id;
	};

	/** o = Output parameter: \n
		The name of the destination file. */
	class o_ParameterTemplate final : public ParameterTemplate
	{
		public:
		o_ParameterTemplate ()
			: ParameterTemplate ("o")
								{ }

		/** 1 and only 1. */
		virtual size_t numberMinValues () const override
								{ return (1); }
		virtual size_t numberMaxValues () const override
								{ return (1); }
		/** true only when is possible to open the file. */
		virtual bool validValue (const std::string& v) const override
								{ return (std::ofstream (v.c_str ()).is_open ()); }
		/** If no values were provided, at least one value by default. */
		virtual MCHEmul::Strings defaultValues () const
								{ return (MCHEmul::Strings ({ "CARTRIDGE.CRT20" })); }

		virtual std::ostream& printOutHelp (std::ostream& o) const override;

		protected:
		const std::string _id;
	};

	/** n = Name of the cartridge parameter: 
		For simulation purposes. A default name should be provided. */
	class n_ParameterTemplate final : public ParameterTemplate
	{
		public:
		n_ParameterTemplate ()
			: ParameterTemplate ("n")
								{ }

		/** 1 and only 1. */
		virtual size_t numberMinValues () const override
								{ return (1); }
		virtual size_t numberMaxValues () const override
								{ return (1); }
		/** true only when the value is something alphanumeric, 
			starting with a letter. */
		virtual bool validValue (const std::string& v) const override
								{ return (std::ofstream (v.c_str ()).is_open ()); }

		virtual std::ostream& printOutHelp (std::ostream& o) const override;

		protected:
		const std::string _id;
	};
}

#endif
  
// End of the file
/*@}*/