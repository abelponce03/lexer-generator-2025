#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "../../regex/regex_parser_driver.hpp"
#include "../bob_construye_nfa.hpp"
#include "../lexer_nfa.hpp"
#include "../lexer_dfa.hpp"

// Estructura para representar una regla de token
struct TokenRule {
    std::string name;
    std::string pattern;
    bool ignorable;

    TokenRule(const std::string& n, const std::string& p, bool i = false) 
        : name(n), pattern(p), ignorable(i) {}
};

// Función para imprimir los tokens generados
void imprimirTokens(const std::vector<Lexeme>& tokens) {
    std::cout << "Tokens generados (" << tokens.size() << "):" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << i + 1 << ". " << tokens[i].kind << " ('" << tokens[i].text << "') en posición " << tokens[i].position << std::endl;
    }
}

// Función para crear un lexer a partir de reglas
std::shared_ptr<LexerDFA> crearLexer(const std::vector<TokenRule>& rules) {
    RegexParserDriver driver;
    NfaBuild builder;
    LexerNFA lexerNfa;

    // Para cada regla, crear un NFA y añadirlo al lexer
    for (const auto& rule : rules) {
        std::cout << "Procesando regla: " << rule.name << " con patrón: " << rule.pattern << std::endl;
        
        // Parsear la expresión regular
        auto expr = driver.parse(rule.pattern);
        if (!expr) {
            std::cerr << "Error al parsear la expresión regular: " << rule.pattern << std::endl;
            continue;
        }
        
        // Construir el NFA para esta expresión
        auto nfa = builder.buildFromExpression(expr);
        
        // Añadir este NFA al lexer
        lexerNfa.addPattern(rule.name, nfa, rule.ignorable);
    }
    
    // Construir el lexer NFA y convertirlo a DFA
    return lexerNfa.toDFA();
}

// Prueba de expresiones simples (identificadores, números, operadores)
void pruebaLexerSimple() {
    std::cout << "\n=== Prueba de Lexer Simple ===\n" << std::endl;
    
    std::vector<TokenRule> rules = {
        TokenRule("NUMERO", "[0-9]+"),
        TokenRule("ID", "[a-zA-Z][a-zA-Z0-9_]*"),
        TokenRule("OPERADOR", "[\\+\\-\\*/]"),
        TokenRule("ESPACIO", "( |\\t|\\n|\\r)+", true)
    };
    
    auto lexer = crearLexer(rules);
    
    // Probar con una cadena simple
    std::string input = "abc 123 + xyz";
    std::cout << "Analizando: \"" << input << "\"" << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
}

// Prueba de un lenguaje más complejo (como el ejemplo de HULK)
void pruebaLexerHulk() {
    std::cout << "\n=== Prueba de Lexer HULK ===\n" << std::endl;
    
    std::vector<TokenRule> rules = {
        TokenRule("EQUAL", "="),
        TokenRule("FUNCTION", "function"),
        TokenRule("ARROW", "=>"),
        TokenRule("SEMICOLON", ";"),
        TokenRule("COLON", ":"),
        TokenRule("PLUS", "\\+"),
        TokenRule("LPAREN", "\\("),
        TokenRule("RPAREN", "\\)"),
        TokenRule("COMMA", ","),
        TokenRule("NUMBER", "[0-9]+"),
        TokenRule("IDENTIFIER", "(_|[a-zA-Z])(_|[a-z0-9A-Z])*"),
        TokenRule("WHITESPACE", "( |\\t|\\n|\\r)+", true)
    };
    
    auto lexer = crearLexer(rules);
    
    // Probar con una cadena de HULK
    std::string input = "function suma(a:Number, b:Number) => a + b;";
    std::cout << "Analizando: \"" << input << "\"" << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
}

// Prueba de manejo de errores
void pruebaLexerConError() {
    std::cout << "\n=== Prueba de Lexer con Error ===\n" << std::endl;
    
    std::vector<TokenRule> rules = {
        TokenRule("EQUAL", "="),
        TokenRule("FUNCTION", "function"),
        TokenRule("ARROW", "=>"),
        TokenRule("SEMICOLON", ";"),
        TokenRule("IDENTIFIER", "(_|[a-zA-Z])(_|[a-z0-9A-Z])*"),
        TokenRule("WHITESPACE", "( |\\t|\\n|\\r)+", true)
    };
    
    auto lexer = crearLexer(rules);
    
    // Probar con una cadena que contiene un carácter no reconocible
    std::string input = "function test() => value # error;";
    std::cout << "Analizando: \"" << input << "\"" << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
}

int main() {
    std::cout << "== Pruebas del Lexer Completo ==" << std::endl;
    
    pruebaLexerSimple();
    pruebaLexerHulk();
    pruebaLexerConError();
    
    std::cout << "\nPruebas finalizadas." << std::endl;
    return 0;
}