#ifndef WINDOWS_COMPAT_HPP
#define WINDOWS_COMPAT_HPP

#ifdef _WIN32
// Asegurarse de incluir stdio.h primero
#include <stdio.h>
#include <io.h>

// Algunos compiladores/versiones necesitan esta definición explícita
extern "C" {
    int __cdecl _fileno(FILE*);
}

// Wrapper para _fileno que evita problemas con la macro
inline int safe_fileno(FILE* file) {
    return _fileno(file);
}

#define fileno safe_fileno
#endif

#endif // WINDOWS_COMPAT_HPP