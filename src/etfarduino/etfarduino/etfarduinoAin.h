// etfarduinoAin.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __etfarduinoAIN_H_
#define __etfarduinoAIN_H_

#include "resource.h"       // main symbols
#include "AdaptorKit.h"
#include "EtfArduinoService.h"
#include "MessageWindow.h"
#include "etfarduinoadapt.h"

EXTERN_C CComModule _Module;

#pragma warning(disable:4996) // no warnings: CComModule::UpdateRegistryClass was declared deprecated

//This abstract class extends the CswClockedDevice class by a single ..
//..pure virtual function GetSingleValue()
class ATL_NO_VTABLE CetfarduinoInputBase: public CswClockedDevice
{
public:
    typedef unsigned short RawDataType;
    enum BitsEnum {Bits=10}; // bits must fit in rawdatatype
    virtual HRESULT GetSingleValue(int index, RawDataType *Value) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CetfarduinoAin class declaration
//
// CetfarduinoAin is based on ImwDevice and ImwInput via chains:..
//.. ImwDevice -> CmwDevice -> CswClockedDevice -> CetfarduinoInputBase ->..
//.. TADDevice -> CetfarduinoAin  and..
//.. ImwInput -> TADDevice -> CetfarduinoAin
class ATL_NO_VTABLE CetfarduinoAin :
	public TADDevice<CetfarduinoInputBase>, //is based on ImwDevice
	public CComCoClass<CetfarduinoAin, &CLSID_etfarduinoAin>,
	public ISupportErrorInfo,
	public IDispatchImpl<IetfarduinoAin, &IID_IetfarduinoAin, &LIBID_etfarduinoLib>
{
    typedef TADDevice<CetfarduinoInputBase> TBaseObj;

public:

// In this macro enter (1)name of the adaptor class,..
//..(2)program ID, (3)version independent program ID, (4)index of the name..
//..bearing resource string -- from the resource.h file. Keep flags untouched.
DECLARE_REGISTRY( CetfarduinoAdapt, _T("etfarduino.etfarduinoAin.1"), _T("etfarduino.etfarduinoAin"),
				  IDS_PROJNAME, THREADFLAGS_BOTH )

//this line is not needed if the program does not support aggregation
DECLARE_PROTECT_FINAL_CONSTRUCT()

//ATL macros internally implementing QueryInterface() for the mapped interfaces
BEGIN_COM_MAP(CetfarduinoAin)
	COM_INTERFACE_ENTRY(IetfarduinoAin)
	COM_INTERFACE_ENTRY(ImwDevice)
	COM_INTERFACE_ENTRY(ImwInput)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

public:
	// -----------------------
	// Public member functions
	// -----------------------
	CetfarduinoAin();

    HRESULT Open(IUnknown* Interface, long ID);
	HRESULT SetDaqHwInfo();

	STDMETHOD(SetChannelProperty)(long UserVal, tagNESTABLEPROP* pChan, VARIANT* NewValue);
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
	// Metoda se poziva kada se dodaju, izbrisu ili izmijene kanali ai objekta
	STDMETHOD(ChildChange)(DWORD typeofchange, NESTABLEPROP* pChan);
	// Metoda se poziva kada se pozove set za neki property u MATLAB-u
	STDMETHOD(SetProperty)(long User, VARIANT* NewValue);
	// Metoda se poziva kada se pozove getsample funkcija u MATLAB okruzenju
    HRESULT GetSingleValue(int chan, RawDataType* value);
	// Metoda se poziva kada se pozove start funkcija u MATLAB okruzenju
	STDMETHOD(Start)();
	// Za nas Arduino sistem, Trigger se poziva odmah nakon start, obzirom da hw triggeri
	// nisu podrzani.
	STDMETHOD(Trigger)();
	// Metoda se poziva kada se pozove stop funkcija u MATLAB okruzenju
	STDMETHOD(Stop)();
	// Metoda koja sluzi za handleovanje poruka koje sistem dobiva od EtfArduinoDriverService
	void ReceivedMessage(UINT wParam, long lParam);
    UINT DeviceId;
private:
	// ----------------------
	// Private members
	// ----------------------
	// Private struct representing a range of values
	struct Range {
		double minVal;
		double maxVal;
		Range(double minVal, double maxVal) : minVal(minVal), maxVal(maxVal) { }
	};
	typedef std::vector<Range> RangeList;
	// List of ranges representing all valid input ranges for the device
	RangeList _validInputRanges;
	// Frequency of the internal clock which dictates the possible sample rates of
	// the device.
	int		tickFrequency;
	// Vrijednosti minimalnog i maksimalnog SampleRate-a kojeg podrzava Arduino kartica
	double	m_minBoardSampleRate;
	double	m_maxBoardSampleRate;
	// Oznacava da li je u toku zadatak akvizicije
	bool m_running;
	// Prati broj sampleova koji su skupljeni u trenutnom acquisition job
	__int64 m_samplesThisRun;
	// Broj koji oznacava na koju je vrijednost postavljena velicina buffera u engineu preko
	// BufferingConfig propertyja
	long m_engineBufferSamples;
	// Cuvati referencu na MessageWindow
	MessageWindow& msgWindow;
	// Property koji oznacava koji clocking mehanizam se koristi: SW ili Internal (HW)
	TRemoteProp<long> pClockSource;
	// Property koji oznacava broj sampleova koji se trebaju dobaviti prije zavrsetka akvizicije
	TRemoteProp<double> pSamplesPerTrigger;
	// Property koji prati broj ponavljanja triggera za svaki run
	TRemoteProp<double> pTriggerRepeat;
	// Objekat koji sluzi za komunikaciju s EtfArduino servisom pomocu kojeg se vrsi komunikacija
	// s arduino uredjajima
	EtfArduinoService service;
	// ------------------------
	// Private member functions
	// ------------------------
	HRESULT FindRange(double low, double high, Range*& range);
	void CalculateSampleRate(double newSampleRate);
	// ----------------------
	// Private static members
	// ----------------------
	static	long	const	USER_INPUTRANGE		=	1;
	// Identifikacija Ain subsystema
	static	UINT	const	subsystemId			=	1;
	static int const MAX_CHANNELS = 2;

};

#endif //__etfarduinoAIN_H_
