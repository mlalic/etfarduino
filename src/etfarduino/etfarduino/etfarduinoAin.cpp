// etfarduinoAin.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "etfarduino_i.h"
#include "etfarduinoAin.h"
#include "MessageWindow.h"
#include "math.h"

/////////////////////////////////////////////////////////////////////////////
// CetfarduinoAin()  default constructor
//
// Function performs all the necessary initializations.
// Function MUST BE MODIFIED by the adaptor programmer. It may use HW API calls.
/////////////////////////////////////////////////////////////////////////////
CetfarduinoAin::CetfarduinoAin():
	DeviceId(0),
	tickFrequency(250000),		// for boards with 16MHz processors
	m_running(false),
	m_samplesThisRun(0),
	msgWindow(MessageWindow::getInstance())
{
	// Find all valid input ranges and use them to populate the vector
	// Hard coded the 0-5 [V] range for etfarduino
	_validInputRanges.push_back(Range(0., 5.));

} // end of default constructor


/////////////////////////////////////////////////////////////////////////////
// Open()
//
// Function is called by the OpenDevice(), which is in turn called by the engine.
// CetfarduinoAin::Open() function's main goals are ..
// 1)to initialize the hardware and hardware dependent properties..
// 2)to expose pointers to the engine and the adaptor to each other..
// 3)to process the device ID, which is input by a user in the MATLAB command line.
// The call to this function goes through the hierarchical chain: ..
//..CetfarduinoAin::Open() -> CswClockedDevice::Open() -> CmwDevice::Open()
// CmwDevice::Open() in its turn populates the pointer to the..
//..engine (CmwDevice::_engine), which allows to access all engine interfaces.
// Function MUST BE MODIFIED by the adaptor programmer.
//////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoAin::Open(IUnknown* Interface, long ID)
{
	if (ID < 0) {
		// Undocumented MATLAB Daq Engine bug!
		// When Open() returns an error, the reference to the analoginput object is leaked
		// and never freed! Thereby, all resources this object owns are also leaked since
		// the destructor is never called.
		// :TODO: Implement a way to free all resources without the dtor (cannot use RAII for
		// this object)
		return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Invalid device ID."));
	}
	RETURN_HRESULT(TBaseObj::Open(Interface));
	DeviceId = ID;
	TCHAR portName[10];
	bool registered = 
		service.CheckDeviceRegistered(DeviceId, portName);
	if (!registered) {
		char tempMessage[255];
		sprintf(tempMessage, "etfarduino: Device %d not found.", DeviceId);
		return CComCoClass<ImwDevice>::Error(CComBSTR(tempMessage));
	}
    wchar_t idStr[8];
    swprintf(idStr, L"%d", ID);
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"id"), CComVariant(idStr)));		
	// Fixed device config values. They could be moved to a config file.
	m_minBoardSampleRate = 4;
	m_maxBoardSampleRate = 12500;				// 80us period
	_engine->GetBufferingConfig(&m_engineBufferSamples, NULL);
	// ------------
	// SetDaqHwInfo
	// ------------
	wchar_t deviceName[30];
	swprintf(deviceName, _T("etfarduino (Device %d)"), DeviceId);
    RETURN_HRESULT(InitHwInfo(
		VT_UI2,
		Bits,
		1,				// maximum number of channels
		SE_INPUT,		// Setting Single Ended input channels
		Cetfarduinoadapt::ConstructorName,
		deviceName));
	RETURN_HRESULT(SetDaqHwInfo());

	//////////////////////////////// Properties ///////////////////////////////////
	// The following Section sets the Propinfo for the Analog input device      //
	///////////////////////////////////////////////////////////////////////////////
	// Attach to the ClockSource property
    ATTACH_PROP(ClockSource);
	// The etfarduino must work as both software and internally clocked
    EnableSwClocking();
    pClockSource->AddMappedEnumValue(CLOCKSOURCE_SOFTWARE, TEXT("Software"));
	pClockSource->AddMappedEnumValue(CLOCKSOURCE_INTERNAL, TEXT("Internal"));
	pClockSource->put_DefaultValue(CComVariant(CLOCKSOURCE_INTERNAL));
	pClockSource = CLOCKSOURCE_INTERNAL;

	// SampleRate property
	double defaultSampleRate = 1000;
	ATTACH_PROP(SampleRate);
	pSampleRate.SetDefaultValue(CComVariant(defaultSampleRate));
	pSampleRate = defaultSampleRate;
	pSampleRate.SetRange(m_minBoardSampleRate, m_maxBoardSampleRate);
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(TEXT("minsamplerate"), CComVariant(m_minBoardSampleRate)));	
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(TEXT("maxsamplerate"), CComVariant(m_maxBoardSampleRate)));	

	// SamplesPerTrigger property
	// Needs to be set so that 1s of data is acquired with the default sample rate
	ATTACH_PROP(SamplesPerTrigger);
	pSamplesPerTrigger->put_DefaultValue(CComVariant(defaultSampleRate));
	pSamplesPerTrigger = defaultSampleRate;

	// TriggerRepeat property
	// How many times will the trigger be repeated
	ATTACH_PROP(TriggerRepeat);

	// ----------------------------
	// Postaviti InputRange default
	// ----------------------------
	CRemoteProp remoteProp;
	CComVariant var;
	// Hardcoded input range of [0, 5] [V]
	double InputRange[2] = {0., 5.};
	CreateSafeVector(InputRange, 2, &var);
	remoteProp.Attach(_EngineChannelList, TEXT("inputrange"));
	remoteProp->put_DefaultValue(var);
	remoteProp->put_User(USER_INPUTRANGE);
	remoteProp.SetRange(InputRange[0], InputRange[1]);
	remoteProp.Release();

    TRemoteProp<long>  pInputType;
    pInputType.Attach(_EnginePropRoot, L"InputType");
    pInputType->AddMappedEnumValue(0, L"SingleEnded");

    return S_OK;
} // end of Open()

