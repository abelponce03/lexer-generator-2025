#ifndef LEXER_NFA_HPP
#define LEXER_NFA_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "nfa.hpp"

// Declaración anticipada
class LexerDFA;

class LexerNFA {
public:
    // Un patrón de token asocia un nombre de token con un NFA que lo reconoce
    struct TokenPattern {
        std::string name;
        std::shared_ptr<NFA> nfa;
        bool isIgnorable; // Si se debe omitir este token (como espacios en blanco)
    };
    
    // Agrega un patrón al lexer
    void addPattern(const std::string& tokenName, std::shared_ptr<NFA> nfa, bool isIgnorable = false);
    
    // Construye el NFA combinado a partir de todos los patrones
    void build();
    
    // Convierte a un DFA para una coincidencia más eficiente
    std::shared_ptr<LexerDFA> toDFA();
    
    // Accessors para uso de LexerDFA
    std::shared_ptr<NFA> getCombinedNfa() const { return combinedNfa; }
    const std::map<NFA::State, std::pair<std::string, bool>>& getFinalStateToToken() const { 
        return finalStateToToken; 
    }
    
private:
    std::vector<TokenPattern> patterns;
    std::shared_ptr<NFA> combinedNfa;
    
    // Mapea estados finales a su nombre de token y si deben ignorarse
    std::map<NFA::State, std::pair<std::string, bool>> finalStateToToken;
};

#endif // LEXER_NFA_HPP