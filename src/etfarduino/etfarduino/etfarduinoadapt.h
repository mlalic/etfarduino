// etfarduinoadapt.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __etfarduinoadapt_H_
#define __etfarduinoadapt_H_

#include "resource.h"       // main symbols
#include "AdaptorKit.h"
#include "etfarduino_i.h"	// Za _Module (i jos nesto?)
// extern _Module objekat potreban za registry updates i sl.
EXTERN_C CComModule _Module;

#pragma warning(disable:4996) // no warnings: CComModule::UpdateRegistryClass was declared deprecated
#pragma warning(disable:4297) // For MS Visual Studio 2008: to disable warning on function not expected to throw exception.

/////////////////////////////////////////////////////////////////////////////
// Cetfarduinoadapt class -- implements ImwAdaptor
//
class ATL_NO_VTABLE Cetfarduinoadapt :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<Cetfarduinoadapt, &CLSID_etfarduinoadapt>,
	public ImwAdaptor
{
public:
DECLARE_NOT_AGGREGATABLE(Cetfarduinoadapt)

DECLARE_PROTECT_FINAL_CONSTRUCT()
DECLARE_CLASSFACTORY_SINGLETON(Cetfarduinoadapt)
BEGIN_COM_MAP(Cetfarduinoadapt)
	COM_INTERFACE_ENTRY(ImwAdaptor)
END_COM_MAP()

BEGIN_CATEGORY_MAP(Cetfarduinoadapt)
    IMPLEMENTED_CATEGORY(CATID_ImwAdaptor)
END_CATEGORY_MAP()

//TO_DO: In this macro enter (1)name of the adaptor class,..
//..(2)program ID, (3)version independent program ID, (4)index of the name..
//..bearing resource string -- from the resource.h file. Keep flags untouched.
DECLARE_REGISTRY( Cetfarduinoadapt, _T("etfarduino.etfarduinoadapt.1"), _T("etfarduino.etfarduinoadapt"),
				  IDS_PROJNAME, THREADFLAGS_BOTH )
//END TO_DO

public:
	Cetfarduinoadapt();
	~Cetfarduinoadapt();
	STDMETHOD(AdaptorInfo)(IPropContainer * Container);
    STDMETHOD(OpenDevice)(REFIID riid, long nParams, VARIANT __RPC_FAR *Param,
              REFIID EngineIID, IUnknown __RPC_FAR *pIEngine,
              void __RPC_FAR *__RPC_FAR *ppIDevice);
    STDMETHOD(TranslateError)(HRESULT eCode, BSTR * retVal);

    static OLECHAR ConstructorName[100];

private:
LPWSTR StringToLower(LPCWSTR in,LPWSTR out)
{
    while(*in)
		*out++ = towlower(*in++);
    return out;
} // end of StringToLower()

};

#endif //__etfarduinoadapt_H_
