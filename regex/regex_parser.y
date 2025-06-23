%require "3.2"
%language "c++"
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%locations

%code requires {
    #include <string>
    #include <memory>
    #include "../ast/expression.hpp"
    #include "../ast/bin_op.hpp"
    #include "../ast/un_op.hpp"
    #include "../ast/atoms/regex_atom.hpp"
    #include "../ast/atoms/atom_set.hpp"
    #include "../ast/atoms/charset.hpp"
    #include "../ast/atoms/matchable_atom.hpp"
    
    class RegexParserDriver;
}

%param { RegexParserDriver& driver }

%code {
    #include "regex_parser_driver.hpp"
    
    // Declaración del lexer para Bison - Cambiamos regex_parser por parser
    yy::parser::symbol_type yylex(RegexParserDriver& driver);
}

// Definición de tokens
%token <char> CHAR 
%token LPAREN "(" RPAREN ")" LBRACKET "[" RBRACKET "]"
%token DASH "-" PIPE "|" STAR "*" PLUS "+" QUESTION "?" DOT "." CARET "^"
%token END 0 "end of file"

// Definición de tipos para los no-terminales
%type <std::shared_ptr<Expression>> regex union_expr concat_expr unary_expr atom
%type <std::vector<std::pair<char, char>>> range_elements range_element
%type <bool> negation

// Precedencia y asociatividad
%left "|"
%left CONCAT
%left "*" "+" "?"

%%

%start regex;

regex:
    union_expr { driver.result = $1; }
  ;

union_expr:
    concat_expr                     { $$ = $1; }
  | union_expr "|" concat_expr      { 
        auto binOp = std::make_shared<BinOp>($1, $3, BinaryOperator::Union);
        $$ = std::make_shared<Expression>(binOp);
    }
  ;

concat_expr:
    unary_expr                      { $$ = $1; }
  | concat_expr unary_expr %prec CONCAT {
        auto binOp = std::make_shared<BinOp>($1, $2, BinaryOperator::Concat);
        $$ = std::make_shared<Expression>(binOp);
    }
  ;

unary_expr:
    atom                            { $$ = $1; }
  | unary_expr "*"                  {
        auto unOp = std::make_shared<UnOp>($1, UnaryOperator::KleeneStar);
        $$ = std::make_shared<Expression>(unOp);
    }
  | unary_expr "+"                  {
        auto unOp = std::make_shared<UnOp>($1, UnaryOperator::Plus);
        $$ = std::make_shared<Expression>(unOp);
    }
  | unary_expr "?"                  {
        auto unOp = std::make_shared<UnOp>($1, UnaryOperator::QuestionMark);
        $$ = std::make_shared<Expression>(unOp);
    }
  ;

atom:
    CHAR                            { 
        RegexAtom atom = RegexAtom::Char($1);
        MatchableAtom matchable(atom);
        $$ = std::make_shared<Expression>(matchable);
    }
  | "."                             {
        AtomSet wildcard = AtomSet::Wildcard();
        MatchableAtom matchable(wildcard);
        $$ = std::make_shared<Expression>(matchable);
    }
  | "(" union_expr ")"              { $$ = $2; }  // Cambiado de regex a union_expr
  | "[" negation range_elements "]" {
        CharSet charset($3, $2);
        AtomSet atomSet = AtomSet::fromCharSet(charset);
        MatchableAtom matchable(atomSet);
        $$ = std::make_shared<Expression>(matchable);
    }
  ;

negation:
    /* empty */                     { $$ = false; }
  | "^"                             { $$ = true; }
  ;

range_elements:
    range_element                   { $$ = $1; }
  | range_elements range_element    { 
        $$ = $1;
        $$.insert($$.end(), $2.begin(), $2.end());
    }
  ;

range_element:
    CHAR                            { 
        $$ = std::vector<std::pair<char, char>>();
        $$.push_back(std::make_pair($1, $1));
    }
  | CHAR "-" CHAR                   {
        $$ = std::vector<std::pair<char, char>>();
        // Asegurar que el rango esté ordenado correctamente (inicio <= fin)
        if ($1 <= $3) {
            $$.push_back(std::make_pair($1, $3));
        } else {
            $$.push_back(std::make_pair($3, $1));
        }
    }
  ;

%%

// Corregimos el nombre de la clase a parser en lugar de regex_parser
// y usamos yy::location en lugar de location_type
void yy::parser::error(const yy::location& l, const std::string& m) {
    driver.error(l, m);
}