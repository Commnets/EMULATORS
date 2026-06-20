# EMULATORS Framework C++ Style Reference

Use this reference when creating new framework classes or touching code where local style matters.

## File Layout

Headers usually start with:

```cpp
/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: ClassName.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: DD/MM/YYYY \n
 *	Description: Short description.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CLASSNAME__
#define __MCHEMUL_CLASSNAME__
```

Use the module group and guard prefix already used nearby:

- `CORE`: `__MCHEMUL_NAME__` or the existing local `__CORE_NAME__` variant.
- `COMMODORE`: `__COMMODORE_NAME__`.
- Other modules: match neighboring files exactly.

Headers end with:

```cpp
}

#endif
  
// End of the file
/*@}*/
```

`.cpp` files normally begin with their own header first, then extra dependencies:

```cpp
#include <CORE/ClassName.hpp>
#include <CORE/OtherDependency.hpp>
```

## Formatting Rules

- Use the repository's tab-based indentation and align continuation lines like nearby code. Do not replace leading tabs with spaces in patches, examples, or proposed snippets.
- Put a space before function-call parentheses: `initialize ()`, `size ()`.
- Put a space between template name and argument list: `std::vector <MCHEmul::UByte>`.
- Put a space in casts: `static_cast <size_t> (value)`.
- Put spaces around pointer arrows: `_memory -> initialize ()`.
- Wrap return expressions in parentheses: `return (result);`.
- Keep braces in the local style:
  - Free/member function definitions put `{` on the next line.
  - Constructors use initializer lists with members on separate aligned lines when non-trivial.
  - Short inline methods use the compact aligned body style already present in headers.
- Separate `.cpp` function definitions with:

```cpp
// ---
```

## Comments

- Include comments in proposed snippets and real code when they explain why the code exists, document hardware timing, capture a framework convention, or prevent a future misread of non-obvious logic.
- Avoid comments that only repeat the expression, assignment, or method name.
- Match nearby comment style: short `//` comments inside functions, Doxygen-style block comments for public API/member documentation, and existing explanatory multi-line comments for hardware behaviour.
- Keep comments concise and close to the code they explain.

## Naming

- Types/classes: `PascalCase`, often domain-specific: `PhysicalStorageSubset`, `VIAControlLineType2`.
- Methods/functions: lower camel case, e.g. `getInfoStructure ()`, `setActive ()`, `initializeInternalCounters ()`.
- Member variables: leading underscore, e.g. `_id`, `_activeForReading`, `_wavesPlain`.
- Constants/statics: leading underscore and uppercase where established, e.g. `_DEFAULTVALUE`, `_GETINFOBYTESLIMIT`.
- Enums often use leading underscore values: `_RAM`, `_ROM`, `_TRIANGLE`.
- Type aliases use plural domain names for collections: `using SoundWaves = std::vector <SoundWave*>;`.

## Class Structure

Prefer this order when creating a class:

1. `public:` plus friends/static constants/enums.
2. Constructors/destructor/copy and move policy.
3. Accessors and mutators.
4. Lifecycle methods such as `initialize ()`, `simulate ()`, `clock ()`.
5. `getInfoStructure () const` for `InfoClass` derivatives.
6. `protected:` hooks and helpers.
7. `private:` implementation helpers.
8. Data members, with short Doxygen comments where helpful.

Use `final` for closed concrete classes. Use `virtual ... override` when overriding. Delete default construction, copy, assignment, or move operations when the existing ownership/lifecycle pattern requires it.

## Constructors And Ownership

Initialize all members in initializer lists and keep member order consistent with declarations:

```cpp
MCHEmul::ClassName::ClassName (int id, Dependency* d)
	: MCHEmul::InfoClass ("ClassName"),
	  _id (id),
	  _dependency (d),
	  _active (false)
{ 
	assert (_dependency != nullptr);
}
```

If the class owns raw pointers, say so in the header comment and delete them in the destructor:

