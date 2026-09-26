#include "VICIIGraphicReference.hpp"

#include <cassert>
#include <iostream>

// ---
VICIIGraphicReference::VICIIGraphicReference ()
{
}

// ---
void VICIIGraphicReference::initialize (const GraphicReferenceScenario& scenario)
{
	assert (scenario._initialVC < 1024 && scenario._initialRC < 8);
	assert (scenario._rasterLine >= 48 && scenario._rasterLine <= 247);
	assert ((scenario._bitmapReference || (scenario._initialD011 & 0x60) == 0) &&
		scenario._initialColorMode <= 2);
	for (const GraphicReferenceWrite& write : scenario._writes)
		assert (write._cycle >= 1 && write._cycle <= 63 &&
			(write._register == 0x11 || write._register == 0x16 || write._register == 0x18 ||
				(write._register >= 0x21 && write._register <= 0x23)) &&
			(write._register == 0x11 ? scenario._bitmapReference || (write._value & 0x60) == 0 :
				write._register != 0x16 || ((write._value ^ scenario._initialD016) & 0x18) == 0));

	_scenario = scenario;
	_vc = _vcbase = scenario._initialVC;
	_vlmi = _displayIndex = 0;
	_rc = scenario._initialRC;
	_d011 = scenario._initialD011;
	_fetchD011 = _drawD011 = _d011;
	_d016 = scenario._initialD016;
	_d018 = scenario._initialD018;
	_cpuNibble = scenario._initialCpuNibble;
	_memoryIncomplete = false;
	_prefetch = 4;
	_xScroll = 0;
	_idle = scenario._initialIdle;
	_multicolorPhase = false;
	_multicolorPixel = 0;
	_mainBorder = _borderState = true;
	for (unsigned short i = 0; i < 40; i++)
	{
		_matrix [i] = scenario._matrixSnapshot ? scenario._initialMatrix [i] : initialScreenCode (i);
		_colors [i] = scenario._matrixSnapshot ? scenario._initialColors [i] : stimulusInitialColor (scenario, i);
	}
	_fetched = _pipe0 = _pipe1 = _active = GraphicLatch ();
	_cycleFlags = _pendingPixelFlags = CycleFlags ();
	_pendingPixels = {};
	_colorRegisters [0] = scenario._borderColor;
	for (unsigned char index = 0; index < 3; index++)
		_colorRegisters [index + 1] = scenario._backgroundColors [index];
	_pendingColorRegister = _latchedColorRegister = 0xff;
	_pendingColorValue = _latchedColorValue = 0;
	_pixels = {};
	_fetches = {};
}

// ---
void VICIIGraphicReference::executeCycle (unsigned short cycle)
{
	assert (cycle >= 1 && cycle <= 63);
	GraphicReferenceFetch& observation = _fetches [cycle];

	// VICE performs phi1 graphics and drawing before the phi2 bad-line/matrix
	// decisions. CPU writes belong after those decisions, not before the fetch.
	if (cycle >= 16 && cycle <= 55)
		fetchGraphics (cycle, observation);
	else
		_fetched = GraphicLatch ();
	// PAL horizontal checks precede drawing in VICE. The seven/eight-dot
	// output transition is handled separately from this cycle-level latch.
	if (cycle == ((_d016 & 8) != 0 ? 17 : 18))
		_mainBorder = false;
	if (cycle == ((_d016 & 8) != 0 ? 57 : 56))
		_mainBorder = true;
	drawCycle ();

	const bool badLine = (_scenario._rasterLine & 7) == (_d011 & 7);
	if (badLine)
		_idle = false;
	if (cycle == 14)
	{
		_vc = _vcbase;
		_vlmi = 0;
		if (badLine)
			_rc = 0;
	}
	if (cycle == 58)
	{
		if (_rc == 7)
		{
			if (!_idle)
				_vcbase = _vc & 0x03ff;
			_idle = true;
		}
		if (!_idle || badLine)
		{
			_rc = (unsigned char) ((_rc + 1) & 7);
			_idle = false;
		}
	}

	// This fixture has DEN already latched at line $30 and no sprite BA source.
	observation._baLow = badLine && cycle >= 12 && cycle <= 54;
	if (observation._baLow)
	{
		if (_prefetch != 0)
			_prefetch--;
	}
	else
		_prefetch = 4;
	if (badLine && cycle >= 15 && cycle <= 54)
		fetchMatrix (observation);
	observation._vc = _vc;
	observation._vlmi = _vlmi;
	observation._prefetch = _prefetch;

	// The PAL table starts phi1 at sprite X=$194 and advances eight dots/cycle.
	// Convert that absolute coordinate to ScreenMemory's visible origin at $1f0
	// before packing the group. This is a coordinate-system conversion, not an
	// adjustment of the graphics-pipeline latency.
	_cycleFlags._valid = true;
	_cycleFlags._visible = cycle >= 15 && cycle <= 54;
	const unsigned short palX = (unsigned short)
		((0x194 + (cycle - 1) * 8) % 504);
	_cycleFlags._x = (unsigned short)
		((((palX + 8) % 504) >> 3) << 3);
	_fetchD011 = _d011;
	for (const GraphicReferenceWrite& write : _scenario._writes)
		if (write._cycle == cycle)
			applyRegisterWrite (write);
}

// ---
void VICIIGraphicReference::beginNextLine
	(unsigned short line, const std::vector <GraphicReferenceWrite>& writes)
{
	// Keep matrix/color RAM, RC, VCBASE and output latches across the boundary.
	_scenario._rasterLine = line;
	_scenario._writes = writes;
	_vc = _vcbase;
	_pixels = {};
	_fetches = {};
}

// ---
unsigned char VICIIGraphicReference::memoryByte (unsigned short address)
{
	if (address >= 0x0400 && address < 0x0800)
		return ((unsigned char) (0x80 | ((address - 0x0400) & 0x3f)));
	if (address >= 0x1000 && address < 0x1800)
	{
		const unsigned short offset = address - 0x1000;
		return ((unsigned char) (0x80 |
			(((offset >> 3) * 37 + (offset & 7) * 13 + 0x25) & 0x7f)));
	}
	return (address == 0x3fff ? 0x3c : 0);
}

// ---
unsigned char VICIIGraphicReference::colorByte (unsigned short vc)
{
	return ((unsigned char) (8 | (vc & 7)));
}

// ---
unsigned char VICIIGraphicReference::initialScreenCode (unsigned short index)
{
	return ((unsigned char) (0x20 + index));
}

// ---
unsigned char VICIIGraphicReference::initialColor (unsigned short index)
{
	return ((unsigned char) (1 + index % 7));
}

// ---
unsigned char VICIIGraphicReference::stimulusInitialColor
	(const GraphicReferenceScenario& scenario, unsigned short index)
{
	return ((unsigned char) (initialColor (index) |
		((scenario._initialColorMode == 1 ||
		  (scenario._initialColorMode == 2 && (index & 1) != 0)) ? 8 : 0)));
}

// ---
bool VICIIGraphicReference::pixelsMatch
	(const Pixels& actual, const Pixels& expected, unsigned short& firstDifferentX,
	 bool compareForeground, bool compareSourceIdentity)
{
	// Compare ScreenMemory coordinates, not VICE's delayed dbuf call number.
	// PAL X=$18..$157 maps to visible X=32..351 because the visible raster starts
	// at PAL X=$1f0. Border composition remains outside this component test.
	for (unsigned short x = 32; x < 352; x++)
		if (!actual [x]._sampled || !expected [x]._sampled ||
			(compareForeground && actual [x]._foreground != expected [x]._foreground) ||
			actual [x]._color != expected [x]._color ||
			(compareSourceIdentity &&
				 (actual [x]._screenCode != expected [x]._screenCode ||
				  actual [x]._sourceCycle != expected [x]._sourceCycle ||
				  actual [x]._graphicIndex != expected [x]._graphicIndex ||
				  actual [x]._matrixIndex != expected [x]._matrixIndex)))
		{
			firstDifferentX = x;
			return (false);
		}
	return (true);
}

