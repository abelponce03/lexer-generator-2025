#include <iostream>
#include <memory>
#include <cassert>
#include "../nfa.hpp"
#include "../../ast/expression.hpp"
#include "../../ast/bin_op.hpp"
#include "../../ast/un_op.hpp"
#include "../../ast/atoms/regex_atom.hpp"
#include "../../ast/atoms/atom_set.hpp"
#include "../../ast/atoms/charset.hpp"
#include "../../ast/atoms/matchable_atom.hpp"

// Función de utilidad para crear rápidamente una expresión de carácter
std::shared_ptr<Expression> createCharExpression(char c) {
    return std::make_shared<Expression>(MatchableAtom(RegexAtom::Char(c)));
}

// Función para probar que un NFA acepta ciertas cadenas y rechaza otras
void testNFA(const NFA& nfa, const std::vector<std::string>& shouldAccept, 
             const std::vector<std::string>& shouldReject) {
    for (const auto& str : shouldAccept) {
        bool result = nfa.simulate(str);
        std::cout << "Probando '" << str << "': " << (result ? "ACEPTADA" : "RECHAZADA (debería aceptar)") << std::endl;
        assert(result);
    }
    
    for (const auto& str : shouldReject) {
        bool result = nfa.simulate(str);
        std::cout << "Probando '" << str << "': " << (!result ? "RECHAZADA" : "ACEPTADA (debería rechazar)") << std::endl;
        assert(!result);
    }
}

void testLiteralMatch() {
    std::cout << "\n=== Prueba de coincidencia literal ===\n";
    
    // Construimos "abc"
    auto exprA = createCharExpression('a');
    auto exprB = createCharExpression('b');
    auto exprC = createCharExpression('c');
    
    auto concatAB = std::make_shared<BinOp>(exprA, exprB, BinaryOperator::Concat);
    auto exprAB = std::make_shared<Expression>(concatAB);
    
    auto concatABC = std::make_shared<BinOp>(exprAB, exprC, BinaryOperator::Concat);
    auto exprABC = std::make_shared<Expression>(concatABC);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprABC);
    
    // Probar
    testNFA(nfa, {"abc"}, {"ab", "a", "bc", "abcd"});
}

void testKleeneStar() {
    std::cout << "\n=== Prueba de Kleene Star ===\n";
    
    // Construimos "a*"
    auto exprA = createCharExpression('a');
    auto starA = std::make_shared<UnOp>(exprA, UnaryOperator::KleeneStar);
    auto exprStarA = std::make_shared<Expression>(starA);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprStarA);
    
    // Probar
    testNFA(nfa, {"", "a", "aa", "aaaa"}, {"b", "ab", "ba"});
}

void testPlus() {
    std::cout << "\n=== Prueba de Operador Plus ===\n";
    
    // Construimos "a+"
    auto exprA = createCharExpression('a');
    auto plusA = std::make_shared<UnOp>(exprA, UnaryOperator::Plus);
    auto exprPlusA = std::make_shared<Expression>(plusA);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprPlusA);
    
    // Probar
    testNFA(nfa, {"a", "aa", "aaaa"}, {"", "b", "ab", "ba"});
}

void testQuestionMark() {
    std::cout << "\n=== Prueba de Operador Question Mark ===\n";
    
    // Construimos "a?"
    auto exprA = createCharExpression('a');
    auto qMarkA = std::make_shared<UnOp>(exprA, UnaryOperator::QuestionMark);
    auto exprQMarkA = std::make_shared<Expression>(qMarkA);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprQMarkA);
    
    // Probar
    testNFA(nfa, {"", "a"}, {"aa", "b", "ab"});
}

void testUnion() {
    std::cout << "\n=== Prueba de Operador Union ===\n";
    
    // Construimos "a|b"
    auto exprA = createCharExpression('a');
    auto exprB = createCharExpression('b');
    
    auto unionAB = std::make_shared<BinOp>(exprA, exprB, BinaryOperator::Union);
    auto exprUnionAB = std::make_shared<Expression>(unionAB);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprUnionAB);
    
    // Probar
    testNFA(nfa, {"a", "b"}, {"", "ab", "c"});
}

void testCharSet() {
    std::cout << "\n=== Prueba de CharSet ===\n";
    
    // Construimos "[a-z]"
    std::vector<std::pair<char, char>> range = {{'a', 'z'}};
    CharSet charset(range, false);
    AtomSet atomSet = AtomSet::fromCharSet(charset);
    MatchableAtom matchableAtom(atomSet);
    auto exprCharSet = std::make_shared<Expression>(matchableAtom);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprCharSet);
    
    // Probar
    testNFA(nfa, {"a", "b", "z"}, {"", "A", "0", "ab"});
}

void testNegatedCharSet() {
    std::cout << "\n=== Prueba de CharSet Negado ===\n";
    
    // Construimos "[^a-z]"
    std::vector<std::pair<char, char>> range = {{'a', 'z'}};
    CharSet charset(range, true);
    AtomSet atomSet = AtomSet::fromCharSet(charset);
    MatchableAtom matchableAtom(atomSet);
    auto exprCharSet = std::make_shared<Expression>(matchableAtom);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprCharSet);
    
    // Probar
    testNFA(nfa, {"A", "Z", "0", "9"}, {"a", "b", "z"});
}

void testWildcard() {
    std::cout << "\n=== Prueba de Wildcard ===\n";
    
    // Construimos "."
    AtomSet atomSet = AtomSet::Wildcard();
    MatchableAtom matchableAtom(atomSet);
    auto exprWildcard = std::make_shared<Expression>(matchableAtom);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprWildcard);
    
    // Probar
    testNFA(nfa, {"a", "Z", "0", "."}, {"", "ab"});
}

void testComplexExpression() {
    std::cout << "\n=== Prueba de Expresión Compleja ===\n";
    
    // Construimos "(a|b)*c"
    auto exprA = createCharExpression('a');
    auto exprB = createCharExpression('b');
    auto exprC = createCharExpression('c');
    
    auto unionAB = std::make_shared<BinOp>(exprA, exprB, BinaryOperator::Union);
    auto exprUnionAB = std::make_shared<Expression>(unionAB);
    
    auto starAB = std::make_shared<UnOp>(exprUnionAB, UnaryOperator::KleeneStar);
    auto exprStarAB = std::make_shared<Expression>(starAB);
    
    auto concatABC = std::make_shared<BinOp>(exprStarAB, exprC, BinaryOperator::Concat);
    auto exprABC = std::make_shared<Expression>(concatABC);
    
    // Construir el NFA
    NfaBuild builder;
    NFA nfa = builder.buildFromRegex(*exprABC);
    
    // Probar
    testNFA(nfa, {"c", "ac", "bc", "abc", "ababababc"}, {"", "a", "b", "ab", "abab"});
}

int main() {
    std::cout << "Iniciando pruebas de construcción de NFA...\n";
    
    testLiteralMatch();
    testKleeneStar();
    testPlus();
    testQuestionMark();
    testUnion();
    testCharSet();
    testNegatedCharSet();
    testWildcard();
    testComplexExpression();
    
    std::cout << "\nTodas las pruebas pasaron correctamente!\n";
    return 0;
}