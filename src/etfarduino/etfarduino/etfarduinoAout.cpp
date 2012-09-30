// etfarduinoAout.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "etfarduino_i.h"
#include "etfarduinoAout.h"
#include "etfarduinoadapt.h"

/////////////////////////////////////////////////////////////////////////////
// CetfarduinoAout

/////////////////////////////////////////////////////////////////////////////
// Open()
//
// Function is called by the OpenDevice(), which is in turn called by the engine.
// CetfarduinoAout::Open() function's main goals are ..
// 1)to initialize the hardware and hardware dependent properties..
// 2)to expose pointers to the engine and the adaptor to each other..
// 3)to process the device ID, which is Output by a user in the MATLAB command line.
// The

/////////////////////////////////////////////////////////////////////////////
// CDemoAin()  default constructor
//
// Function performs all the necessary initializations.
// Function MUST BE MODIFIED by the adaptor programmer. It may use HW API calls.
/////////////////////////////////////////////////////////////////////////////
CetfarduinoAout::CetfarduinoAout(): DeviceId(0) {
	// Find all valid output ranges and use them to populate the vector
	// Hard coded the 0-5 [V] range for etfarduino
	_validOutputRanges.push_back(Range(0., 5.));
}


/////////////////////////////////////////////////////////////////////////////
// Open()
//
// Function is called by the OpenDevice(), which is in turn called by the engine.
// CDemoAin::Open() function's main goals are ..
// 1)to initialize the hardware and hardware dependent properties..
// 2)to expose pointers to the engine and the adaptor to each other..
// 3)to process the device ID, which is input by a user in the MATLAB command line.
// The call to this function goes through the hierarchical chain: ..
//..CDemoAin::Open() -> CswClockedDevice::Open() -> CmwDevice::Open()
// CmwDevice::Open() in its turn populates the pointer to the..
//..engine (CmwDevice::_engine), which allows to access all engine interfaces.
// Function MUST BE MODIFIED by the adaptor programmer.
//////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoAout::Open(IUnknown* Interface, long ID)
{
	if (ID < 0) {
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
    EnableSwClocking(true);
	// Clock source
    TRemoteProp<long>  pClockSource;
    pClockSource.Attach(_EnginePropRoot,L"ClockSource");
    pClockSource->AddMappedEnumValue(CLOCKSOURCE_SOFTWARE, L"Software");
    pClockSource->RemoveEnumValue(CComVariant(L"Internal"));
    pClockSource.SetDefaultValue(CLOCKSOURCE_SOFTWARE);
    pClockSource = CLOCKSOURCE_SOFTWARE;

	wchar_t devname[25];
	swprintf(devname, L"etfarduino (Device %d)", DeviceId);
    RETURN_HRESULT(InitHwInfo(VT_UI1, Bits, 1, OUTPUT, Cetfarduinoadapt::ConstructorName, devname));
	RETURN_HRESULT(SetDaqHwInfo()); 

	// Attach to properties!
	CRemoteProp remoteProp;
	CComVariant var;
	// Hardcoded input range of [0, 5] [V]
	double outputRange[2] = {0., 5.};
	CreateSafeVector(outputRange, 2, &var);
	remoteProp.Attach(_EngineChannelList, TEXT("OutputRange"));
	remoteProp->put_DefaultValue(var);
	remoteProp->put_User(USER_INPUTRANGE);
	remoteProp.SetRange(outputRange[0], outputRange[1]);
	remoteProp.Release();
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// SetDaqHWInfo()
//
// Description:
// Set the fields needed for DaqHwInfo. 
//  It is used when you call daqhwinfo(analoginput('etfarduino'))
///////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoAout::SetDaqHwInfo()
{
    // Device Id
	wchar_t idStr[8];
    swprintf(idStr, L"%d", DeviceId);
    RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"id"), CComVariant(idStr)));		
	
	// Vendor Driver Description
	char driverDescrip[] = "Etf Arduino Driver: Analog Output";
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"vendordriverdescription"), CComVariant(driverDescrip)));

	// Polarity: Only unipolar output possible
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"polarity"), CComVariant(L"Unipolar")));

	// ----------------------------------
	// Set allowed OutputRanges
	// ----------------------------------
	// The SAFEARRAYBOUND is used to create a multi dimensional array    
	SAFEARRAYBOUND rgsabound[2];  //the number of dimensions
    rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = _validOutputRanges.size();
    rgsabound[1].lLbound = 0;
    rgsabound[1].cElements = 2;
	
    SAFEARRAY* ps = SafeArrayCreate(VT_R8, 2, rgsabound);
    if (ps == NULL) 
		throw "Failure to create SafeArray.";
	
	CComVariant voutrange;
    voutrange.parray = ps;
    voutrange.vt = VT_ARRAY | VT_R8;
    double *inRange, *min, *max; 
    HRESULT hr = SafeArrayAccessData(ps, (void**)&inRange);
    if (FAILED(hr)) 
    {
        SafeArrayDestroy(ps);
        throw "Failure to access SafeArray data.";
    }       
	min = inRange;
	max = inRange + _validOutputRanges.size();
	
	// Iterate through the validRanges, and transfer the Output Ranges.
	for (RangeList::iterator it = _validOutputRanges.begin(); it != _validOutputRanges.end(); ++it) {
		*min++ = it->minVal;
		*max++ = it->maxVal;
	}
    SafeArrayUnaccessData(ps);
    RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"OutputRanges"), voutrange));

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////
// ChildChange()
//
// Method called when an analog output channel is added, deleted or changed
///////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoAout::ChildChange(DWORD typeofchange, NESTABLEPROP* pChan) {
	if ((typeofchange & CHILDCHANGE_REASON_MASK) == ADD_CHILD) {
		if (_nChannels == 1) {
			return Error(CComBSTR("etfarduino: Only one analog output channel per object is supported."));
		}
	}
	if (typeofchange & END_CHANGE) {
		// Update the number of channels that have been added, by querying the engine
		long numChans = 0;
		_EngineChannelList->GetNumberOfChannels(&numChans); 
		_nChannels = numChans;	// Store data in the local variable _nChannels
		// Treba dodati ConversionOffset za sve kanale
		for (int i = 0; i < _nChannels; ++i) {
			AOCHANNEL* aochan = NULL;
			_EngineChannelList->GetChannelStructLocal(i, (NESTABLEPROP**)&aochan);
			aochan->ConversionOffset = 1 << (Bits - 1);
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
STDMETHODIMP CetfarduinoAout::SetChannelProperty(long UserVal, tagNESTABLEPROP* pChan, VARIANT* NewValue)
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

////////////////////////////////////////////////////////////
// FindRange()
// 
// Metoda u @param range stavlja pointer na Range strukturu koja
// predstavlja najblizi dozvoljeni range trazenom.
// @param low Trazena donja granica
// @param high Trazena gornja granica
// Vraca DE_INVALID_CHAN_RANGE ukoliko takav range nije moguce pronaci
////////////////////////////////////////////////////////////
HRESULT CetfarduinoAout::FindRange(double low, double high, Range*& range) {
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
	for (RangeList::iterator it = _validOutputRanges.begin(); it != _validOutputRanges.end(); ++it) {
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

/////////////////////////////////////////////////////////////////////////////
// PutSingleValue()
//
// This function gets one single data point form one A/D channel, specified..
//..as a parameter.
// Function is called by GetSingleValues() of the TADDevice class, which..
// ..in turn is called by the engine as a responce to the MATLAB command GETSAMPLE.
// Function MUST BE MODIFIED by the adaptor programmer.
/////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoAout::PutSingleValue(int chan, RawDataType value)
{
	if (!service.PutSingleValue(DeviceId, chan, value)) {
		return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Analog output failed."));
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
STDMETHODIMP CetfarduinoAout::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IetfarduinoAout
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}
