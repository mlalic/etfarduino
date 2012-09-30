// etfarduinoadapt.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "etfarduinoAin.h"
#include "etfarduinoAout.h"
#include "etfarduinoDio.h"
#include "sarrayaccess.h" // safe array helper routines for use with daq adaptors
#include <vector>

//definition of the static member variable, which holds the adaptor..
//.."friendly name"
OLECHAR Cetfarduinoadapt::ConstructorName[100] = {L'\0'};

/////////////////////////////////////////////////////////////////////////////
// Default constructor
//
// The default constructor extracts the adaptor "friendly name" from the ..
//..program name in the registry (where it is put by the DECLARE_REGISTRY..
//..macro.
// Function is NOT MODIFIED for the simple adaptor.
/////////////////////////////////////////////////////////////////////////////
Cetfarduinoadapt::Cetfarduinoadapt()
{
	if (ConstructorName[0]=='\0')
	{
		LPOLESTR str=NULL;
		HRESULT res = OleRegGetUserType( CLSID_etfarduinoadapt,
										 USERCLASSTYPE_SHORT, &str );
		if (SUCCEEDED(res)) // if this fails the else probaby will to..
		{
			StringToLower(str,ConstructorName);
		}
		else
		{
			wcscpy(ConstructorName,L"Fix Me");
		}
		CoTaskMemFree(str);
	}
} // end of default constructor


/////////////////////////////////////////////////////////////////////////////
// Destructor
//
// Function is NOT MODIFIED for the simple adaptor.
/////////////////////////////////////////////////////////////////////////////
Cetfarduinoadapt::~Cetfarduinoadapt()
{
} // end of destructor


/////////////////////////////////////////////////////////////////////////////
// TranslateError()
//
// Function is called by the engine to translate an error code into..
//..a readable error message.
// Cetfarduinoadapt::TranslateError() calls CmwDevice::TranslateError, defined in..
//..AdaptorKit.cpp
// Function is NOT MODIFIED by the adaptor programmer.
/////////////////////////////////////////////////////////////////////////////
HRESULT Cetfarduinoadapt::TranslateError(HRESULT code,BSTR *out)
{
    return CmwDevice::TranslateError(code,out);
} // end TranslateError()


/////////////////////////////////////////////////////////////////////////////
// AdaptorInfo()
//
// The function is used to elicit relevant info about the current HW..
//..configuration from the HW API.
//  The info to extract is:
//..1)number of boards installed
//..2)board names
//..3)supported subsystems (AnalogInput, AnalogOutput, DigitalIO)
// The function is called by the engine in response to the MATLAB..
//..command DAQHWINFO
// Function MUST BE MODIFIED by the adaptor programmer.
/////////////////////////////////////////////////////////////////////////////
HRESULT Cetfarduinoadapt::AdaptorInfo(IPropContainer * Container)
{
    // Place the adaptor name in the appropriate struct in the engine.
    HRESULT hRes = Container->put_MemberValue(L"adaptorname", variant_t(ConstructorName));
    if (!(SUCCEEDED(hRes))) return hRes;

	// Get the name of the adaptor module
    TCHAR name[256];
    GetModuleFileName(_Module.GetModuleInstance(), name, 256); // null returns matlabs version (non existant)
    hRes = Container->put_MemberValue(L"adaptordllname", CComVariant(name));
    if (!(SUCCEEDED(hRes)))
		return hRes;

	// Get the list of registered boards by asking the service
	EtfArduinoService service;
	std::vector<UINT> registeredDevices;
	service.GetRegisteredDeviceIds(registeredDevices);
	short len = registeredDevices.size();

	// The board IDs (and the number of boards)
	TSafeArrayVector<CComBSTR> IDs;
	IDs.Allocate(len);

	// The board names
	TSafeArrayVector<CComBSTR> Names;
	Names.Allocate(registeredDevices.size());

	// Create the safe array
	SAFEARRAY* ps;
    SAFEARRAYBOUND arrayBounds[2];

    arrayBounds[0].lLbound = 0;
    arrayBounds[0].cElements = len;
    arrayBounds[1].lLbound = 0;
    arrayBounds[1].cElements = 3;

    ps = SafeArrayCreate(VT_BSTR, 2, arrayBounds);
    if (ps == NULL)
        throw "Failure to access SafeArray.";

	CComVariant var;
    var.parray = ps;
    var.vt = VT_ARRAY | VT_BSTR;

	// Now build up subsystems arrays -- up to 3 subsystems per board
    CComBSTR* subsystems;
    hRes = SafeArrayAccessData(ps, (void**)&subsystems);
    if (FAILED(hRes))
    {
        SafeArrayDestroy(ps);
        return hRes;
    }

	for (int i = 0; i < len; ++i)
    {
		// The supported subsystems
		bool ai = true;
		bool ao = true;
		bool dio = true;
		// Board name
		wchar_t boardName[25];
		swprintf(boardName, L"etfarduino (Device %d)", registeredDevices[i]);
		Names[i] = CComBSTR(boardName);
		// ID
		wchar_t boardId[5];
		swprintf(boardId, L"%d", registeredDevices[i]);
		IDs[i] = CComBSTR(boardId);
		// Initialize subsystems[] to change it later inside if statements (if needed)
		subsystems[i].Append("");  
		subsystems[i + len]=(BSTR)NULL;
		subsystems[i + 2 * len].Append((BSTR)NULL);
		wchar_t str[40];
		if (ai)
		{
			swprintf(str, L"analoginput('%s',%s)", (wchar_t*)ConstructorName, (wchar_t*)IDs[i]);
			subsystems[i] = str;
		}
		if (ao)
		{
			swprintf(str, L"analogoutput('%s',%s)", (wchar_t*)ConstructorName, (wchar_t*)IDs[i]);
			subsystems[i + len]=str;
		}
		if (dio)
		{
			swprintf(str, L"digitalio('%s',%s)",(wchar_t*) ConstructorName, (wchar_t*)IDs[i]);
			subsystems[i + 2 * len]=str;
		}
    }
    SafeArrayUnaccessData (ps);

	// First, return the constructor names to the engine
    hRes = Container->put_MemberValue(L"objectconstructorname", var);
    if (!(SUCCEEDED(hRes))) return hRes;

	// Return the board numbers to the engine
	var.Clear();
	IDs.Detach(&var);
    RETURN_HRESULT(Container->put_MemberValue(L"installedboardids", var));

	// Board names
	var.Clear();
	Names.Detach(&var);
	RETURN_HRESULT(Container->put_MemberValue(CComBSTR(L"boardnames"),var));
	
    return S_OK;
} // end of AdaptorInfo()


