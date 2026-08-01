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

## Integer Types

- Match the framework's existing use of fundamental integer types: `char`, `unsigned char`, `short`, `unsigned short`, `int`, and `unsigned int`.
- Do not introduce the fixed-width aliases from `<cstdint>`, including `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, and `uint64_t`.
- Do not introduce SDL width aliases such as `Sint8`, `Uint8`, `Sint16`, `Uint16`, `Sint32`, or `Uint32` when a fundamental integer type is sufficient. Retain semantic SDL types such as `SDL_AudioFormat` and `SDL_AudioDeviceID` when they describe an API concept instead of merely selecting an integer width.
- Use `size_t` when a value represents a size, a memory position, or an index into an array, vector, string, or another container.
- Use the nearby domain type for hardware values and counters: typically `unsigned char` for bytes, `unsigned short` for 16-bit addresses, and `unsigned int` for clock or cycle counters.
- When signed arithmetic is required for a bounded displacement or difference, use `int` if its verified range is sufficient instead of selecting a wider fixed-width type.

## Runtime Performance

- Design every proposal and implementation for maximum runtime performance while preserving observable emulation correctness.
- Treat CPU instruction execution, memory and I/O access, chip simulation, raster/cycle processing, audio sample generation, and inner protocol loops as hot paths unless measurement proves otherwise.
- Avoid allocations, container growth, copies, repeated address translation, repeated map or string lookup, redundant validation, unnecessary branches, virtual dispatch, formatting, and logging work inside hot loops.
- Reserve container capacity when the final size is known, reuse existing buffers and computed values, hoist loop-invariant work, and prefer the cheapest existing framework operation that preserves semantics.
- Do not add defensive checks to a lower-level hot path merely for general robustness when its callers already guarantee the invariant. Keep the invariant documented and use `assert` when debug-only verification is appropriate.
- Consider algorithmic complexity, data locality, branch predictability, and the number of operations performed per emulated cycle, not only source-code brevity.
- Validate meaningful hot-path changes with the available performance tests or a focused before/after measurement when practical. Do not claim an optimization without evidence when its effect is uncertain.
- When an optimized expression, lookup, bit manipulation, indexing scheme, cache, or combined operation is complex or non-obvious, place a concise comment immediately beside it explaining how it works, which invariant makes it safe, and which repeated or slower work it avoids.
- Keep straightforward optimized code uncommented when its behavior is already obvious; comments are required for complexity introduced specifically to gain runtime performance.

## Function Ownership

- Do not add free functions outside the existing `global.hpp` and `global.cpp`, including `static` file helpers and functions inside anonymous namespaces.
- Put implementation helpers on the class that owns the affected state or behavior. Make them `private` when they are internal details and `static` only when they do not need an instance.
- Do not create a utility function in an unrelated class merely to avoid a free function.
- If no existing class is a clear owner, ask the user which class should own the method before implementing it.
- Do not refactor pre-existing free functions unless the requested work explicitly includes that cleanup.

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

Define extremely short methods, consisting of one instruction or expression, directly with their declaration inside the class. Do not write the `inline` keyword in this case:

```cpp
int id () const
							{ return (_id); }
bool active () const
							{ return (_active); }
void setActive (bool a)
							{ _active = a; }
```

For a short method that needs more than one instruction or roughly a couple of lines, declare it `inline` in the class and put its definition immediately after the class definition in the same header:

```cpp
class ClassName
{
	public:
	inline void setRange (int min, int max);

	private:
	int _min;
	int _max;
};

inline void ClassName::setRange (int min, int max)
{
	_min = min;
	_max = max;
}
```

Move non-trivial methods to the `.cpp` file.

Use the const/non-const forwarding pattern when exposing mutable access:

```cpp
const Dependency* dependency () const
							{ return (_dependency); }
Dependency* dependency ()
							{ return (const_cast <Dependency*> 
								(const_cast <const ClassName*> (this) -> dependency ())); }
```

## Transient Results

Avoid a local `const` variable when its value is consumed by only one formula:

```cpp
_pendingCycles += (_halfCycle + (nC << shift)) >> 1;
```

Introduce a local `const` result when it is reused by several calculations, or when an otherwise single-use expression is long or complex enough that a descriptive name makes the formula easier to understand:

```cpp
const unsigned int elapsedHalfCycles = _halfCycle + (nC << shift);
_pendingCycles += elapsedHalfCycles >> 1;
_halfCycle = elapsedHalfCycles & 0x01;
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