// ---
bool VICIIGraphicReference::testIndependentAnchors ()
{
	bool result = true;
	for (unsigned char scroll = 0; scroll < 8; scroll++)
	{
		VICIIGraphicReference reference;
		reference.initialize ({ "independent normal anchors", 120, 200,
			3, false, 0x19, (unsigned char) (8 | scroll), {} });
		for (unsigned short cycle = 1; cycle <= 63; cycle++)
			reference.executeCycle (cycle);

		// Independent normal-line specification: PAL column zero at X=$18 maps
		// to ScreenMemory X=32, plus XSCROLL. Forty identities/colors follow.
		// This does not use either renderer's stage count or sourceCycle formula.
		const Pixels& pixels = reference.outputPixels ();
		for (unsigned short x = 32; x < 352; x++)
		{
			bool foreground = false;
			unsigned char color = 0;
			if (x >= 32 + scroll)
			{
				const unsigned short offset = x - 32 - scroll;
				const unsigned char code = (unsigned char) (0x20 + offset / 8);
				const unsigned char data = memoryByte
					((unsigned short) (0x1000 + code * 8 + 3));
				foreground = (data & (0x80 >> (offset % 8))) != 0;
				color = foreground ? (unsigned char) (1 + (offset / 8) % 7) : 0;
				result &= pixels [x]._screenCode == code;
			}
			result &= pixels [x]._sampled &&
				pixels [x]._foreground == foreground && pixels [x]._color == color;
		}

		// Check the comparison itself cannot approve an eight-dot displacement
		// or a wrong color. Mutants are never used as production expectations.
		Pixels shifted = pixels, wrongColor = pixels;
		for (unsigned short x = 40; x < 352; x++)
			shifted [x] = pixels [x - 8];
		wrongColor [32 + scroll]._color ^= 0x0f;
		unsigned short firstDifferentX = 0;
		result &= !pixelsMatch (shifted, pixels, firstDifferentX);
		result &= !pixelsMatch (wrongColor, pixels, firstDifferentX);
	}

	// These checkpoints are independently specified by the BA three-cycle
	// lead and the logged D011 writes; they do not inspect production buffers.
	const unsigned short writes [] = { 21, 31, 45 };
	for (unsigned short writeCycle : writes)
	{
		VICIIGraphicReference reference;
		reference.initialize ({ "late bad-line anchors", 155, 320, 7,
			true, 0x1c, 0x0b, { { writeCycle, 0x11, 0x1b } } });
		for (unsigned short cycle = 1; cycle <= 63; cycle++)
			reference.executeCycle (cycle);
		const Fetches& fetches = reference.fetches ();
		result &= !fetches [writeCycle]._cAccess;
		for (unsigned short cycle = writeCycle + 1; cycle <= writeCycle + 3; cycle++)
			result &= fetches [cycle]._baLow && fetches [cycle]._cAccess &&
				fetches [cycle]._invalidC;
		result &= fetches [writeCycle + 4]._cAccess &&
			!fetches [writeCycle + 4]._invalidC;
		result &= fetches [writeCycle + 5]._gScreen == 0x83;
	}
	std::cout << "PAL independent reference anchors and comparator mutants | "
		<< (result ? "OK" : "ERROR") << std::endl;
	return (result);
}

// ---
bool VICIIGraphicReference::testMulticolorAndBorderAnchors ()
{
	bool result = true;
	// $1b = 00 01 10 11: four distinct colors, two dots per pair.
	// Priority is background for 00/01 and foreground for 10/11.
	const unsigned char multicolor [] = { 2, 2, 5, 5, 7, 7, 3, 3 };
	const unsigned char hires [] = { 2, 2, 2, 3, 3, 2, 3, 3 };
	for (unsigned char characterMode = 0; characterMode < 2; characterMode++)
	{
		VICIIGraphicReference reference;
		GraphicReferenceScenario scenario
			{ "literal decoder anchors", 120, 200, 3, false, 0x19, 0x10, {} };
		scenario._backgroundColors = { { 2, 5, 7 } };
		reference.initialize (scenario);
		reference._pipe1._data = 0x1b;
		reference._pipe1._color = characterMode == 0 ? 3 : 11;
		for (unsigned char pixel = 0; pixel < 8; pixel++)
		{
			const GraphicReferencePixel output = reference.drawPixel (pixel);
			result &= output._color == (characterMode == 0 ? hires [pixel] : multicolor [pixel]);
			result &= output._foreground == (characterMode == 0
				? (0x1b & (0x80 >> pixel)) != 0 : pixel >= 4);
		}
	}
	for (unsigned char columns = 0; columns < 2; columns++)
		for (unsigned char scroll = 0; scroll < 8; scroll++)
		{
			VICIIGraphicReference reference;
			GraphicReferenceScenario scenario { "border geometry anchors", 120, 200,
				3, false, 0x19, (unsigned char) (0x10 | (columns != 0 ? 8 : 0) | scroll), {} };
			scenario._initialColorMode = 2;
			scenario._backgroundColors = { { 2, 5, 7 } };
			scenario._borderColor = 14;
			reference.initialize (scenario);
			for (unsigned short cycle = 1; cycle <= 63; cycle++)
				reference.executeCycle (cycle);
			const Pixels& pixels = reference.outputPixels ();
			for (unsigned short x = 24; x < 360; x++)
			{
				// Bauer's absolute bounds gain eight in ScreenMemory coordinates.
				const bool border = columns != 0 ? x < 32 || x >= 352 : x < 39 || x >= 343;
				result &= pixels [x]._sampled && pixels [x]._border == border;
				result &= pixels [x]._composedColor == (border ? 14 : pixels [x]._color);
			}
			Pixels mutant = pixels;
			mutant [40 + scroll]._color ^= 15;
			unsigned short firstDifferentX = 0;
			result &= !pixelsMatch (mutant, pixels, firstDifferentX);
		}
	std::cout << "PAL independent multicolor literals and 38/40-column border anchors | "
		<< (result ? "OK" : "ERROR") << std::endl;
	return (result);
}

// ---
bool VICIIGraphicReference::testColorResolutionAnchors ()
{
	bool result = true;
	// Literal 6569 ring checkpoints: the first drain still uses the old
	// register. With consecutive writes, dot zero retains the preceding value
	// while dots one through seven use the newly latched register.
	for (unsigned char reg = 0x21; reg <= 0x23; reg++)
		for (unsigned char direction = 0; direction < 2; direction++)
		{
			const unsigned char oldColor = direction == 0 ? 3 : 13;
			const unsigned char newColor = direction == 0 ? 13 : 3;
			GraphicReferenceScenario scenario { "6569 color ring literals", 120, 200,
				3, false, 0x19, 8, {} };
			scenario._backgroundColors = { { oldColor, oldColor, oldColor } };
			scenario._borderColor = 14;
			VICIIGraphicReference reference;
			reference.initialize (scenario);
			std::array <GraphicReferencePixel, 8> rendered {};
			for (GraphicReferencePixel& pixel : rendered)
			{
				pixel._sampled = true;
				pixel._colorToken = pixel._composedColorToken = reg;
			}
			// A literal color cannot change with D021..D023. Border resolution
			// remains independent of the underlying background at the same dot.
			rendered [2]._colorToken = rendered [2]._composedColorToken = 5;
			rendered [4]._border = true;
			rendered [4]._composedColorToken = 0x20;
			reference._cycleFlags._valid = true;
			reference._cycleFlags._x = 160;
			reference.resolveColors (rendered);
			reference.applyRegisterWrite ({ 34, reg, newColor });
			const unsigned char expected [4][8] = {
				{ oldColor, oldColor, 5, oldColor, oldColor, oldColor, oldColor, oldColor },
				{ oldColor, newColor, 5, newColor, newColor, newColor, newColor, newColor },
				{ newColor, 1, 5, 1, 1, 1, 1, 1 },
				{ 1, 1, 5, 1, 1, 1, 1, 1 }
			};
			for (unsigned char drain = 0; drain < 4; drain++)
			{
				reference._cycleFlags._x = (unsigned short) (168 + drain * 8);
				reference.resolveColors (rendered);
				for (unsigned char dot = 0; dot < 8; dot++)
				{
					const GraphicReferencePixel& pixel = reference.outputPixels () [160 + drain * 8 + dot];
					result &= pixel._sampled && pixel._color == expected [drain][dot] &&
						pixel._composedColor == (dot == 4 ? 14 : expected [drain][dot]);
				}
				if (drain == 0)
					reference.applyRegisterWrite ({ 35, reg, 1 });
			}
		}
	std::cout << "PAL 6569 deferred color ring anchors | "
		<< (result ? "OK" : "ERROR") << std::endl;
	return (result);
}

// ---
bool VICIIGraphicReference::testBitmapAndInvalidModeAnchors ()
{
	bool result = true;
	for (unsigned char invalid = 0; invalid < 2; invalid++)
	{
		VICIIGraphicReference reference;
		GraphicReferenceScenario scenario { "bitmap literal anchors", 120, 200,
			3, false, (unsigned char) (invalid != 0 ? 0x79 : 0x39), 0x18, {} };
		scenario._bitmapReference = true;
		scenario._backgroundColors = { { 2, 5, 7 } };
		reference.initialize (scenario);
		reference._pipe1._data = 0x1b;
		reference._pipe1._screenCode = 0x56;
		reference._pipe1._color = 11;
		const unsigned char colors [] = { 2, 2, 5, 5, 6, 6, 11, 11 };
		for (unsigned char pixel = 0; pixel < 8; pixel++)
		{
			const GraphicReferencePixel output = reference.drawPixel (pixel);
			result &= output._color == (invalid != 0 ? 0 : colors [pixel]);
			result &= output._foreground == (pixel >= 4);
		}
		GraphicReferenceFetch fetch;
		reference.fetchGraphics (16, fetch);
		// VC=200, RC=3: bitmap $0643; ECM forces address bits 9/10 low ($0043).
		result &= fetch._gAddress == (invalid != 0 ? 0x0043 : 0x0643);
	}
	std::cout << "PAL independent bitmap/invalid-mode literals and address anchors | "
		<< (result ? "OK" : "ERROR") << std::endl;
	return (result);
}

