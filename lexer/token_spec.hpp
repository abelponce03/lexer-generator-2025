#ifndef TOKEN_SPEC_H
#define TOKEN_SPEC_H

#include <string>

template <typename T>
class TokenSpec {
public:
    std::string pattern;
    T kind;
    bool ignore;

    // Constructor para tokens normales
    static TokenSpec<T> build(T kind, const std::string& pattern) {
        return TokenSpec<T>{pattern, kind, false};
    }

    // Constructor para tokens ignorables (como espacios en blanco)
    static TokenSpec<T> buildIgnorable(T kind, const std::string& pattern) {
        return TokenSpec<T>{pattern, kind, true};
    }

private:
    TokenSpec(const std::string& pattern, T kind, bool ignore)
        : pattern(pattern), kind(kind), ignore(ignore) {}
};

#endif // TOKEN_SPEC_H