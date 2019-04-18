#ifndef EXTERNAPI_H
#define EXTERNAPI_H

#include <windows.h>

namespace Expi {
class CExternMod {
public:
    CExternMod(const char* dllname) {
        Instance = LoadLibraryA(dllname);
    }

    ~CExternMod() {
        if (Instance) {
            FreeLibrary(Instance);
        }
    }

    bool IsAvailable() {
        return Instance != 0;
    }

public:
    HMODULE Instance;
};

template <typename T>
class CExternApi {
public:
    CExternApi(const char* dllname, const char* funcname) {
        call = 0;
        m_hinst = LoadLibraryA(dllname);

        if (m_hinst) {
            call = (T)GetProcAddress(m_hinst, funcname);
        }
    }
    CExternApi(CExternMod* hmod, const char* funcname) {
        m_hinst = 0;
        call = 0;

        if (hmod && hmod->IsAvailable()) {
            call = (T)GetProcAddress(hmod->Instance, funcname);
        }
    }
    ~CExternApi() {
        if (m_hinst) {
            FreeLibrary(m_hinst);
        }
    }

    bool IsAvailable() {
        return call != 0;
    }

public:
    T call;

private:
    HMODULE m_hinst;
};
}

#endif //!EXTERNAPI_H