// ---
GraphicReplayFixture VICIIGraphicReference::demoLineReplay (bool demoB)
{
	// Captured bus values are regression checkpoints, never a hardware oracle.
	GraphicReplayFixture fixture;
	if (!demoB)
	{
		fixture._scenario = { "LetsScrollitA captured line replay", 52, 40,
			7, true, 29, 23, {} };
		fixture._scenario._bank = 1;
		fixture._scenario._initialD018 = 238;
		fixture._scenario._initialCpuNibble = 13;
		fixture._scenario._capturedMemory = fixture._scenario._matrixSnapshot = true;
		fixture._capturedBackgrounds = false;
		fixture._scenario._backgroundColors = { { 0, 0, 0 } };
		fixture._scenario._initialMatrix = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
		fixture._scenario._initialColors = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0 } };
		fixture._scenario._memory = {
			{ 30720, 0 },
			{ 30727, 0 },
			{ 30763, 0 },
			{ 30764, 0 },
			{ 30765, 0 },
			{ 30766, 0 },
			{ 30767, 0 },
			{ 30768, 0 },
			{ 30769, 0 },
			{ 30770, 0 },
			{ 30771, 0 },
			{ 30772, 0 },
			{ 30773, 0 },
			{ 30774, 0 },
			{ 30775, 0 },
			{ 30776, 0 },
			{ 30777, 0 },
			{ 30778, 0 },
			{ 30779, 0 },
			{ 30780, 0 },
			{ 30781, 0 },
			{ 30782, 0 },
			{ 32760, 0 },
			{ 32767, 0 },
		};
		fixture._scenario._colorMemory = {
			{ 43, 3 },
			{ 44, 3 },
			{ 45, 3 },
			{ 46, 3 },
			{ 47, 3 },
			{ 48, 3 },
			{ 49, 3 },
			{ 50, 3 },
			{ 51, 3 },
			{ 52, 3 },
			{ 53, 3 },
			{ 54, 3 },
			{ 55, 3 },
			{ 56, 3 },
			{ 57, 3 },
			{ 58, 3 },
			{ 59, 3 },
			{ 60, 3 },
			{ 61, 0 },
			{ 62, 0 },
		};
		fixture._cycles = {
			{ 62135898, 52, 1, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135899, 52, 2, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135900, 52, 3, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, { { 3, 17, 30 } } },
			{ 62135901, 52, 4, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135902, 52, 5, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135903, 52, 6, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135904, 52, 7, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135905, 52, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135906, 52, 9, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135907, 52, 10, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135908, 52, 11, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135909, 52, 12, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135910, 52, 13, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135911, 52, 14, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135912, 52, 15, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135913, 52, 16, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135914, 52, 17, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135915, 52, 18, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135916, 52, 19, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135917, 52, 20, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135918, 52, 21, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135919, 52, 22, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135920, 52, 23, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135921, 52, 24, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135922, 52, 25, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135923, 52, 26, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135924, 52, 27, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135925, 52, 28, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135926, 52, 29, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135927, 52, 30, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135928, 52, 31, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135929, 52, 32, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135930, 52, 33, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135931, 52, 34, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135932, 52, 35, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135933, 52, 36, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135934, 52, 37, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135935, 52, 38, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135936, 52, 39, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135937, 52, 40, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135938, 52, 41, 8, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135939, 52, 42, 8, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135940, 52, 43, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135941, 52, 44, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135942, 52, 45, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135943, 52, 46, 0, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135944, 52, 47, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135945, 52, 48, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135946, 52, 49, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135947, 52, 50, 10, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135948, 52, 51, 14, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135949, 52, 52, 14, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135950, 52, 53, 14, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135951, 52, 54, 14, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135952, 52, 55, 14, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135953, 52, 56, 14, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135954, 52, 57, 14, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135955, 52, 58, 14, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135956, 52, 59, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135957, 52, 60, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135958, 52, 61, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135959, 52, 62, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135960, 52, 63, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135961, 53, 1, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135962, 53, 2, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135963, 53, 3, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135964, 53, 4, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135965, 53, 5, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135966, 53, 6, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135967, 53, 7, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135968, 53, 8, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135969, 53, 9, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135970, 53, 10, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135971, 53, 11, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135972, 53, 12, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135973, 53, 13, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135974, 53, 14, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135975, 53, 15, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135976, 53, 16, 2, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135977, 53, 17, 6, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135978, 53, 18, 6, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135979, 53, 19, 6, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135980, 53, 20, 8, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135981, 53, 21, 8, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135982, 53, 22, 9, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135983, 53, 23, 9, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135984, 53, 24, 9, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135985, 53, 25, 9, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135986, 53, 26, 9, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135987, 53, 27, 9, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135988, 53, 28, 13, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135989, 53, 29, 13, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135990, 53, 30, 13, true, false, 32767, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62135991, 53, 31, 13, true, false, 32767, 0, 0, 0, 0, 0, 0, false, { { 31, 17, 29 } } },
			{ 62135992, 53, 32, 3, true, true, 32767, 0, 0, 0, 0, 255, 3, true, {  } },
			{ 62135993, 53, 33, 3, true, true, 32767, 0, 255, 3, 1, 255, 3, true, {  } },
			{ 62135994, 53, 34, 3, true, true, 32767, 0, 255, 3, 2, 255, 3, true, {  } },
			{ 62135995, 53, 35, 3, true, true, 32767, 0, 255, 3, 3, 0, 3, false, {  } },
			{ 62135996, 53, 36, 3, true, true, 30727, 0, 0, 3, 4, 0, 3, false, {  } },
			{ 62135997, 53, 37, 3, true, true, 30727, 0, 0, 3, 5, 0, 3, false, {  } },
			{ 62135998, 53, 38, 3, true, true, 30727, 0, 0, 3, 6, 0, 3, false, {  } },
			{ 62135999, 53, 39, 3, true, true, 30727, 0, 0, 3, 7, 0, 3, false, {  } },
			{ 62136000, 53, 40, 3, true, true, 30727, 0, 0, 3, 8, 0, 3, false, {  } },
			{ 62136001, 53, 41, 3, true, true, 30727, 0, 0, 3, 9, 0, 3, false, {  } },
			{ 62136002, 53, 42, 3, true, true, 30727, 0, 0, 3, 10, 0, 3, false, {  } },
			{ 62136003, 53, 43, 3, true, true, 30727, 0, 0, 3, 11, 0, 3, false, {  } },
			{ 62136004, 53, 44, 3, true, true, 30727, 0, 0, 3, 12, 0, 3, false, {  } },
			{ 62136005, 53, 45, 3, true, true, 30727, 0, 0, 3, 13, 0, 3, false, {  } },
			{ 62136006, 53, 46, 3, true, true, 30727, 0, 0, 3, 14, 0, 3, false, {  } },
			{ 62136007, 53, 47, 3, true, true, 30727, 0, 0, 3, 15, 0, 3, false, {  } },
			{ 62136008, 53, 48, 3, true, true, 30727, 0, 0, 3, 16, 0, 3, false, {  } },
			{ 62136009, 53, 49, 3, true, true, 30727, 0, 0, 3, 17, 0, 3, false, {  } },
			{ 62136010, 53, 50, 3, true, true, 30727, 0, 0, 3, 18, 0, 3, false, {  } },
			{ 62136011, 53, 51, 3, true, true, 30727, 0, 0, 3, 19, 0, 3, false, {  } },
			{ 62136012, 53, 52, 3, true, true, 30727, 0, 0, 3, 20, 0, 3, false, {  } },
			{ 62136013, 53, 53, 3, true, true, 30727, 0, 0, 3, 21, 0, 0, false, {  } },
			{ 62136014, 53, 54, 3, true, true, 30727, 0, 0, 0, 22, 0, 0, false, {  } },
			{ 62136015, 53, 55, 3, true, false, 30727, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136016, 53, 56, 3, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136017, 53, 57, 3, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136018, 53, 58, 3, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136019, 53, 59, 3, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136020, 53, 60, 3, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136021, 53, 61, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136022, 53, 62, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136023, 53, 63, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136024, 54, 1, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136025, 54, 2, 8, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136026, 54, 3, 8, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136027, 54, 4, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136028, 54, 5, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136029, 54, 6, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136030, 54, 7, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136031, 54, 8, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136032, 54, 9, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136033, 54, 10, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136034, 54, 11, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136035, 54, 12, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136036, 54, 13, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, { { 13, 17, 31 } } },
			{ 62136037, 54, 14, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136038, 54, 15, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136039, 54, 16, 10, true, false, 32760, 0, 255, 3, 0, 0, 0, false, {  } },
			{ 62136040, 54, 17, 10, true, false, 32760, 0, 255, 3, 0, 0, 0, false, {  } },
			{ 62136041, 54, 18, 0, true, false, 32760, 0, 255, 3, 0, 0, 0, false, {  } },
			{ 62136042, 54, 19, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136043, 54, 20, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136044, 54, 21, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136045, 54, 22, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136046, 54, 23, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136047, 54, 24, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136048, 54, 25, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136049, 54, 26, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136050, 54, 27, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136051, 54, 28, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136052, 54, 29, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136053, 54, 30, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136054, 54, 31, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136055, 54, 32, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136056, 54, 33, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136057, 54, 34, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136058, 54, 35, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136059, 54, 36, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136060, 54, 37, 10, true, false, 30720, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136061, 54, 38, 0, true, false, 30720, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136062, 54, 39, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136063, 54, 40, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136064, 54, 41, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136065, 54, 42, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136066, 54, 43, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136067, 54, 44, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136068, 54, 45, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136069, 54, 46, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136070, 54, 47, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136071, 54, 48, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136072, 54, 49, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136073, 54, 50, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136074, 54, 51, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136075, 54, 52, 10, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136076, 54, 53, 0, true, false, 30720, 0, 0, 3, 0, 0, 0, false, {  } },
			{ 62136077, 54, 54, 0, true, false, 30720, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136078, 54, 55, 0, true, false, 30720, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136079, 54, 56, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136080, 54, 57, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136081, 54, 58, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136082, 54, 59, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136083, 54, 60, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136084, 54, 61, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136085, 54, 62, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 62136086, 54, 63, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
		};
	}
	else
	{
		fixture._scenario = { "LetsScrollitB captured line replay", 147, 280,
			7, true, 27, 19, {} };
		fixture._scenario._bank = 0;
		fixture._scenario._initialD018 = 28;
		fixture._scenario._initialCpuNibble = 10;
		fixture._scenario._capturedMemory = fixture._scenario._matrixSnapshot = true;
		fixture._capturedBackgrounds = true;
		fixture._scenario._backgroundColors = { { 3, 6, 14 } };
		fixture._scenario._initialMatrix = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
		fixture._scenario._initialColors = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
		fixture._scenario._memory = {
			{ 1304, 32 },
			{ 1305, 32 },
			{ 1306, 32 },
			{ 1307, 32 },
			{ 1308, 32 },
			{ 1309, 32 },
			{ 1310, 32 },
			{ 1311, 32 },
			{ 1312, 32 },
			{ 1313, 32 },
			{ 1314, 32 },
			{ 1315, 32 },
			{ 1316, 32 },
			{ 1317, 32 },
			{ 1318, 32 },
			{ 1319, 32 },
			{ 1320, 32 },
			{ 1321, 32 },
			{ 1322, 32 },
			{ 1323, 32 },
			{ 1324, 32 },
			{ 1325, 32 },
			{ 1326, 32 },
			{ 1327, 32 },
			{ 1328, 32 },
			{ 1329, 32 },
			{ 1330, 32 },
			{ 1331, 32 },
			{ 1332, 32 },
			{ 1333, 32 },
			{ 1334, 32 },
			{ 1335, 32 },
			{ 1336, 32 },
			{ 1337, 32 },
			{ 1338, 32 },
			{ 1339, 32 },
			{ 1340, 32 },
			{ 1341, 32 },
			{ 1342, 32 },
			{ 1343, 32 },
			{ 1347, 18 },
			{ 1348, 3 },
			{ 1349, 8 },
			{ 1350, 9 },
			{ 1351, 14 },
			{ 1352, 7 },
			{ 1353, 32 },
			{ 1354, 6 },
			{ 1355, 15 },
			{ 1356, 18 },
			{ 1357, 32 },
			{ 1358, 12 },
			{ 1359, 42 },
			{ 1360, 32 },
			{ 1361, 32 },
			{ 1362, 32 },
			{ 1363, 32 },
			{ 1364, 32 },
			{ 1365, 32 },
			{ 1366, 32 },
			{ 1367, 32 },
			{ 1368, 32 },
			{ 1369, 32 },
			{ 1370, 32 },
			{ 1371, 32 },
			{ 1372, 32 },
			{ 1373, 32 },
			{ 1374, 32 },
			{ 1375, 32 },
			{ 1376, 32 },
			{ 12319, 0 },
			{ 12343, 0 },
			{ 12351, 15 },
			{ 12359, 207 },
			{ 12367, 192 },
			{ 12391, 0 },
			{ 12407, 15 },
			{ 12415, 192 },
			{ 12439, 192 },
			{ 12544, 1 },
			{ 12545, 85 },
			{ 12546, 0 },
			{ 12547, 5 },
			{ 12548, 85 },
			{ 12549, 64 },
			{ 12550, 0 },
			{ 12551, 0 },
			{ 12631, 0 },
			{ 14335, 213 },
			{ 14360, 0 },
			{ 14384, 0 },
			{ 14392, 5 },
			{ 14400, 69 },
			{ 14408, 64 },
			{ 14432, 1 },
			{ 14448, 5 },
			{ 14456, 64 },
			{ 14480, 64 },
			{ 14592, 0 },
			{ 14672, 0 },
			{ 16376, 0 },
			{ 16383, 0 },
		};
		fixture._scenario._colorMemory = {
			{ 280, 8 },
			{ 281, 8 },
			{ 282, 8 },
			{ 283, 8 },
			{ 284, 8 },
			{ 285, 8 },
			{ 286, 8 },
			{ 287, 8 },
			{ 288, 8 },
			{ 289, 8 },
			{ 290, 8 },
			{ 291, 8 },
			{ 292, 8 },
			{ 293, 8 },
			{ 294, 8 },
			{ 295, 8 },
			{ 296, 8 },
			{ 297, 8 },
			{ 298, 8 },
			{ 299, 8 },
			{ 300, 8 },
			{ 301, 8 },
			{ 302, 8 },
			{ 303, 8 },
			{ 304, 8 },
			{ 305, 8 },
			{ 306, 8 },
			{ 307, 8 },
			{ 308, 8 },
			{ 309, 8 },
			{ 310, 8 },
			{ 311, 8 },
			{ 312, 8 },
			{ 313, 8 },
			{ 314, 8 },
			{ 315, 8 },
			{ 316, 8 },
			{ 317, 8 },
			{ 318, 8 },
			{ 319, 8 },
			{ 323, 8 },
			{ 324, 8 },
			{ 325, 8 },
			{ 326, 8 },
			{ 327, 8 },
			{ 328, 8 },
			{ 329, 8 },
			{ 330, 8 },
			{ 331, 8 },
			{ 332, 8 },
			{ 333, 8 },
			{ 334, 8 },
			{ 335, 8 },
			{ 336, 8 },
			{ 337, 8 },
			{ 338, 8 },
			{ 339, 8 },
			{ 340, 8 },
			{ 341, 8 },
			{ 342, 8 },
			{ 343, 8 },
			{ 344, 8 },
			{ 345, 8 },
			{ 346, 8 },
			{ 347, 8 },
			{ 348, 8 },
			{ 349, 8 },
			{ 350, 8 },
			{ 351, 8 },
			{ 352, 8 },
		};
		fixture._cycles = {
			{ 31812675, 147, 1, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812676, 147, 2, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812677, 147, 3, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812678, 147, 4, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812679, 147, 5, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812680, 147, 6, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812681, 147, 7, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812682, 147, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812683, 147, 9, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812684, 147, 10, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812685, 147, 11, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812686, 147, 12, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812687, 147, 13, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812688, 147, 14, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812689, 147, 15, 10, false, true, 0, 0, 0, 0, 0, 32, 8, false, {  } },
			{ 31812690, 147, 16, 10, true, true, 12544, 1, 32, 8, 1, 32, 8, false, {  } },
			{ 31812691, 147, 17, 10, true, true, 12544, 1, 32, 8, 2, 32, 8, false, {  } },
			{ 31812692, 147, 18, 10, true, true, 12544, 1, 32, 8, 3, 32, 8, false, {  } },
			{ 31812693, 147, 19, 10, true, true, 12544, 1, 32, 8, 4, 32, 8, false, {  } },
			{ 31812694, 147, 20, 10, true, true, 12544, 1, 32, 8, 5, 32, 8, false, {  } },
			{ 31812695, 147, 21, 10, true, true, 12544, 1, 32, 8, 6, 32, 8, false, {  } },
			{ 31812696, 147, 22, 10, true, true, 12544, 1, 32, 8, 7, 32, 8, false, {  } },
			{ 31812697, 147, 23, 10, true, true, 12544, 1, 32, 8, 8, 32, 8, false, {  } },
			{ 31812698, 147, 24, 10, true, true, 12544, 1, 32, 8, 9, 32, 8, false, {  } },
			{ 31812699, 147, 25, 10, true, true, 12544, 1, 32, 8, 10, 32, 8, false, {  } },
			{ 31812700, 147, 26, 10, true, true, 12544, 1, 32, 8, 11, 32, 8, false, {  } },
			{ 31812701, 147, 27, 10, true, true, 12544, 1, 32, 8, 12, 32, 8, false, {  } },
			{ 31812702, 147, 28, 10, true, true, 12544, 1, 32, 8, 13, 32, 8, false, {  } },
			{ 31812703, 147, 29, 10, true, true, 12544, 1, 32, 8, 14, 32, 8, false, {  } },
			{ 31812704, 147, 30, 10, true, true, 12544, 1, 32, 8, 15, 32, 8, false, {  } },
			{ 31812705, 147, 31, 10, true, true, 12544, 1, 32, 8, 16, 32, 8, false, {  } },
			{ 31812706, 147, 32, 10, true, true, 12544, 1, 32, 8, 17, 32, 8, false, {  } },
			{ 31812707, 147, 33, 10, true, true, 12544, 1, 32, 8, 18, 32, 8, false, {  } },
			{ 31812708, 147, 34, 10, true, true, 12544, 1, 32, 8, 19, 32, 8, false, {  } },
			{ 31812709, 147, 35, 10, true, true, 12544, 1, 32, 8, 20, 32, 8, false, {  } },
			{ 31812710, 147, 36, 10, true, true, 12544, 1, 32, 8, 21, 32, 8, false, {  } },
			{ 31812711, 147, 37, 10, true, true, 12544, 1, 32, 8, 22, 32, 8, false, {  } },
			{ 31812712, 147, 38, 10, true, true, 12544, 1, 32, 8, 23, 32, 8, false, {  } },
			{ 31812713, 147, 39, 10, true, true, 12544, 1, 32, 8, 24, 32, 8, false, {  } },
			{ 31812714, 147, 40, 10, true, true, 12544, 1, 32, 8, 25, 32, 8, false, {  } },
			{ 31812715, 147, 41, 10, true, true, 12544, 1, 32, 8, 26, 32, 8, false, {  } },
			{ 31812716, 147, 42, 10, true, true, 12544, 1, 32, 8, 27, 32, 8, false, {  } },
			{ 31812717, 147, 43, 10, true, true, 12544, 1, 32, 8, 28, 32, 8, false, {  } },
			{ 31812718, 147, 44, 10, true, true, 12544, 1, 32, 8, 29, 32, 8, false, {  } },
			{ 31812719, 147, 45, 10, true, true, 12544, 1, 32, 8, 30, 32, 8, false, {  } },
			{ 31812720, 147, 46, 10, true, true, 12544, 1, 32, 8, 31, 32, 8, false, {  } },
			{ 31812721, 147, 47, 10, true, true, 12544, 1, 32, 8, 32, 32, 8, false, {  } },
			{ 31812722, 147, 48, 10, true, true, 12544, 1, 32, 8, 33, 32, 8, false, {  } },
			{ 31812723, 147, 49, 10, true, true, 12544, 1, 32, 8, 34, 32, 8, false, {  } },
			{ 31812724, 147, 50, 10, true, true, 12544, 1, 32, 8, 35, 32, 8, false, {  } },
			{ 31812725, 147, 51, 10, true, true, 12544, 1, 32, 8, 36, 32, 8, false, {  } },
			{ 31812726, 147, 52, 10, true, true, 12544, 1, 32, 8, 37, 32, 8, false, {  } },
			{ 31812727, 147, 53, 10, true, true, 12544, 1, 32, 8, 38, 32, 8, false, {  } },
			{ 31812728, 147, 54, 10, true, true, 12544, 1, 32, 8, 39, 32, 8, false, {  } },
			{ 31812729, 147, 55, 10, true, false, 12544, 1, 32, 8, 0, 0, 0, false, {  } },
			{ 31812730, 147, 56, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812731, 147, 57, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812732, 147, 58, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812733, 147, 59, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812734, 147, 60, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812735, 147, 61, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812736, 147, 62, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812737, 147, 63, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812738, 148, 1, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812739, 148, 2, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812740, 148, 3, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812741, 148, 4, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812742, 148, 5, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812743, 148, 6, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812744, 148, 7, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812745, 148, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812746, 148, 9, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812747, 148, 10, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812748, 148, 11, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812749, 148, 12, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812750, 148, 13, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812751, 148, 14, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812752, 148, 15, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812753, 148, 16, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812754, 148, 17, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812755, 148, 18, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812756, 148, 19, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812757, 148, 20, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812758, 148, 21, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812759, 148, 22, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812760, 148, 23, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812761, 148, 24, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812762, 148, 25, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812763, 148, 26, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812764, 148, 27, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812765, 148, 28, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812766, 148, 29, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812767, 148, 30, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812768, 148, 31, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812769, 148, 32, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812770, 148, 33, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812771, 148, 34, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812772, 148, 35, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812773, 148, 36, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812774, 148, 37, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812775, 148, 38, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812776, 148, 39, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812777, 148, 40, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812778, 148, 41, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812779, 148, 42, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812780, 148, 43, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812781, 148, 44, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812782, 148, 45, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812783, 148, 46, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812784, 148, 47, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812785, 148, 48, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812786, 148, 49, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812787, 148, 50, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812788, 148, 51, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812789, 148, 52, 10, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812790, 148, 53, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812791, 148, 54, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812792, 148, 55, 0, true, false, 12545, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812793, 148, 56, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812794, 148, 57, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812795, 148, 58, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812796, 148, 59, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812797, 148, 60, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812798, 148, 61, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812799, 148, 62, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812800, 148, 63, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812801, 149, 1, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812802, 149, 2, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812803, 149, 3, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812804, 149, 4, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812805, 149, 5, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812806, 149, 6, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812807, 149, 7, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812808, 149, 8, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812809, 149, 9, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812810, 149, 10, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812811, 149, 11, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812812, 149, 12, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812813, 149, 13, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812814, 149, 14, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812815, 149, 15, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812816, 149, 16, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812817, 149, 17, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812818, 149, 18, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812819, 149, 19, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812820, 149, 20, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812821, 149, 21, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812822, 149, 22, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812823, 149, 23, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812824, 149, 24, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812825, 149, 25, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812826, 149, 26, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812827, 149, 27, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812828, 149, 28, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812829, 149, 29, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812830, 149, 30, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812831, 149, 31, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812832, 149, 32, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812833, 149, 33, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812834, 149, 34, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812835, 149, 35, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812836, 149, 36, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812837, 149, 37, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812838, 149, 38, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812839, 149, 39, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812840, 149, 40, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812841, 149, 41, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812842, 149, 42, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812843, 149, 43, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812844, 149, 44, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812845, 149, 45, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812846, 149, 46, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812847, 149, 47, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812848, 149, 48, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812849, 149, 49, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812850, 149, 50, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812851, 149, 51, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812852, 149, 52, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812853, 149, 53, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812854, 149, 54, 10, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812855, 149, 55, 0, true, false, 12546, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31812856, 149, 56, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812857, 149, 57, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812858, 149, 58, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812859, 149, 59, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812860, 149, 60, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812861, 149, 61, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812862, 149, 62, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812863, 149, 63, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812864, 150, 1, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812865, 150, 2, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812866, 150, 3, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812867, 150, 4, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812868, 150, 5, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812869, 150, 6, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812870, 150, 7, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812871, 150, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812872, 150, 9, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812873, 150, 10, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812874, 150, 11, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812875, 150, 12, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812876, 150, 13, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812877, 150, 14, 6, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812878, 150, 15, 6, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812879, 150, 16, 6, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812880, 150, 17, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812881, 150, 18, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812882, 150, 19, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812883, 150, 20, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812884, 150, 21, 9, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812885, 150, 22, 9, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812886, 150, 23, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812887, 150, 24, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812888, 150, 25, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812889, 150, 26, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, { { 26, 17, 28 } } },
			{ 31812890, 150, 27, 9, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812891, 150, 28, 9, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812892, 150, 29, 9, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812893, 150, 30, 9, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812894, 150, 31, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812895, 150, 32, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812896, 150, 33, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812897, 150, 34, 13, true, false, 12547, 5, 32, 8, 0, 0, 0, false, { { 34, 33, 13 } } },
			{ 31812898, 150, 35, 2, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812899, 150, 36, 2, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812900, 150, 37, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812901, 150, 38, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812902, 150, 39, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812903, 150, 40, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812904, 150, 41, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812905, 150, 42, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812906, 150, 43, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812907, 150, 44, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812908, 150, 45, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812909, 150, 46, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812910, 150, 47, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812911, 150, 48, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812912, 150, 49, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812913, 150, 50, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812914, 150, 51, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812915, 150, 52, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812916, 150, 53, 10, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812917, 150, 54, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812918, 150, 55, 0, true, false, 12547, 5, 32, 8, 0, 0, 0, false, {  } },
			{ 31812919, 150, 56, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812920, 150, 57, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812921, 150, 58, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812922, 150, 59, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812923, 150, 60, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812924, 150, 61, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812925, 150, 62, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812926, 150, 63, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812927, 151, 1, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812928, 151, 2, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812929, 151, 3, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812930, 151, 4, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812931, 151, 5, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812932, 151, 6, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812933, 151, 7, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812934, 151, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812935, 151, 9, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812936, 151, 10, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812937, 151, 11, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812938, 151, 12, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812939, 151, 13, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812940, 151, 14, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812941, 151, 15, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812942, 151, 16, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812943, 151, 17, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812944, 151, 18, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812945, 151, 19, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812946, 151, 20, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812947, 151, 21, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812948, 151, 22, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812949, 151, 23, 9, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812950, 151, 24, 9, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812951, 151, 25, 9, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812952, 151, 26, 9, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812953, 151, 27, 13, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812954, 151, 28, 13, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812955, 151, 29, 13, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812956, 151, 30, 13, true, false, 12548, 85, 32, 8, 0, 0, 0, false, { { 30, 33, 14 } } },
			{ 31812957, 151, 31, 2, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812958, 151, 32, 2, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812959, 151, 33, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812960, 151, 34, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812961, 151, 35, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812962, 151, 36, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812963, 151, 37, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812964, 151, 38, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812965, 151, 39, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812966, 151, 40, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812967, 151, 41, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812968, 151, 42, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812969, 151, 43, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812970, 151, 44, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812971, 151, 45, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812972, 151, 46, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812973, 151, 47, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812974, 151, 48, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812975, 151, 49, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812976, 151, 50, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812977, 151, 51, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812978, 151, 52, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812979, 151, 53, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812980, 151, 54, 10, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812981, 151, 55, 0, true, false, 12548, 85, 32, 8, 0, 0, 0, false, {  } },
			{ 31812982, 151, 56, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812983, 151, 57, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812984, 151, 58, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812985, 151, 59, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812986, 151, 60, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812987, 151, 61, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812988, 151, 62, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812989, 151, 63, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812990, 152, 1, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812991, 152, 2, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812992, 152, 3, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812993, 152, 4, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812994, 152, 5, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812995, 152, 6, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812996, 152, 7, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812997, 152, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812998, 152, 9, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31812999, 152, 10, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813000, 152, 11, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813001, 152, 12, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813002, 152, 13, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813003, 152, 14, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813004, 152, 15, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813005, 152, 16, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813006, 152, 17, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813007, 152, 18, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813008, 152, 19, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813009, 152, 20, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813010, 152, 21, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813011, 152, 22, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813012, 152, 23, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813013, 152, 24, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813014, 152, 25, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813015, 152, 26, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813016, 152, 27, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813017, 152, 28, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813018, 152, 29, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813019, 152, 30, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813020, 152, 31, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813021, 152, 32, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813022, 152, 33, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813023, 152, 34, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813024, 152, 35, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813025, 152, 36, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813026, 152, 37, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813027, 152, 38, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813028, 152, 39, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813029, 152, 40, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813030, 152, 41, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813031, 152, 42, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813032, 152, 43, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813033, 152, 44, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813034, 152, 45, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813035, 152, 46, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813036, 152, 47, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813037, 152, 48, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813038, 152, 49, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813039, 152, 50, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813040, 152, 51, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813041, 152, 52, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813042, 152, 53, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813043, 152, 54, 0, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813044, 152, 55, 10, true, false, 12549, 64, 32, 8, 0, 0, 0, false, {  } },
			{ 31813045, 152, 56, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813046, 152, 57, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813047, 152, 58, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813048, 152, 59, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813049, 152, 60, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813050, 152, 61, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813051, 152, 62, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813052, 152, 63, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813053, 153, 1, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813054, 153, 2, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813055, 153, 3, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813056, 153, 4, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813057, 153, 5, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813058, 153, 6, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813059, 153, 7, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813060, 153, 8, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813061, 153, 9, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813062, 153, 10, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813063, 153, 11, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813064, 153, 12, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813065, 153, 13, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813066, 153, 14, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813067, 153, 15, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813068, 153, 16, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813069, 153, 17, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813070, 153, 18, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813071, 153, 19, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813072, 153, 20, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813073, 153, 21, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813074, 153, 22, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813075, 153, 23, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813076, 153, 24, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813077, 153, 25, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813078, 153, 26, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813079, 153, 27, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813080, 153, 28, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813081, 153, 29, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813082, 153, 30, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813083, 153, 31, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813084, 153, 32, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813085, 153, 33, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813086, 153, 34, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813087, 153, 35, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813088, 153, 36, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813089, 153, 37, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813090, 153, 38, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813091, 153, 39, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813092, 153, 40, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813093, 153, 41, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813094, 153, 42, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813095, 153, 43, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813096, 153, 44, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813097, 153, 45, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813098, 153, 46, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813099, 153, 47, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813100, 153, 48, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813101, 153, 49, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813102, 153, 50, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813103, 153, 51, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813104, 153, 52, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813105, 153, 53, 10, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813106, 153, 54, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813107, 153, 55, 0, true, false, 12550, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813108, 153, 56, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813109, 153, 57, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813110, 153, 58, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813111, 153, 59, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813112, 153, 60, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813113, 153, 61, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813114, 153, 62, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813115, 153, 63, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813116, 154, 1, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813117, 154, 2, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813118, 154, 3, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813119, 154, 4, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813120, 154, 5, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813121, 154, 6, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813122, 154, 7, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813123, 154, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813124, 154, 9, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813125, 154, 10, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813126, 154, 11, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813127, 154, 12, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813128, 154, 13, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813129, 154, 14, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813130, 154, 15, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813131, 154, 16, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813132, 154, 17, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813133, 154, 18, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813134, 154, 19, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813135, 154, 20, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813136, 154, 21, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813137, 154, 22, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813138, 154, 23, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813139, 154, 24, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813140, 154, 25, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813141, 154, 26, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813142, 154, 27, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813143, 154, 28, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813144, 154, 29, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813145, 154, 30, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813146, 154, 31, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813147, 154, 32, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813148, 154, 33, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813149, 154, 34, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813150, 154, 35, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813151, 154, 36, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813152, 154, 37, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813153, 154, 38, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813154, 154, 39, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813155, 154, 40, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813156, 154, 41, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813157, 154, 42, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813158, 154, 43, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813159, 154, 44, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813160, 154, 45, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813161, 154, 46, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813162, 154, 47, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813163, 154, 48, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813164, 154, 49, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813165, 154, 50, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813166, 154, 51, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813167, 154, 52, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813168, 154, 53, 0, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813169, 154, 54, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813170, 154, 55, 10, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813171, 154, 56, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813172, 154, 57, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813173, 154, 58, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813174, 154, 59, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813175, 154, 60, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813176, 154, 61, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813177, 154, 62, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813178, 154, 63, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813179, 155, 1, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813180, 155, 2, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813181, 155, 3, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813182, 155, 4, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813183, 155, 5, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813184, 155, 6, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813185, 155, 7, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813186, 155, 8, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813187, 155, 9, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813188, 155, 10, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813189, 155, 11, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813190, 155, 12, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813191, 155, 13, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813192, 155, 14, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813193, 155, 15, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813194, 155, 16, 9, true, false, 16383, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813195, 155, 17, 9, true, false, 16383, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813196, 155, 18, 13, true, false, 16383, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813197, 155, 19, 13, true, false, 16383, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813198, 155, 20, 13, true, false, 16383, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813199, 155, 21, 13, true, false, 16383, 0, 0, 0, 0, 0, 0, false, { { 21, 17, 27 } } },
			{ 31813200, 155, 22, 9, true, true, 16383, 0, 0, 0, 0, 255, 9, true, {  } },
			{ 31813201, 155, 23, 9, true, true, 14335, 213, 255, 9, 1, 255, 9, true, {  } },
			{ 31813202, 155, 24, 9, true, true, 14335, 213, 255, 9, 2, 255, 9, true, {  } },
			{ 31813203, 155, 25, 9, true, true, 14335, 213, 255, 9, 3, 18, 8, false, {  } },
			{ 31813204, 155, 26, 9, true, true, 12439, 192, 18, 8, 4, 3, 8, false, {  } },
			{ 31813205, 155, 27, 9, true, true, 12319, 0, 3, 8, 5, 8, 8, false, {  } },
			{ 31813206, 155, 28, 9, true, true, 12359, 207, 8, 8, 6, 9, 8, false, {  } },
			{ 31813207, 155, 29, 9, true, true, 12367, 192, 9, 8, 7, 14, 8, false, {  } },
			{ 31813208, 155, 30, 9, true, true, 12407, 15, 14, 8, 8, 7, 8, false, {  } },
			{ 31813209, 155, 31, 9, true, true, 12351, 15, 7, 8, 9, 32, 8, false, {  } },
			{ 31813210, 155, 32, 9, true, true, 12551, 0, 32, 8, 10, 6, 8, false, {  } },
			{ 31813211, 155, 33, 9, true, true, 12343, 0, 6, 8, 11, 15, 8, false, {  } },
			{ 31813212, 155, 34, 9, true, true, 12415, 192, 15, 8, 12, 18, 8, false, {  } },
			{ 31813213, 155, 35, 9, true, true, 12439, 192, 18, 8, 13, 32, 8, false, {  } },
			{ 31813214, 155, 36, 9, true, true, 12551, 0, 32, 8, 14, 12, 8, false, {  } },
			{ 31813215, 155, 37, 9, true, true, 12391, 0, 12, 8, 15, 42, 8, false, {  } },
			{ 31813216, 155, 38, 9, true, true, 12631, 0, 42, 8, 16, 32, 8, false, {  } },
			{ 31813217, 155, 39, 9, true, true, 12551, 0, 32, 8, 17, 32, 8, false, {  } },
			{ 31813218, 155, 40, 9, true, true, 12551, 0, 32, 8, 18, 32, 8, false, {  } },
			{ 31813219, 155, 41, 9, true, true, 12551, 0, 32, 8, 19, 32, 8, false, {  } },
			{ 31813220, 155, 42, 9, true, true, 12551, 0, 32, 8, 20, 32, 8, false, {  } },
			{ 31813221, 155, 43, 9, true, true, 12551, 0, 32, 8, 21, 32, 8, false, {  } },
			{ 31813222, 155, 44, 9, true, true, 12551, 0, 32, 8, 22, 32, 8, false, {  } },
			{ 31813223, 155, 45, 9, true, true, 12551, 0, 32, 8, 23, 32, 8, false, {  } },
			{ 31813224, 155, 46, 9, true, true, 12551, 0, 32, 8, 24, 32, 8, false, {  } },
			{ 31813225, 155, 47, 9, true, true, 12551, 0, 32, 8, 25, 32, 8, false, {  } },
			{ 31813226, 155, 48, 9, true, true, 12551, 0, 32, 8, 26, 32, 8, false, {  } },
			{ 31813227, 155, 49, 9, true, true, 12551, 0, 32, 8, 27, 32, 8, false, {  } },
			{ 31813228, 155, 50, 9, true, true, 12551, 0, 32, 8, 28, 32, 8, false, {  } },
			{ 31813229, 155, 51, 9, true, true, 12551, 0, 32, 8, 29, 32, 8, false, {  } },
			{ 31813230, 155, 52, 9, true, true, 12551, 0, 32, 8, 30, 32, 8, false, {  } },
			{ 31813231, 155, 53, 9, true, true, 12551, 0, 32, 8, 31, 32, 8, false, {  } },
			{ 31813232, 155, 54, 9, true, true, 12551, 0, 32, 8, 32, 32, 8, false, {  } },
			{ 31813233, 155, 55, 9, true, false, 12551, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813234, 155, 56, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813235, 155, 57, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813236, 155, 58, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813237, 155, 59, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813238, 155, 60, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, { { 60, 24, 30 } } },
			{ 31813239, 155, 61, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813240, 155, 62, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813241, 155, 63, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813242, 156, 1, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813243, 156, 2, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813244, 156, 3, 9, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813245, 156, 4, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813246, 156, 5, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813247, 156, 6, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813248, 156, 7, 13, false, false, 0, 0, 0, 0, 0, 0, 0, false, { { 7, 17, 24 } } },
			{ 31813249, 156, 8, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813250, 156, 9, 2, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813251, 156, 10, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813252, 156, 11, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813253, 156, 12, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813254, 156, 13, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813255, 156, 14, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813256, 156, 15, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813257, 156, 16, 10, true, false, 16376, 0, 255, 9, 0, 0, 0, false, {  } },
			{ 31813258, 156, 17, 0, true, false, 16376, 0, 255, 9, 0, 0, 0, false, {  } },
			{ 31813259, 156, 18, 0, true, false, 16376, 0, 255, 9, 0, 0, 0, false, {  } },
			{ 31813260, 156, 19, 0, true, false, 14480, 64, 18, 8, 0, 0, 0, false, {  } },
			{ 31813261, 156, 20, 10, true, false, 14360, 0, 3, 8, 0, 0, 0, false, {  } },
			{ 31813262, 156, 21, 10, true, false, 14400, 69, 8, 8, 0, 0, 0, false, {  } },
			{ 31813263, 156, 22, 0, true, false, 14408, 64, 9, 8, 0, 0, 0, false, {  } },
			{ 31813264, 156, 23, 0, true, false, 14448, 5, 14, 8, 0, 0, 0, false, {  } },
			{ 31813265, 156, 24, 0, true, false, 14392, 5, 7, 8, 0, 0, 0, false, {  } },
			{ 31813266, 156, 25, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813267, 156, 26, 10, true, false, 14384, 0, 6, 8, 0, 0, 0, false, {  } },
			{ 31813268, 156, 27, 0, true, false, 14456, 64, 15, 8, 0, 0, 0, false, {  } },
			{ 31813269, 156, 28, 0, true, false, 14480, 64, 18, 8, 0, 0, 0, false, {  } },
			{ 31813270, 156, 29, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813271, 156, 30, 10, true, false, 14432, 1, 12, 8, 0, 0, 0, false, {  } },
			{ 31813272, 156, 31, 10, true, false, 14672, 0, 42, 8, 0, 0, 0, false, {  } },
			{ 31813273, 156, 32, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813274, 156, 33, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813275, 156, 34, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813276, 156, 35, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813277, 156, 36, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813278, 156, 37, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813279, 156, 38, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813280, 156, 39, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813281, 156, 40, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813282, 156, 41, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813283, 156, 42, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813284, 156, 43, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813285, 156, 44, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813286, 156, 45, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813287, 156, 46, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813288, 156, 47, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813289, 156, 48, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813290, 156, 49, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813291, 156, 50, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813292, 156, 51, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813293, 156, 52, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813294, 156, 53, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813295, 156, 54, 0, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813296, 156, 55, 10, true, false, 14592, 0, 32, 8, 0, 0, 0, false, {  } },
			{ 31813297, 156, 56, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813298, 156, 57, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813299, 156, 58, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813300, 156, 59, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813301, 156, 60, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813302, 156, 61, 10, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813303, 156, 62, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
			{ 31813304, 156, 63, 0, false, false, 0, 0, 0, 0, 0, 0, 0, false, {  } },
		};
	}
	return (fixture);
}

