// etfarduinoDio.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "etfarduino_i.h"
#include "etfarduinoadapt.h"
#include "etfarduinoDio.h"

//////////////////////////////////////////////////////////////////////////////
// CetfarduinoDio()
//
// Default constructor
//////////////////////////////////////////////////////////////////////////////
CetfarduinoDio::CetfarduinoDio() {
	// Empty
}

CetfarduinoDio::~CetfarduinoDio() {
	// Empty
}

/////////////////////////////////////////////////////////////////////////////
// InterfaceSupportsErrorInfo()
//
// Function indicates whether or not an interface supports the IErrorInfo..
//..interface. It is created by the wizard.
// Function is NOT MODIFIED by the adaptor programmer.
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CetfarduinoDio::InterfaceSupportsErrorInfo(REFIID riid)
{
    static const IID* arr[] = 
    {
        &IID_IetfarduinoDio
    };
    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
        if (InlineIsEqualGUID(*arr[i],riid))
            return S_OK;
    }
    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////// 
// Open()
//
// Description:
//  This routine is called when MatLab executes the 'digitalio' instruction. 
//  Several things should be validated at this point. 
///////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoDio::Open(IUnknown* Interface, long ID)
{
   	if (ID < 0) {
		return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Invalid device ID."));
	}
	// assign the engine access pointer
    RETURN_HRESULT(CmwDevice::Open(Interface));

	DeviceId = static_cast<UINT>(ID);	// Set the Device Number to the requested device number

	// Check if this device is registered with the service
	TCHAR portName[10];
	bool registered = 
		service.CheckDeviceRegistered(DeviceId, portName);
	if (!registered) {
		char tempMessage[255];
		sprintf(tempMessage, "etfarduino: Device %d not found.", DeviceId);
		return CComCoClass<ImwDevice>::Error(CComBSTR(tempMessage));
	}

	// Set the daqhwinfo for the digitalio subsytem
	RETURN_HRESULT(SetDaqHwInfo()); 
	
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// SetDaqHWInfo()
//
// Description:
// Set the fields needed for DaqHwInfo. 
//  It is used when you call daqhwinfo(analoginput('etfarduino'))
///////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoDio::SetDaqHwInfo()
{
	//Adaptor Name
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"adaptorname"), CComVariant(Cetfarduinoadapt::ConstructorName)));
	
    // Device Name
	wchar_t devname[25];
	swprintf(devname, L"etfarduino (Device %d)", DeviceId);
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"devicename"), CComVariant(devname)));
	
    // Device Id
	wchar_t idStr[8];
    swprintf(idStr, L"%d", DeviceId);
    RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"id"), CComVariant(idStr)));		
	
	// Sort out the daqhwinfo for the ports
	// For now, there is only one port and one line!
	CComVariant totalLines(1);
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"totallines"), totalLines));
	
	// Set the rest of the port info
	CComVariant var;
	long ports = 1;

	// Here we create the SafeArray we are going to use to set all out port values.        
	SAFEARRAY* ps = SafeArrayCreateVector(VT_I4, 0, ports);
	if (ps == NULL) 
       return E_OUTOFMEMORY;
	// set the data type and values
	var.vt = VT_ARRAY | VT_I4;
	var.parray = ps;
	TSafeArrayAccess <long> ar(&var);  // used to access the array
	
	// Port directions
	// So far, only one port supported...
	ar[0] = 1;		// Output
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"portdirections"), var));
	
	// Port ID: 0-based index becomes its id.
	for (int i = 0; i < ports; ++i)
		ar[i] = i;
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"portids"), var));
	
	// Port line config
	for (int i = 0; i < ports; ++i)
		ar[i] = i;
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"portlineconfig"), var));
	
	// Port masks
	ar[0] = 1;
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"portlinemasks"),var) );
	
	// Vendor Driver Description
	char driverDescrip[] = "Etf Arduino Driver: Digital I/O";
	RETURN_HRESULT(_DaqHwInfo->put_MemberValue(CComBSTR(L"vendordriverdescription"),CComVariant(driverDescrip)));
	
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ***** NYI ******
// ReadValues
// Description:
//  Read Digital I/O port values.
/////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoDio::ReadValues(LONG NumberOfPorts, LONG* PortList, ULONG* Data)
{
	return E_NOTIMPL;
}


///////////////////////////////////////////////////////////////////////////////
// WriteValues
//
// Description:
//  Write Digital I/O port values.
////////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoDio::WriteValues(LONG NumberOfPorts, LONG* PortList, ULONG* Data, ULONG* Mask)
{
	for (int i = 0; i < NumberOfPorts; ++i) {
		if (!service.SendDigitalValue(DeviceId, PortList[i], Data[i])) {
			return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Digital write failed."));
		}
	}
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// SetPortDirection
//
// Description:
//  Set the digital I/O port direction. 
//////////////////////////////////////////////////////////////////////////////
HRESULT CetfarduinoDio::SetPortDirection(LONG Port, ULONG DirectionValues)
{
	// For now, there is no possiblity to change any port direction
	return CComCoClass<ImwDevice>::Error(CComBSTR("etfarduino: Port direction change is not supported."));	
}

////////////////////////////////////////////////////////////////////////////
// PortLineWidth()
//
// Description:
//	This function is used to return the number of lines from a channel,
//  given the channel mask.
///////////////////////////////////////////////////////////////////////////
long CetfarduinoDio::PortLineWidth(DWORD chanMask)
{
	long retval = 0; // The value we are going to return 
	for (int i = 0; i < 32; i++) // The maximum number of bits the mask can be is 32
	{
		retval = retval + (chanMask & 1); // Check if the last bit of the mask is set
		chanMask = (chanMask >> 1); // Right shift the mask, to get the next bit to check
	}
	return retval; // Return the value
}