/////////////////////////////////////////////////////////////////////////////
// OpenDevice()
//
// Function is called by the engine in the response to the MATLAB request..
//..to open an adaptor. It has two goals:
//..1)to dispatch the correct Open() function, defined in the etfarduino.cpp
//..2)to populate and return to the engine the pointer to the ImwDevice ..
//..interface, which is used in consequtive calls from the engine into the..
//..adaptor.
// Function MUST BE MODIFIED by the adaptor programmer.
//////////////////////////////////////////////////////////////////////////////
HRESULT Cetfarduinoadapt::OpenDevice(REFIID riid,   long nParams, VARIANT __RPC_FAR *Param,
                               REFIID EngineIID,
                               IUnknown __RPC_FAR *pIEngine,
                               void __RPC_FAR *__RPC_FAR *ppIDevice)
{
    if (ppIDevice == NULL)
        return E_POINTER;

    long id = 0; // default to an id of 0
    if (nParams>1)
          return Error("Too many input arguments to etfarduino constructor.");
    if (nParams == 1)
    {
        RETURN_HRESULT(VariantChangeType(Param, Param, 0, VT_I4));
        id = Param[0].lVal;
    }

    bool Success = FALSE;
	CComPtr<ImwDevice> pDevice;

	if (InlineIsEqualGUID(__uuidof(ImwInput),riid))
    {
        CComObject<CetfarduinoAin>* Ain;
        CComObject<CetfarduinoAin>::CreateInstance(&Ain);
		RETURN_HRESULT(Ain->Open((IDaqEngine*)pIEngine, id));
        pDevice = Ain;
		Success = TRUE;
    }

// Uncomment the block to implement the analog output subsystem
	if ( InlineIsEqualGUID(__uuidof(ImwOutput),riid))
	{
        CComObject<CetfarduinoAout>* Aout;
        CComObject<CetfarduinoAout>::CreateInstance(&Aout);
		RETURN_HRESULT(Aout->Open((IDaqEngine*)pIEngine,id));
        pDevice=Aout;
		Success = TRUE;
	}

// Uncomment the block to implement the digital I/O subsystem
	if (InlineIsEqualGUID(__uuidof(ImwDIO),riid))
	{
        CComObject<CetfarduinoDio>* dio;
        CComObject<CetfarduinoDio>::CreateInstance(&dio);
		RETURN_HRESULT(dio->Open((IDaqEngine*)pIEngine, id));
        pDevice = dio;
		Success = TRUE;
	}

    if (Success)
	    return pDevice->QueryInterface(riid, ppIDevice);
    else
        return E_FAIL;
} // end of OpenDevice()