// ---
GraphicReplayFixture VICIIGraphicReference::membersLineReplay (unsigned short frame)
{
	assert (frame < 3);
	GraphicReplayFixture fixture;
#include "fixtures/10000members-s5.inc"
#include "fixtures/10000members-s5-visible.inc"
	if (frame == 1)
	{
#include "fixtures/10000members-s5-frame2.inc"
	}
	else if (frame == 2)
	{
#include "fixtures/10000members-s5-frame3.inc"
	}
	return (fixture);
}

// ---
void VICIIGraphicReference::fetchGraphics
	(unsigned short cycle, GraphicReferenceFetch& observation)
{
	observation._gAccess = true;
	_fetched = GraphicLatch ();
	_fetched._sourceCycle = cycle;
	_fetched._graphicIndex = cycle - 16;
	if (_idle)
		observation._gAddress = (unsigned short) ((_scenario._bank << 14) |
			((_d011 & 0x40) != 0 ? 0x39ff : 0x3fff));
	else
	{
		assert (_vlmi < _matrix.size ());
		_fetched._matrixIndex = _vlmi;
		_fetched._screenCode = _matrix [_vlmi];
		_fetched._color = _colors [_vlmi];
		if (_scenario._bitmapReference)
		{
			// VICE 6569 keeps the falling BMM edge for one fetch; ECM masks A9/A10.
			const unsigned char mode = _d011 | (_fetchD011 & 0x20);
			unsigned short address = (mode & 0x20) != 0
				? (unsigned short) (((_d018 & 8) << 10) | (_vc << 3) | _rc)
				: (unsigned short) (((_d018 & 14) << 10) | (_fetched._screenCode << 3) | _rc);
			if ((mode & 0x40) != 0)
				address &= 0x39ff;
			observation._gAddress = (unsigned short) ((_scenario._bank << 14) | address);
		}
		else
			observation._gAddress = (unsigned short)
				((_scenario._bank << 14) + ((_d018 & 14) << 10) + _fetched._screenCode * 8 + _rc);
		_vlmi++;
		_vc = (unsigned short) ((_vc + 1) & 1023);
	}
	_fetched._data = readMemoryByte (observation._gAddress);
	observation._gData = _fetched._data;
	observation._gScreen = _fetched._screenCode;
	observation._gColor = _fetched._color;
}

