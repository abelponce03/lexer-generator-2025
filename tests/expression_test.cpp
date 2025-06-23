#include <iostream>
#include <memory>
#include <cassert>
#include <string>
#include "../../ast/expression.hpp"
#include "../../ast/bin_op.hpp"
#include "../../ast/un_op.hpp"
#include "../../ast/atoms/regex_atom.hpp"
#include "../../ast/atoms/atom_set.hpp"
#include "../../ast/atoms/charset.hpp"
#include "../../ast/atoms/matchable_atom.hpp"

// Función de utilidad para hacer assert sobre cadenas
void assert_equal(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        std::cerr << "Error: esperaba '" << expected << "' pero obtuvo '" << actual << "'" << std::endl;
        assert(false);
    }
}

void test_display_atom() {
    std::cout << "Prueba: display_atom" << std::endl;
    
    MatchableAtom atom(RegexAtom::Char('a'));
    Expression expr(atom);
    
    assert_equal(expr.toString(), "a");
}

void test_display_binop_concat() {
    std::cout << "Prueba: display_binop_concat" << std::endl;
    
    auto left = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('a')));
    auto right = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('b')));
    
    auto binop = std::make_shared<BinOp>(left, right, BinaryOperator::Concat);
    Expression expr(binop);
    
    assert_equal(expr.toString(), "(ab)");
}

void test_display_binop_union() {
    std::cout << "Prueba: display_binop_union" << std::endl;
    
    auto left = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('a')));
    auto right = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('b')));
    
    auto binop = std::make_shared<BinOp>(left, right, BinaryOperator::Union);
    Expression expr(binop);
    
    assert_equal(expr.toString(), "(a|b)");
}

void test_display_unop_kleene() {
    std::cout << "Prueba: display_unop_kleene" << std::endl;
    
    auto operand = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('a')));
    auto unop = std::make_shared<UnOp>(operand, UnaryOperator::KleeneStar);
    Expression expr(unop);
    
    assert_equal(expr.toString(), "a*");
}

void test_display_unop_plus() {
    std::cout << "Prueba: display_unop_plus" << std::endl;
    
    auto operand = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('b')));
    auto unop = std::make_shared<UnOp>(operand, UnaryOperator::Plus);
    Expression expr(unop);
    
    assert_equal(expr.toString(), "b+");
}

void test_display_unop_question() {
    std::cout << "Prueba: display_unop_question" << std::endl;
    
    auto operand = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('c')));
    auto unop = std::make_shared<UnOp>(operand, UnaryOperator::QuestionMark);
    Expression expr(unop);
    
    assert_equal(expr.toString(), "c?");
}

void test_nested_expression_display() {
    std::cout << "Prueba: nested_expression_display" << std::endl;
    
    auto left = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('a')));
    auto right = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('b')));
    
    auto binop = std::make_shared<BinOp>(left, right, BinaryOperator::Union);
    auto union_expr = std::make_shared<Expression>(binop);
    
    auto unop = std::make_shared<UnOp>(union_expr, UnaryOperator::KleeneStar);
    Expression star_expr(unop);
    
    assert_equal(star_expr.toString(), "(a|b)*");
}

void test_another_nested_expression_display() {
    std::cout << "Prueba: another_nested_expression_display" << std::endl;
    
    auto a_expr = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('a')));
    auto b_expr = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('b')));
    
    auto binop = std::make_shared<BinOp>(a_expr, b_expr, BinaryOperator::Union);
    auto union_expr = std::make_shared<Expression>(binop);
    
    auto unop = std::make_shared<UnOp>(union_expr, UnaryOperator::KleeneStar);
    auto kleene_expr = std::make_shared<Expression>(unop);
    
    auto c_expr = std::make_shared<Expression>(MatchableAtom(RegexAtom::Char('c')));
    
    auto concat = std::make_shared<BinOp>(kleene_expr, c_expr, BinaryOperator::Concat);
    Expression final_expr(concat);
    
    assert_equal(final_expr.toString(), "((a|b)*c)");
}

int main() {
    std::cout << "== Pruebas de Expresiones ==" << std::endl;
    
    test_display_atom();
    test_display_binop_concat();
    test_display_binop_union();
    test_display_unop_kleene();
    test_display_unop_plus();
    test_display_unop_question();
    test_nested_expression_display(); 
    test_another_nested_expression_display();
    
    std::cout << "\nTodas las pruebas pasaron correctamente." << std::endl;
    return 0;
}