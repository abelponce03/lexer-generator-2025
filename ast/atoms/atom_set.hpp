#ifndef ATOM_SET_HPP
#define ATOM_SET_HPP

#include "charset.hpp"

class AtomSet {
public:
    enum class Type {
        CharSet,
        Wildcard
    };

private:
    Type type;
    CharSet charset;

public:
    // Constructor para conjunto de caracteres
    static AtomSet fromCharSet(const CharSet& cs) {
        return AtomSet(Type::CharSet, cs);
    }

    // Constructor para wildcard (punto en regex)
    static AtomSet Wildcard() {
        return AtomSet(Type::Wildcard, CharSet({}, false));
    }

    // Getter para tipo
    Type getType() const {
        return type;
    }

    // Getter para charset (solo válido si es de tipo CharSet)
    const CharSet* getCharSet() const {
        if (type == Type::CharSet) {
            return &charset;
        }
        return nullptr;
    }

    // Comprueba si un carácter coincide con este conjunto
    bool matches(char c) const {
        if (type == Type::Wildcard) {
            return true;  // El wildcard coincide con cualquier carácter
        } else {
            return charset.matches(c);
        }
    }

    // Conversión a string para depuración
    std::string toString() const {
        if (type == Type::Wildcard) {
            return ".";
        } else {
            return charset.toString();
        }
    }

private:
    AtomSet(Type t, const CharSet& cs) : type(t), charset(cs) {}
};

// Operador de igualdad para comparar con un carácter
inline bool operator==(const AtomSet& as, char c) {
    return as.matches(c);
}

inline bool operator==(char c, const AtomSet& as) {
    return as.matches(c);
}

#endif // ATOM_SET_HPP