// ---
void VICIIGraphicReference::drawCycle ()
{
	std::array <GraphicReferencePixel, 8> rendered {};
	for (unsigned char pixel = 0; pixel < 8; pixel++)
	{
		// PAL 6569: rising mode edges reach dot four; falling edges reach dot six.
		if (_scenario._bitmapReference && pixel == 4)
			_drawD011 |= _d011 & 0x60;
		if (_scenario._bitmapReference && pixel == 6)
			_drawD011 &= (unsigned char) (_d011 | 0x9f);
		rendered [pixel] = drawPixel (pixel);
	}
	composeBorder (rendered);
	resolveColors (rendered);
	advanceOutputPipeline ();
}

// ---
void VICIIGraphicReference::composeBorder (std::array <GraphicReferencePixel, 8>& rendered)
{
	// In 38-column mode the new border latch reaches dot seven; in
	// 40-column mode all eight dots still use the preceding latch.
	for (unsigned char pixel = 0; pixel < 8; pixel++)
	{
		GraphicReferencePixel& output = rendered [pixel];
		output._border = ((_d016 & 8) == 0 && pixel == 7) ? _mainBorder : _borderState;
		output._composedColor = output._border ? _scenario._borderColor : output._color;
		output._composedColorToken = output._border ? 0x20 : output._colorToken;
	}
	_borderState = _mainBorder;
}

