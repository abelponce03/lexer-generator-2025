#ifndef MATCHABLE_ATOM_HPP
#define MATCHABLE_ATOM_HPP

#include "regex_atom.hpp"
#include "atom_set.hpp"

class MatchableAtom {
public:
    enum class Type {
        Atom,
        AtomSet
    };

private:
    Type type;
    
    // Unión para almacenar uno de los dos posibles valores
    union {
        RegexAtom atom;
        AtomSet atomSet;
    };

public:
    // Constructor para RegexAtom
    MatchableAtom(const RegexAtom& a) : type(Type::Atom) {
        new (&atom) RegexAtom(a); // Placement new
    }
    
    // Constructor para AtomSet
    MatchableAtom(const AtomSet& as) : type(Type::AtomSet) {
        new (&atomSet) AtomSet(as); // Placement new
    }
    
    // Destructor
    ~MatchableAtom() {
        if (type == Type::Atom) {
            atom.~RegexAtom();
        } else {
            atomSet.~AtomSet();
        }
    }
    
    // Constructor de copia
    MatchableAtom(const MatchableAtom& other) : type(other.type) {
        if (type == Type::Atom) {
            new (&atom) RegexAtom(other.atom);
        } else {
            new (&atomSet) AtomSet(other.atomSet);
        }
    }
    
    // Operador de asignación
    MatchableAtom& operator=(const MatchableAtom& other) {
        if (this != &other) {
            this->~MatchableAtom(); // Destruir el objeto actual
            new (this) MatchableAtom(other); // Reconstruir con el otro
        }
        return *this;
    }
    
    // Getter para tipo
    Type getType() const {
        return type;
    }

    // Getter para atom
    const RegexAtom* getAtom() const {
        return (type == Type::Atom) ? &atom : nullptr;
    }

    // Getter para atomSet
    const AtomSet* getAtomSet() const {
        return (type == Type::AtomSet) ? &atomSet : nullptr;
    }

    // Comprueba si un carácter coincide con este átomo
    bool matches(char c) const {
        if (type == Type::Atom) {
            return atom == c;
        } else {
            return atomSet.matches(c);
        }
    }

    // Conversión a string para depuración
    std::string toString() const {
        if (type == Type::Atom) {
            return atom.toString();
        } else {
            return atomSet.toString();
        }
    }
};

// Operador de igualdad para comparar con un carácter
inline bool operator==(const MatchableAtom& ma, char c) {
    return ma.matches(c);
}

inline bool operator==(char c, const MatchableAtom& ma) {
    return ma.matches(c);
}

#endif // MATCHABLE_ATOM_HPP