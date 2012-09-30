// etfarduinoDio.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __ETFARDUINODIO_H__
#define __ETFARDUINODIO_H__

#include "resource.h"       // main symbols
#include "etfarduino_i.h"
#include "EtfArduinoService.h"

// Forward declaration
class Cetfarduinoadapt;

/////////////////////////////////////////////////////////////////////////////
// CetfarduinoDio
class ATL_NO_VTABLE CetfarduinoDio : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CetfarduinoDio, &CLSID_etfarduinoDio>,
	public ISupportErrorInfo,
	public TDIODevice<CmwDevice>,
	public IetfarduinoDio
{
public:
	

DECLARE_REGISTRY( Cetfarduinodapt, _T("etfarduino.etfarduinoDio.1"), _T("etfarduino.etfarduinoDio"),
				  IDS_PROJNAME, THREADFLAGS_BOTH )

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CetfarduinoDio)
	COM_INTERFACE_ENTRY(IetfarduinoDio)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(ImwDevice)
	COM_INTERFACE_ENTRY(ImwDIO)
END_COM_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IetfarduinoDio
public:
	CetfarduinoDio(); 
	~CetfarduinoDio(); 

    HRESULT SetDaqHwInfo(); 
	HRESULT Open(IUnknown *Interface,long ID); 

     // ImwDIO methods
	STDMETHOD(ReadValues)(LONG NumberOfPorts, LONG * PortList, ULONG * Data);
	STDMETHOD(WriteValues)(LONG NumberOfPorts, LONG * PortList, ULONG * Data, ULONG * Mask);
	STDMETHOD(SetPortDirection)(LONG Port, ULONG DirectionValues);
 
private:
	// Object used to talk to the service
	EtfArduinoService service;
	// Is the direction configureable?
	bool			 _DirConfigurable; 
	// The number of ports
	std::vector<int> m_portNum2Channel; 
	// The driver name
	LPCSTR			 m_driverName;
	// The device ID
	UINT			 DeviceId; 

	bool			 m_isInitializedDI; // Is DigitalInput device initialized?
	bool			 m_isInitializedDO; // Is DigitalOutput device initialized?

	long PortLineWidth( DWORD chanMask ); // Function to calculate the width of a port, given the port mask
	
};

#endif