// ---
void VICIIGraphicReference::resolveColors
	(const std::array <GraphicReferencePixel, 8>& rendered)
{
	// VICE's 6569 resolver applies the previously latched CPU color write,
	// then resolves the NEXT ring entry before emitting/replacing this one.
	// Entry zero was already resolved at dot seven of the preceding cycle.
	if (_latchedColorRegister != 0xff)
		_colorRegisters [_latchedColorRegister - 0x20] = _latchedColorValue;
	for (unsigned char pixel = 0; pixel < 8; pixel++)
	{
		GraphicReferencePixel& next = _pendingPixels [(pixel + 1) & 7];
		next._color = next._colorToken = resolveColorToken (next._colorToken);
		next._composedColor = next._composedColorToken = resolveColorToken (next._composedColorToken);
		if (_pendingPixelFlags._valid)
			_pixels [_pendingPixelFlags._x + pixel] = _pendingPixels [pixel];
		_pendingPixels [pixel] = rendered [pixel];
	}
	_pendingPixelFlags = _cycleFlags;
	// A CPU write follows this drawing phase. Collect it at the next call;
	// it reaches the color-resolution registers at the following call.
	_latchedColorRegister = _pendingColorRegister;
	_latchedColorValue = _pendingColorValue;
	_pendingColorRegister = 0xff;
}