////////////////////////////////////////
// SetProperty()
//
// Called when a property is changed from the MATLAB environment.
// It gives us a chance to update other properties, do error checking, etc.
// /////////////////////////////////////
STDMETHODIMP CetfarduinoAin::SetProperty(long User, VARIANT* NewValue) {
	if (User) {
		CLocalProp* pProp = PROP_FROMUSER(User);
		variant_t* val = (variant_t*) NewValue;
		// SampleRate property
		if (User == USER_VAL(pSampleRate)) {
			double newSampleRate = *val;
			if (pClockSource == CLOCKSOURCE_SOFTWARE) {
				return CswClockedDevice::SetProperty(User, NewValue);
			}
			// :TODO: Channels
			// Calculate a sample rate which is compatible with the board
			int const factor = (static_cast<double>(tickFrequency) / newSampleRate) + 0.5;
			int const period = 4 * factor;
			newSampleRate = (1. / period) * 1000 * 1000;	// us -> Hz

			if (newSampleRate > 12500) {
				// Issue warning of max sample rate
				_engine->WarningMessage(CComBSTR("etfarduino: Sample rates higher than 12,500 are not supported by etfarduino."));
				newSampleRate = 12500;
			}

			pSampleRate = newSampleRate;
			*val = pSampleRate;
		}
		// ClockSource property
		if (User == USER_VAL(pClockSource)) {
			if ((long)(*val) == CLOCKSOURCE_SOFTWARE) {
				// Software clocking chosen
				// First, change the SampleRate to SW clocked defaults
				double softwareClockedDefault = 100;		// Let's avoid magic values
				pSampleRate.SetDefaultValue(CComVariant(softwareClockedDefault));
				pSampleRate = softwareClockedDefault;
				pSampleRate.SetRange(
					CComVariant((double)MIN_SW_SAMPLERATE),
					CComVariant((double)MAX_SW_SAMPLERATE));
				EnableSwClocking();
				// SamplesPerTrigger needs to be configured so that 1s of input data is acquired
				pSamplesPerTrigger->put_DefaultValue(CComVariant(softwareClockedDefault));
				pSamplesPerTrigger = softwareClockedDefault;
			} else {
				// Internal (HW) clocking chosen
				// First, change the SampleRate to SW clocked defaults
				double internalClockDefault = 1000;
				pSampleRate.SetDefaultValue(CComVariant(internalClockDefault));
				pSampleRate = internalClockDefault;
				pSampleRate.SetRange(m_minBoardSampleRate, m_maxBoardSampleRate);
				// Za SwClocking ovaj dio uradi CswClockedDevice::EnableSwClocking
				RETURN_HRESULT(_DaqHwInfo->put_MemberValue(L"minsamplerate", CComVariant(m_minBoardSampleRate)));	
				RETURN_HRESULT(_DaqHwInfo->put_MemberValue(L"maxsamplerate", CComVariant(m_maxBoardSampleRate)));
				// SamplesPerTrigger needs to be configured so that 1s of input data is acquired
				pSamplesPerTrigger->put_DefaultValue(CComVariant(internalClockDefault));
				pSamplesPerTrigger = internalClockDefault;
			}
		}
	}
	return S_OK;
}


