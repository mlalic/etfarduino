// etfarduinoAout.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __ETFARDUINOAOUT_H__
#define __ETFARDUINOAOUT_H__

#include "resource.h"
#include "etfarduino_i.h"
#include "AdaptorKit.h"
#include "EtfArduinoService.h"
#include "MessageWindow.h"
#include "etfarduinoadapt.h"


#pragma warning(disable:4996) // no warnings: CComModule::UpdateRegistryClass was declared deprecated

//This abstract class extends the CswClockedDevice class by a single ..
//..pure virtual function PutSingleValue()
class ATL_NO_VTABLE CetfarduinoOutputBase: public CswClockedDevice
{
public:
    typedef unsigned char RawDataType;
    enum BitsEnum {Bits=8}; // bits must fit in rawdatatype
    virtual HRESULT PutSingleValue(int index, RawDataType Value) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CetfarduinoAout class declaration
//
// CetfarduinoAout is based on ImwDevice and ImwOutput via chains:..
//.. ImwDevice -> CmwDevice -> CswClockedDevice -> CetfarduinoAoutputBase ->..
//.. TADDevice -> CetfarduinoAout  and..
//.. ImwOutput -> TADDevice -> CetfarduinoAout
class ATL_NO_VTABLE CetfarduinoAout :
	public TDADevice<CetfarduinoOutputBase>, //is based on ImwDevice
	public CComCoClass<CetfarduinoAout, &CLSID_etfarduinoAout>,
	public ISupportErrorInfo,
	public IDispatchImpl<IetfarduinoAout, &IID_IetfarduinoAout, &LIBID_etfarduinoLib>
{
    typedef TDADevice<CetfarduinoOutputBase> TBaseObj;

public:

DECLARE_REGISTRY( CDemoAdapt, _T("etfarduino.etfarduinoAout.1"), _T("etfarduino.etfarduinoAout"),
				  IDS_PROJNAME, THREADFLAGS_BOTH )

//this line is not needed if the program does not support aggregation
DECLARE_PROTECT_FINAL_CONSTRUCT()

//ATL macros internally implementing QueryInterface() for the mapped interfaces
BEGIN_COM_MAP(CetfarduinoAout)
	COM_INTERFACE_ENTRY(ImwDevice)
	COM_INTERFACE_ENTRY(IetfarduinoAout)
	COM_INTERFACE_ENTRY(ImwOutput)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

public:
	CetfarduinoAout();
    HRESULT Open(IUnknown* Interface, long ID);
    HRESULT PutSingleValue(int chan, RawDataType value);
	HRESULT SetDaqHwInfo();
	STDMETHOD(ChildChange)(DWORD typeofchange, NESTABLEPROP* pChan);
	STDMETHOD(SetChannelProperty)(long UserVal, tagNESTABLEPROP* pChan, VARIANT* NewValue);

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
private:
	UINT DeviceId;
	EtfArduinoService service;
	// Private struct representing a range of values
	struct Range {
		double minVal;
		double maxVal;
		Range(double minVal, double maxVal) : minVal(minVal), maxVal(maxVal) { }
	};
	typedef std::vector<Range> RangeList;
	// A list of ranges representing all valid output ranges for the device
	RangeList _validOutputRanges;
	// Private member functions
	HRESULT FindRange(double low, double high, Range*& range);
	// Private static members
	static	long	const	USER_INPUTRANGE		=	1;
	static	UINT	const	subsystemId			=	2;
};


#endif
