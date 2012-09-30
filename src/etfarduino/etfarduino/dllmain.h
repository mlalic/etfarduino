// dllmain.h : Declaration of module class.
#ifndef DLLMAIN__HEADER

class CetfarduinoModule : public CAtlDllModuleT< CetfarduinoModule >
{
public :
	DECLARE_LIBID(LIBID_etfarduinoLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ETFARDUINO, "{B042B81D-DB91-4930-AF2D-21E81F954126}")
};

extern class CComModule _Module;
#define DLLMAIN__HEADER
#endif