HRESULT CetfarduinoAin::SetDaqHwInfo() {
	// ----------------------------------
	// Set allowed InputRanges
	// ----------------------------------
	// The SAFEARRAYBOUND is used to create a multi dimensional array    
	SAFEARRAYBOUND rgsabound[2];  //the number of dimensions
    rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = _validInputRanges.size(); // bipolar and unipolar - size of dimension 1
    rgsabound[1].lLbound = 0;
    rgsabound[1].cElements = 2;     // upper and lower range values - size of dimension 2
	
    SAFEARRAY* ps = SafeArrayCreate(VT_R8, 2, rgsabound); //use the SafeArrayBound to create the array
    if (ps == NULL) 
		throw "Failure to create SafeArray.";
	
	CComVariant vinrange;
    vinrange.parray = ps;
    vinrange.vt = VT_ARRAY | VT_R8;
    double *inRange, *min, *max;
    HRESULT hr = SafeArrayAccessData(ps, (void**)&inRange);
    if (FAILED(hr)) 
    {
        SafeArrayDestroy(ps);
        throw "Failure to access SafeArray data.";
    }       
	min = inRange;
	max = inRange + _validInputRanges.size();
	
	// Iterate through the validRanges, and transfer the Input Ranges.
	for (RangeList::iterator it = _validInputRanges.begin(); it != _validInputRanges.end(); ++it) {
		*min++ = it->minVal;
		*max++ = it->maxVal;
	}
    SafeArrayUnaccessData(ps);
    RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"inputranges"), vinrange));

	// Polarity
	// Za sad je predvidjeno samo Unipolarni polaritet...
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"polarity"), CComVariant(L"Unipolar")));

	// Description
	char driverDescrip[] = "Etf Arduino Driver: Analog Input";
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"vendordriverdescription"), CComVariant(driverDescrip)));

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////
// ChildChange()
//
// Metoda koja se poziva pri brisanju, dodavanju ili izmjeni kanala na ai objektu.
///////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoAin::ChildChange(DWORD typeofchange, NESTABLEPROP* pChan) {
	if ((typeofchange & CHILDCHANGE_REASON_MASK) == ADD_CHILD) {
		if (_nChannels == 1) {
			// Ne moze vise od jednog kanala za sad
			return Error(CComBSTR("etfarduino: Nije podrzano vise od jednog kanala po AnalogInput objektu"));
		}
	}
	if (typeofchange & END_CHANGE) {
		// Update the number of channels that have been added, by querying the engine
		long numChans;
		_EngineChannelList->GetNumberOfChannels(&numChans); 
		_nChannels = numChans;	// Store data in the local variable _nChannels
		// Treba dodati ConversionOffset za sve kanale
		for (int i = 0; i < _nChannels; ++i) {
			AICHANNEL* aichan = NULL;
			_EngineChannelList->GetChannelStructLocal(i, (NESTABLEPROP**)&aichan);
			aichan->ConversionOffset = 1 << (Bits - 1);
		}
		UpdateChans(true);		// Update all related channel information
	}
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// SetChannelProperty()
//
// Called by the engine when a channel property is set
// An interface to the property, the new value and as a link to the channel are passed 
// to the method
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoAin::SetChannelProperty(long UserVal, tagNESTABLEPROP* pChan, VARIANT* NewValue)
{
    int Index = pChan->Index-1;  // we use 0 based index
	variant_t& vtNewValue = reinterpret_cast<variant_t&>(*NewValue);
	
	// Input Range property
	if(UserVal == USER_INPUTRANGE) {
		TSafeArrayAccess<double> NewRange(NewValue);
		Range* range = 0;
		RETURN_HRESULT(FindRange(NewRange[0], NewRange[1], range));
		NewRange[0] = range->minVal;
		NewRange[1] = range->maxVal;
		_updateChildren = true;
	}
	_updateChildren = true;
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// GetSingleValue()
//
// This function gets one single data point form one A/D channel, specified..
//..as a parameter.
// Function is called by GetSingleValues() of the TADDevice class, which..
// ..in turn is called by the engine as a responce to the MATLAB command GETSAMPLE.
// Function MUST BE MODIFIED by the adaptor programmer.
/////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoAin::GetSingleValue(int chan, RawDataType *value)
{
	*value = (RawDataType) service.GetSingleValue(DeviceId);
	return S_OK;
} // end of GetSingleValue()


///////////////////////////////////////////////////////////////////////////////
// Start()
//
// The method called when the user starts the data acquisition with the start 
// function
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoAin::Start()
{
	if (pClockSource == CLOCKSOURCE_SOFTWARE) {
		return InputType::Start();
	}
	// Another acquisition task cannot start before the running one is complete
	if (m_running) {
		return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Attempt to start a device before previous task stopped."));
	}
	// This run is just starting so there are no samples collected yet.
	m_samplesThisRun = 0;
	// The total buffer size needed is the product of the number of channels and the number
	// of samples to be collected.
	// The size of the buffer cannot change while the acquisition task is active so we set
	// the buffer size in the config before triggering the task.
	_engine->GetBufferingConfig(&m_engineBufferSamples, NULL);
	// Send all relevant config values to the device
	if (!service.SetAcquisitionBufferSize(DeviceId, m_engineBufferSamples * _nChannels)) {
		return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Problem configuring the device for acquisition, buffer size."));
	}
	if (!service.SetSampleRate(DeviceId, pSampleRate)) {
		return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Problem configuring the device for acquisition, sample rate."));
	}
	// :TODO: SetChannelNumber

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////
// Trigger()
//
// The trigger method, called automatically after start unless the TriggerType is
// manual and ManualHwTriggerOn is trigger
///////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoAin::Trigger()
{
	TAtlLock<CetfarduinoAin> _lock(*this);
	if (pClockSource == CLOCKSOURCE_SOFTWARE) {
		return InputType::Trigger();
	}

	msgWindow.AddDevice(DeviceId, subsystemId, this);

	if (!service.StartAcquisition(DeviceId)) {
		return CComCoClass<ImwDevice>::Error(_T("etfarduino: Problem starting acquisition."));		
	}
	// Flag the subsystem as running
	m_running = true;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////
// ReceivedMessage()
//
// Deal with the messages sent to the message window by EtfArduinoService
///////////////////////////////////////////////////////////////////////////////////////
void CetfarduinoAin::ReceivedMessage(UINT wParam, long lParam)
{
	bool mustStop = false;
	// Buffer filled!
	if (m_running) {
		// Get a pointer to the struct which will be used to return the buffer of data to
		// the engine.
		BUFFER_ST* pBuffer = 0;
		_engine->GetBuffer(0, &pBuffer);
		if (pBuffer == NULL) {
			// The engine failed to provide a buffer, cannot continue acquisition
			double triggerRepeat = pTriggerRepeat;
			if (m_samplesThisRun < (pSamplesPerTrigger * (1 + triggerRepeat))) {
				// Not all samples for the run have been acquired which means some data
				// is missing.
				// Each run is made of a number of triggers each of which has to acquire
				// a set number of samples.
				_engine->DaqEvent(EVENT_DATAMISSED, -1, m_samplesThisRun, NULL);
			}
			mustStop = true;
		} else {
			// Each channel provides a set number of samples
			long pointsPerBuffer = m_engineBufferSamples * _nChannels;
			int samplesToFetch = pBuffer->ValidPoints / _nChannels;
			service.GetBufferedData(DeviceId, pBuffer->ptr, pointsPerBuffer);
			// Set all engine struct values and return it
			pBuffer->StartPoint = m_samplesThisRun * _nChannels;
			pBuffer->ValidPoints = samplesToFetch * _nChannels;
			m_samplesThisRun += pBuffer->ValidPoints / _nChannels;
			if ((pBuffer->Flags & BUFFER_IS_LAST) || pBuffer->ValidPoints < pointsPerBuffer) {
				mustStop = true;
			}
			// Konacno vratiti buffer engineu
			_engine->PutBuffer(pBuffer);
		}
		if (mustStop) {
			Stop();
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Stop
//
// Stop the analog input acquisition
////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoAin::Stop()
{
	_engine->WarningMessage(CComBSTR(TEXT("Stopping")));
	if (pClockSource == CLOCKSOURCE_SOFTWARE) {
		return InputType::Stop();
	}

	TAtlLock<CetfarduinoAin> _lock(*this);

	if (m_running) {
		// Have the device stop acquisition
		service.StopAcquisition(DeviceId);
		// Remove it from the message window
		msgWindow.RemoveDevice(DeviceId, subsystemId);
		m_running = false;
		_engine->DaqEvent(
			EVENT_STOP,			// Event type
			-1,					// time: kada je -1 izracuna ga na osnovu sampleova
			m_samplesThisRun,	// broj sampla u kojem se desava ovaj event
			NULL);				// Message -- koristen kod error eventa
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// InterfaceSupportsErrorInfo()
//
// Function indicates whether or not an interface supports the IErrorInfo..
//..interface. It is created by the wizard.
// Function is NOT MODIFIED by the adaptor programmer.
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoAin::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IetfarduinoAin
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
} // end of InterfaceSupportsErrorInfo()

////////////////////////////////////////////////////////////
// FindRange()
// 
// Metoda u @param range stavlja pointer na Range strukturu koja
// predstavlja najblizi dozvoljeni range trazenom.
// @param low Trazena donja granica
// @param high Trazena gornja granica
// Vraca DE_INVALID_CHAN_RANGE ukoliko takav range nije moguce pronaci
////////////////////////////////////////////////////////////
HRESULT CetfarduinoAin::FindRange(double low, double high, Range*& range) {
	if (low > high) {
		return DE_INVALID_CHAN_RANGE;
	}
	if (low == high) {
        return CComCoClass<ImwDevice>::Error(_T("etfarduino: Low value cannot be equal to the high value."));
	}
	if (low < 0 || high < 0) {
        return CComCoClass<ImwDevice>::Error(_T("etfarduino: Only (positive) unipolar ranges are supported."));
	}
	// Large number -- ne moze se koristiti std::numeric_limits<double>::max() jer je Microsoft
	// u windows.h stavio max macro koji ako se #undef mozda izazove neke neocekivane posljedice
	double minRange = 6000.;
	range = 0;
	double const EPS = 1e-6;
	for (RangeList::iterator it = _validInputRanges.begin(); it != _validInputRanges.end(); ++it) {
		// Pronaci Range koji najbolje odgovara trazenom
		// Zasniva se na trazenju rangea koji ima manju donju granicu i vecu gornju granicu od trazene
		// ali tako da se uzme onaj range kojem je razlika izmedju gornje i donje granice minimalna
		if (it->minVal - low <= EPS && it->maxVal - high >= EPS && minRange > it->maxVal - it->minVal) {
			range = &(*it);
			minRange = it->maxVal - it->minVal;
		}
	}
	if (!range) {
		// Ni jedan range ne odgovara navedenim uslovima za odabir Range-a
		return DE_INVALID_CHAN_RANGE;
	}
	return S_OK;
}
