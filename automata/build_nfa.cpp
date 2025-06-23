#include "build_nfa.hpp"
#include "../ast/expression.hpp"
#include "../regex/regex_parser_driver.hpp"
#include "../ast/atoms/regex_atom.hpp"
#include "../ast/atoms/matchable_atom.hpp"
#include "../ast/atoms/atom_set.hpp"
#include <stack>
#include <iostream>

std::shared_ptr<NFA> NfaBuild::buildFromExpression(const std::shared_ptr<Expression>& expr) {
    if (!expr) {
        return nullptr;
    }
    
    switch (expr->getType()) {
        case Expression::Type::Atom:
            return buildFromAtom(*expr->getAtom());
            
        case Expression::Type::BinOp:
            return buildFromBinaryOp(*expr->getBinOp());
            
        case Expression::Type::UnOp:
            return buildFromUnaryOp(*expr->getUnOp());
            
        default:
            return nullptr;
    }
}

std::shared_ptr<NFA> NfaBuild::buildFromAtom(const MatchableAtom& atom) {
    if (atom.getType() == MatchableAtom::Type::Atom) {
        // Es un único carácter
        const RegexAtom* regexAtom = atom.getAtom();
        if (regexAtom && regexAtom->isChar()) {
            char c = regexAtom->getCharOrDefault();
            return NFA::fromChar(c);
        }
    } else if (atom.getType() == MatchableAtom::Type::AtomSet) {
        // Es un conjunto de caracteres (como [a-z])
        const AtomSet* atomSet = atom.getAtomSet();
        if (atomSet) {
            if (atomSet->getType() == AtomSet::Type::Wildcard) {
                // Para el wildcard "."
                return NFA::fromWildcard();
            } else if (atomSet->getType() == AtomSet::Type::CharSet) {
                // Para conjuntos de caracteres como [a-z] o [^0-9]
                const CharSet* charSet = atomSet->getCharSet();
                if (charSet) {
                    // Crear una copia del CharSet para pasar como referencia
                    CharSet charSetCopy = *charSet;
                    return NFA::fromCharSet(charSetCopy);
                }
            }
        }
    }
    
    return nullptr;
}

std::shared_ptr<NFA> NfaBuild::buildFromBinaryOp(const BinOp& binOp) {
    std::shared_ptr<NFA> leftNFA = buildFromExpression(binOp.left);
    std::shared_ptr<NFA> rightNFA = buildFromExpression(binOp.right);
    
    if (!leftNFA || !rightNFA) {
        return nullptr;
    }
    
    switch (binOp.op) {
        case BinaryOperator::Concat:
            return NFA::concatenate(leftNFA, rightNFA);
            
        case BinaryOperator::Union:
            return NFA::alternate(leftNFA, rightNFA);
            
        default:
            return nullptr;
    }
}

std::shared_ptr<NFA> NfaBuild::buildFromUnaryOp(const UnOp& unOp) {
    std::shared_ptr<NFA> operandNFA = buildFromExpression(unOp.operand);
    
    if (!operandNFA) {
        return nullptr;
    }
    
    switch (unOp.op) {
        case UnaryOperator::KleeneStar:
            return NFA::kleeneStar(operandNFA);
            
        case UnaryOperator::Plus:
            return NFA::plus(operandNFA);
            
        case UnaryOperator::QuestionMark:
            return NFA::optional(operandNFA);
            
        default:
            return nullptr;
    }
}

// Para permitir el uso directo desde regex
std::shared_ptr<NFA> NfaBuild::buildFromRegex(const std::string& regex) {
    RegexParserDriver driver;
    auto expr = driver.parse(regex);
    if (!expr) {
        std::cerr << "Error al parsear la expresión regular: " << regex << std::endl;
        return nullptr;
    }
    
    return buildFromExpression(expr);
}