```cpp
MCHEmul::ClassName::~ClassName ()
{
	for (auto i : _items)
		delete (i);
}
```

Do not replace raw pointer ownership with smart pointers as a side effect unless the surrounding module already uses them or the user asks for that refactor.

## InfoClass Pattern

For `InfoClass` derivatives:

Document `getInfoStructure () const` in the `.cpp`, immediately before the method definition or in the nearby class-specific documentation block used by the file. The documentation must enumerate every field added to the returned `InfoStructure`:

- Use `FIELD = Attribute: meaning` for scalar values added with `result.add ("FIELD", ...)`.
- Use `FIELD = InfoStructure: meaning` for nested structures added with `result.add ("FIELD", std::move (...))` or equivalent.
- If the class derives from another class that already contributes fields through its own `getInfoStructure ()`, start the documentation with `The attributes and infostructures of the parent class, plus:`.
- Keep field names exactly as they appear in `result.add (...)`, preserving uppercase/local spelling.

```cpp
/**
  *	The name of the fields are: \n
  * The attributes and infostructures of the parent class, plus: \n
  * ID		= Attribute: Internal identifier. \n
  * ACTIVE	= Attribute: Whether the component is active. \n
  */
MCHEmul::InfoStructure MCHEmul::ClassName::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID",		_id);
	result.add ("ACTIVE",	_active);

	return (result);
}
```

Use uppercase field names. For nested objects, build nested `InfoStructure` values and `std::move` them where the local code does.

## Inline Methods

Keep trivial accessors inline:

```cpp
int id () const
							{ return (_id); }
bool active () const
							{ return (_active); }
void setActive (bool a)
							{ _active = a; }
```

Use the const/non-const forwarding pattern when exposing mutable access:

```cpp
const Dependency* dependency () const
							{ return (_dependency); }
Dependency* dependency ()
							{ return (const_cast <Dependency*> 
								(const_cast <const ClassName*> (this) -> dependency ())); }
```

## Error Handling And Validation

- Use `assert` for invariants that should never fail in debug builds.
- Return `false`, empty structures, or default values for normal runtime failure where the surrounding API does that.
- Use `_LOG (...)` plus `assert (false)` for unsupported internal modes when nearby code follows that pattern.
- Check boundaries explicitly for memory/address/vector operations.

## Includes

- Include what the file uses directly. Do not depend on accidental transitive includes.
- In headers, prefer forward declarations for pointer/reference-only dependencies when nearby code does.
- Keep include paths in angle brackets with repository module prefixes: `<CORE/...>`, `<COMMODORE/...>`.

## New File Checklist

When adding a new class:

- Create matching `include/<MODULE>/Name.hpp` and `src/<MODULE>/Name.cpp` unless the class is intentionally header-only.
- Add both files to the relevant `.vcxproj` and `.vcxproj.filters` when the Visual Studio project lists files explicitly.
- Add the header to the module `incs.hpp` only if neighboring code uses that aggregator for public inclusion.
- Keep changes scoped to the new behavior and avoid touching generated binaries or unrelated formatter files.

## Minimal Class Template

```cpp
namespace MCHEmul
{
	class ClassName : public InfoClass
	{
		public:
		ClassName (int id);

		int id () const
							{ return (_id); }

		virtual void initialize ();

		virtual InfoStructure getInfoStructure () const override;

		private:
		int _id;
		bool _active;
	};
}
```

```cpp
#include <CORE/ClassName.hpp>

// ---
MCHEmul::ClassName::ClassName (int id)
	: MCHEmul::InfoClass ("ClassName"),
	  _id (id),
	  _active (false)
{ 
	initialize ();
}

// ---
void MCHEmul::ClassName::initialize ()
{
	_active = false;
}

// ---
MCHEmul::InfoStructure MCHEmul::ClassName::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID",		_id);
	result.add ("ACTIVE",	_active);

	return (result);
}
```
