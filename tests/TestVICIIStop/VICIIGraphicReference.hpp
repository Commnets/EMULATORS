#ifndef __TESTVICIIGRAPHICREFERENCE__
#define __TESTVICIIGRAPHICREFERENCE__

#include <array>
#include <string>
#include <vector>

/** Controlled PAL stimulus, not a complete demo snapshot. */
struct GraphicReferenceWrite final
{
	unsigned short _cycle;
	unsigned char _register, _value;
};

struct GraphicReferenceMemoryByte final
{
	unsigned short _address;
	unsigned char _value;
};

struct GraphicReferenceScenario final
{
	std::string _name;
	unsigned short _rasterLine, _initialVC;
	unsigned char _initialRC;
	bool _initialIdle;
	unsigned char _initialD011, _initialD016;
	std::vector <GraphicReferenceWrite> _writes;
	// 0: hires colors; 1: all multicolor; 2: alternating hires/multicolor.
	unsigned char _initialColorMode = 0;
	std::array <unsigned char, 3> _backgroundColors { { 0, 0, 0 } };
	unsigned char _borderColor = 0;
	bool _compareComposition = false;
	unsigned char _bank = 0, _initialD018 = 0x14, _initialCpuNibble = 9;
	bool _capturedMemory = false, _matrixSnapshot = false;
	std::vector <GraphicReferenceMemoryByte> _memory, _colorMemory;
	std::array <unsigned char, 40> _initialMatrix {}, _initialColors {};
	bool _compareForeground = true;
	// Bitmap/ECM stimulus is opt-in; the existing text cases keep their contract.
	bool _bitmapReference = false;
	// Source identity is opt-in so existing color/priority contracts do not change.
	bool _compareSourceIdentity = false;
};

struct GraphicReferencePixel final
{
	bool _sampled = false, _foreground = false;
	unsigned char _color = 0, _screenCode = 0;
	unsigned short _sourceCycle = 0, _graphicIndex = 0xffff, _matrixIndex = 0xffff;
	bool _border = false;
	unsigned char _composedColor = 0;
	// 0..15 are literal colors; $20..$23 select the deferred color registers.
	unsigned char _colorToken = 0, _composedColorToken = 0;
};

struct GraphicReferenceFetch final
{
	bool _gAccess = false, _cAccess = false, _invalidC = false, _baLow = false;
	unsigned short _gAddress = 0, _vc = 0, _vlmi = 0, _cIndex = 0;
	unsigned char _gData = 0, _gScreen = 0, _gColor = 0;
	unsigned char _cScreen = 0, _cColor = 0, _prefetch = 4;
};

struct GraphicReplayCycle final
{
	unsigned int _absoluteCycle;
	unsigned short _line, _cycle;
	unsigned char _cpuNibble;
	bool _gAccess, _cAccess;
	unsigned short _gAddress;
	unsigned char _gData, _gScreen, _gColor;
	unsigned short _cIndex;
	unsigned char _cScreen, _cColor;
	bool _invalidC;
	std::vector <GraphicReferenceWrite> _writes;
};

struct GraphicReplayFixture final
{
	GraphicReferenceScenario _scenario;
	std::vector <GraphicReplayCycle> _cycles;
	// Missing captured colors are not converted into an exact-demo approval.
	bool _capturedBackgrounds = false, _capturedBorder = false;
};

/** Independent, restricted PAL text and opt-in bitmap reference. \n
	Uses VICE's fetch/draw phase separation, display index and output latches. \n
	No production headers, VIC-II constants, buffers or sequencer helpers are used. */
class VICIIGraphicReference final
{
	public:
	using Pixels = std::array <GraphicReferencePixel, 504>;
	using Fetches = std::array <GraphicReferenceFetch, 64>;

	VICIIGraphicReference ();
	void initialize (const GraphicReferenceScenario& scenario);
	void executeCycle (unsigned short cycle);
	void beginNextLine (unsigned short line, const std::vector <GraphicReferenceWrite>& writes);
	void setCpuNibble (unsigned char nibble)
							{ _cpuNibble = nibble; }
	bool memoryIncomplete () const
							{ return (_memoryIncomplete); }

	const Pixels& outputPixels () const
							{ return (_pixels); }
	const Fetches& fetches () const
							{ return (_fetches); }
	unsigned short videoCounterBase () const
							{ return (_vcbase); }

	static unsigned char memoryByte (unsigned short address);
	static unsigned char colorByte (unsigned short vc);
	static unsigned char initialScreenCode (unsigned short index);
	static unsigned char initialColor (unsigned short index);
	static unsigned char stimulusInitialColor
		(const GraphicReferenceScenario& scenario, unsigned short index);
	static bool pixelsMatch (const Pixels& actual, const Pixels& expected,
		unsigned short& firstDifferentX, bool compareForeground = true,
		bool compareSourceIdentity = false);
	static bool testIndependentAnchors ();
	static bool testMulticolorAndBorderAnchors ();
	static bool testColorResolutionAnchors ();
	static bool testBitmapAndInvalidModeAnchors ();
	static GraphicReplayFixture demoLineReplay (bool demoB);
	static GraphicReplayFixture membersLineReplay (unsigned short frame = 0);

	private:
	struct GraphicLatch final
	{
		unsigned char _data = 0, _screenCode = 0, _color = 0;
		unsigned short _sourceCycle = 0, _graphicIndex = 0xffff, _matrixIndex = 0xffff;
	};

	struct CycleFlags final
	{
		bool _valid = false, _visible = false;
		unsigned short _x = 0;
	};

	void fetchGraphics (unsigned short cycle, GraphicReferenceFetch& observation);
	void drawCycle ();
	void composeBorder (std::array <GraphicReferencePixel, 8>& rendered);
	void resolveColors (const std::array <GraphicReferencePixel, 8>& rendered);
	unsigned char resolveColorToken (unsigned char token) const;
	GraphicReferencePixel drawPixel (unsigned char pixel);
	void advanceOutputPipeline ();
	void fetchMatrix (GraphicReferenceFetch& observation);
	void applyRegisterWrite (const GraphicReferenceWrite& write);
	unsigned char readMemoryByte (unsigned short address);
	unsigned char readColorByte (unsigned short vc);

	GraphicReferenceScenario _scenario;
	unsigned short _vc = 0, _vcbase = 0, _vlmi = 0, _displayIndex = 0;
	unsigned char _rc = 0, _d011 = 0, _d016 = 0, _prefetch = 4;
	unsigned char _xScroll = 0;
	unsigned char _fetchD011 = 0, _drawD011 = 0;
	bool _idle = true;
	bool _multicolorPhase = false, _mainBorder = true, _borderState = true;
	unsigned char _multicolorPixel = 0;
	unsigned char _d018 = 0x14, _cpuNibble = 9;
	bool _memoryIncomplete = false;
	std::array <unsigned char, 40> _matrix {}, _colors {};
	GraphicLatch _fetched, _pipe0, _pipe1, _active;
	CycleFlags _cycleFlags;
	std::array <GraphicReferencePixel, 8> _pendingPixels {};
	CycleFlags _pendingPixelFlags;
	std::array <unsigned char, 4> _colorRegisters {};
	unsigned char _pendingColorRegister = 0xff, _pendingColorValue = 0;
	unsigned char _latchedColorRegister = 0xff, _latchedColorValue = 0;
	Pixels _pixels {};
	Fetches _fetches {};
};

#endif