// ---
unsigned char VICIIGraphicReference::resolveColorToken (unsigned char token) const
{
	assert (token < 16 || (token >= 0x20 && token <= 0x23));
	return (token < 16 ? token : _colorRegisters [token - 0x20]);
}

// ---
GraphicReferencePixel VICIIGraphicReference::drawPixel (unsigned char pixel)
{
	if (pixel == _xScroll)
	{
		_active = _pipe1;
		_multicolorPhase = true;
	}
	GraphicReferencePixel result;
	result._sampled = true;
	if (_scenario._bitmapReference)
	{
		const bool bitmap = (_drawD011 & 0x20) != 0;
		const bool multicolor = (_d016 & 0x10) != 0 && (bitmap || (_active._color & 8) != 0);
		if (multicolor && _multicolorPhase)
			_multicolorPixel = _active._data >> 6;
		const unsigned char pair = multicolor ? _multicolorPixel :
			(unsigned char) ((_active._data & 0x80) != 0 ? 3 : 0);
		result._foreground = (pair & 2) != 0;
		if ((_drawD011 & 0x40) != 0 && (bitmap || (_d016 & 0x10) != 0))
			result._color = result._colorToken = 0;
		else if (bitmap && multicolor)
		{
			result._color = pair == 0 ? _scenario._backgroundColors [0] : pair == 1
				? _active._screenCode >> 4 : pair == 2 ? _active._screenCode & 15 : _active._color;
			result._colorToken = pair == 0 ? 0x21 : result._color;
		}
		else if (bitmap)
			result._color = result._colorToken = pair != 0 ? _active._screenCode >> 4 : _active._screenCode & 15;
		else
		{
			result._color = pair != 0 ? _active._color : _scenario._backgroundColors [0];
			result._colorToken = pair != 0 ? _active._color : 0x21;
		}
	}
	else if ((_d016 & 0x10) != 0 && (_active._color & 8) != 0)
	{
		if (_multicolorPhase)
			_multicolorPixel = _active._data >> 6;
		result._foreground = (_multicolorPixel & 2) != 0;
		result._color = _multicolorPixel == 3 ? _active._color & 7 :
			_scenario._backgroundColors [_multicolorPixel];
		result._colorToken = _multicolorPixel == 3 ? _active._color & 7 :
			(unsigned char) (0x21 + _multicolorPixel);
	}
	else
	{
		result._foreground = (_active._data & 0x80) != 0;
		result._color = result._foreground ? _active._color : _scenario._backgroundColors [0];
		result._colorToken = result._foreground ? _active._color : 0x21;
	}
	result._screenCode = _active._screenCode;
	result._sourceCycle = _active._sourceCycle;
	result._graphicIndex = _active._graphicIndex;
	result._matrixIndex = _active._matrixIndex;
	_active._data = (unsigned char) (_active._data << 1);
	_multicolorPhase = !_multicolorPhase;
	return (result);
}

