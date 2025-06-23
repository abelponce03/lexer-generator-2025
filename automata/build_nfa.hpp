#ifndef BOB_CONSTRUYE_NFA_HPP
#define BOB_CONSTRUYE_NFA_HPP

#include <memory>
#include <string>
#include "nfa.hpp"

class Expression;
class BinOp;
class UnOp;
class MatchableAtom;

class NfaBuild {
public:
    // Construye un NFA a partir de una expresión
    std::shared_ptr<NFA> buildFromExpression(const std::shared_ptr<Expression>& expr);
    
    // Construye un NFA a partir de un átomo
    std::shared_ptr<NFA> buildFromAtom(const MatchableAtom& atom);
    
    // Construye un NFA a partir de una operación binaria
    std::shared_ptr<NFA> buildFromBinaryOp(const BinOp& binOp);
    
    // Construye un NFA a partir de una operación unaria
    std::shared_ptr<NFA> buildFromUnaryOp(const UnOp& unOp);
    
    // Construye un NFA directamente desde una cadena de expresión regular
    std::shared_ptr<NFA> buildFromRegex(const std::string& regex);
};

#endif // BOB_CONSTRUYE_NFA_HPP