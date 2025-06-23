#ifndef LEXER_DFA_HPP
#define LEXER_DFA_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "lexer_nfa.hpp"

// Estructura simple para representar una coincidencia
struct Match {
    std::string tokenName;
    size_t length;
    bool isIgnorable;
};

// Estructura para representar un lexema reconocido
struct Lexeme {
    std::string kind;
    std::string text;
    size_t position;
};

class LexerDFA {
public:
    void build(const LexerNFA& nfa);
    std::vector<Lexeme> scan(const std::string& input);
    
private:
    Match findLongestMatch(const std::string& input, size_t startPos);
    void minimizeDFA(); // Método para minimizar el DFA
    
    // Estado -> (Carácter -> Estado)
    std::map<int, std::map<char, int>> transitions;
    
    // Estados finales y sus tokens asociados
    std::map<int, std::pair<std::string, bool>> finalStates;
    
    int startState;
};

#endif // LEXER_DFA_HPP