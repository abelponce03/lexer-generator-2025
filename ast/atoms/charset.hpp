#ifndef CHARSET_H
#define CHARSET_H

#include <vector>
#include <string>
#include <algorithm>
#include <utility>

class CharSet {
public:
    std::vector<std::pair<char, char>> range;
    bool neg;

    // Constructor que normaliza y ordena los rangos
    CharSet(const std::vector<std::pair<char, char>>& range, bool neg = false) 
        : neg(neg) {
        this->range = range;
        // Normalizar rangos (asegurar que start <= end)
        for (auto& r : this->range) {
            if (r.first > r.second) {
                std::swap(r.first, r.second);
            }
        }
        // Ordenar rangos
        std::sort(this->range.begin(), this->range.end());
    }

    // Comprueba si un carácter coincide con este conjunto
    bool matches(char c) const {
        bool result = false;
        for (const auto& r : range) {
            if (c >= r.first && c <= r.second) {
                result = true;
                break;
            }
        }
        return neg ? !result : result;
    }

    // Representación como string para depuración
    std::string toString() const {
        std::string result = "[";
        if (neg) {
            result += "^";
        }
        for (const auto& r : range) {
            result += r.first;
            result += '-';
            result += r.second;
        }
        result += "]";
        return result;
    }

    // Verifica si este conjunto de caracteres está negado (por ejemplo, [^a-z])
    bool isNegated() const {
        return neg;
    }

    // Obtiene los rangos de caracteres en este conjunto
    const std::vector<std::pair<char, char>>& getRanges() const {
        return range;
    }
};

// Operador de igualdad para comparar con un carácter
inline bool operator==(const CharSet& cs, char c) {
    return cs.matches(c);
}

inline bool operator==(char c, const CharSet& cs) {
    return cs.matches(c);
}

#endif // CHARSET_H