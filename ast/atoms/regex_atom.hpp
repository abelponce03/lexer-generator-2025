#ifndef REGEX_ATOM_HPP
#define REGEX_ATOM_HPP

#include <string>

class RegexAtom {
public:
    enum class Type {
        Char,
        Epsilon
    };

private:
    Type type;
    char value;

public:
    // Constructor para carácter
    static RegexAtom Char(char c) {
        return RegexAtom(Type::Char, c);
    }

    // Constructor para épsilon (cadena vacía)
    static RegexAtom Epsilon() {
        return RegexAtom(Type::Epsilon, '\0');
    }

    // Getter para tipo
    Type getType() const {
        return type;
    }

    // Getter para valor de carácter (devuelve puntero a carácter o nullptr)
    const char* getChar() const {
        if (type == Type::Char) {
            return &value;
        }
        return nullptr;
    }

    // Alternativa sin std::optional
    char getCharOrDefault(char defaultValue = '\0') const {
        return (type == Type::Char) ? value : defaultValue;
    }

    // Método para verificar si el tipo es Char
    bool isChar() const {
        return type == Type::Char;
    }

    // Operador de igualdad con carácter
    bool operator==(char c) const {
        return type == Type::Char && value == c;
    }

    // Conversión a string para depuración
    std::string toString() const {
        if (type == Type::Char) {
            return std::string(1, value);
        } else {
            return "\\epsilon";
        }
    }

private:
    RegexAtom(Type t, char v) : type(t), value(v) {}
};

#endif // REGEX_ATOM_HPP