// ---
void VICIIGraphicReference::advanceOutputPipeline ()
{
	_pipe1 = _pipe0;
	_pipe0._data = _cycleFlags._visible ? _fetched._data : 0;
	_pipe0._sourceCycle = _fetched._sourceCycle;
	_pipe0._graphicIndex = _fetched._graphicIndex;
	if (_cycleFlags._visible)
	{
		_xScroll = _d016 & 7;
		if (_idle)
		{
			_pipe0._screenCode = _pipe0._color = 0;
			_pipe0._matrixIndex = 0xffff;
		}
		else
		{
			assert (_displayIndex < _matrix.size ());
			_pipe0._matrixIndex = _displayIndex;
			_pipe0._screenCode = _matrix [_displayIndex];
			_pipe0._color = _colors [_displayIndex];
			_displayIndex++;
		}
	}
	else
		_displayIndex = 0;
}

// ---
void VICIIGraphicReference::fetchMatrix (GraphicReferenceFetch& observation)
{
	assert (_vlmi < _matrix.size ());
	observation._cAccess = true;
	observation._cIndex = _vlmi;
	observation._invalidC = _prefetch != 0;
	_matrix [_vlmi] = observation._invalidC ? 0xff : readMemoryByte
		((unsigned short) ((_scenario._bank << 14) + ((_d018 & 0xf0) << 6) + _vc));
	_colors [_vlmi] = observation._invalidC ? _cpuNibble : readColorByte (_vc);
	observation._cScreen = _matrix [_vlmi];
	observation._cColor = _colors [_vlmi];
}

// ---
void VICIIGraphicReference::applyRegisterWrite (const GraphicReferenceWrite& write)
{
	if (write._register == 0x11)
		_d011 = write._value;
	else if (write._register == 0x16)
		_d016 = write._value;
	else if (write._register == 0x18)
		_d018 = write._value;
	else
	{
		_scenario._backgroundColors [write._register - 0x21] = write._value & 15;
		_pendingColorRegister = write._register;
		_pendingColorValue = write._value & 15;
	}
}

// ---
unsigned char VICIIGraphicReference::readMemoryByte (unsigned short address)
{
	if (!_scenario._capturedMemory)
		return (memoryByte (address));
	for (const GraphicReferenceMemoryByte& byte : _scenario._memory)
		if (byte._address == address)
			return (byte._value);
	_memoryIncomplete = true;
	return (0x5a);
}

// ---
unsigned char VICIIGraphicReference::readColorByte (unsigned short vc)
{
	if (!_scenario._capturedMemory)
		return (colorByte (vc));
	for (const GraphicReferenceMemoryByte& byte : _scenario._colorMemory)
		if (byte._address == vc)
			return (byte._value);
	_memoryIncomplete = true;
	return (0x0d);
}
