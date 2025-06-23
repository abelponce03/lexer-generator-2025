#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "../regex/regex_parser_driver.hpp"
#include "../automata/build_nfa.hpp"
#include "../automata/lexer_nfa.hpp"
#include "../automata/lexer_dfa.hpp"

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
        std::cout << i + 1 << ". " << tokens[i].kind << " ('" << tokens[i].text << "') en posicion " << tokens[i].position << std::endl;
    }
}

// Función para crear un lexer a partir de reglas
std::shared_ptr<LexerDFA> crearLexer(const std::vector<TokenRule>& rules) {
    std::cout << "Creando lexer con " << rules.size() << " reglas..." << std::endl;
    
    RegexParserDriver driver;
    NfaBuild builder;
    LexerNFA lexerNfa;

    // Para cada regla, crear un NFA y añadirlo al lexer
    for (const auto& rule : rules) {
        std::cout << "Procesando regla: " << rule.name << " con patron: " << rule.pattern << std::endl;
        
        // Parsear la expresión regular
        auto expr = driver.parse(rule.pattern);
        if (!expr) {
            std::cerr << "Error al parsear la expresion regular: " << rule.pattern << std::endl;
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

// Verifica que los tipos de tokens sean los esperados
void verificarTokens(const std::vector<Lexeme>& tokens, const std::vector<std::string>& expectedTypes) {
    assert(tokens.size() == expectedTypes.size());
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].kind != expectedTypes[i]) {
            std::cerr << "Error en token " << i + 1 << ": esperaba '" 
                      << expectedTypes[i] << "' pero encontro '" << tokens[i].kind << "'" << std::endl;
            assert(false);
        }
    }
    
    std::cout << "Tipos de tokens verificados correctamente" << std::endl;
}

// Prueba 1: Caso de colisión
void pruebaCasoColision() {
    std::cout << "\n=== Prueba de Caso de Colision ===\n" << std::endl;
    
    std::vector<TokenRule> rules = {
        TokenRule("1", "a"),
        TokenRule("2", "a*b")
    };
    
    auto lexer = crearLexer(rules);
    
    // String con muchas 'a's
    std::string input = "aaaaaaaaaaaaaaaaaaaaaaa";
    std::cout << "Analizando: \"" << input << "\"" << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
    
    // Verificar que todos los tokens sean del tipo "1"
    std::vector<std::string> expectedTypes(input.length(), "1");
    verificarTokens(tokens, expectedTypes);
}

// Prueba 2: Análisis de una línea HULK simple
void pruebaLineaHulkSimple() {
    std::cout << "\n=== Prueba de Linea HULK Simple ===\n" << std::endl;
    
    std::vector<TokenRule> rules = {
        TokenRule("EQUAL", "="),
        TokenRule("LET", "let"),
        TokenRule("IN", "in"),
        TokenRule("SEMICOLON", ";"),
        TokenRule("COLON", ":"),
        TokenRule("NUMBER", "[0-9]+"),
        TokenRule("IDENTIFIER", "(_|[a-zA-Z])(_|[a-z0-9A-Z])*"),
        TokenRule("WHITESPACE", "[ \\t\\n\\r]+", true)
    };
    
    auto lexer = crearLexer(rules);
    
    std::string input = "let candela: Number = 69 in x;";
    std::cout << "Analizando: \"" << input << "\"" << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
    
    // Verificar los tipos de tokens
    std::vector<std::string> expectedTypes = {
        "LET", "IDENTIFIER", "COLON", "IDENTIFIER", "EQUAL", 
        "NUMBER", "IN", "IDENTIFIER", "SEMICOLON"
    };
    verificarTokens(tokens, expectedTypes);
}

// Prueba 3: Análisis de una línea HULK más compleja
void pruebaLineaHulkCompleja() {
    std::cout << "\n=== Prueba de Linea HULK Compleja ===\n" << std::endl;
    
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
        TokenRule("WHITESPACE", "[ \\t\\n\\r]+", true)
    };
    
    auto lexer = crearLexer(rules);
    
    std::string input = "function function_sexo (a:Number, b:Number): Number => a + b + 69;";
    std::cout << "Analizando: \"" << input << "\"" << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
    
    // Verificar los tipos de tokens
    std::vector<std::string> expectedTypes = {
        "FUNCTION", "IDENTIFIER", "LPAREN", "IDENTIFIER", "COLON", "IDENTIFIER",
        "COMMA", "IDENTIFIER", "COLON", "IDENTIFIER", "RPAREN", "COLON", "IDENTIFIER",
        "ARROW", "IDENTIFIER", "PLUS", "IDENTIFIER", "PLUS", "NUMBER", "SEMICOLON"
    };
    verificarTokens(tokens, expectedTypes);
}

// Prueba 4: Manejo de error léxico
void pruebaErrorLexico() {
    std::cout << "\n=== Prueba de Error Lexico ===\n" << std::endl;
    
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
        TokenRule("WHITESPACE", "[ \\t\\n\\r]+", true)
    };
    
    auto lexer = crearLexer(rules);
    
    std::string input = "function a() => b # c;";
    std::cout << "Analizando: \"" << input << "\"" << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
    
    // Debería detectarse un error en el carácter '#'
    std::cout << "Nota: Se esperaba un error lexico para el caracter '#'" << std::endl;
}

// Prueba 5: Múltiples errores léxicos
void pruebaMultiplesErrores() {
    std::cout << "\n=== Prueba de Multiples Errores Lexicos ===\n" << std::endl;
    
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
        TokenRule("IDENTIFIER", "(_|[a-zA-Z])(_|[a-z0-9A-Z])*"),
        TokenRule("WHITESPACE", "[ \\t\\n\\r]+", true)
    };
    
    auto lexer = crearLexer(rules);
    
    std::string input = 
        "function 2a () => b $ c;\n"
        "function testicol () => d + e;\n"
        "function minus() => f - g;";
    
    std::cout << "Analizando entrada con multiples errores:" << std::endl;
    std::cout << input << std::endl;
    
    auto tokens = lexer->scan(input);
    imprimirTokens(tokens);
    
    // Deberían detectarse errores en los caracteres '2', '$' y '-'
    std::cout << "Nota: Se esperaban errores lexicos para los caracteres '2', '$' y '-'" << std::endl;
}

int main() {
    std::cout << "== Pruebas Completas del Lexer ==" << std::endl;
    
    try {
        pruebaCasoColision();
        pruebaLineaHulkSimple();
        pruebaLineaHulkCompleja();
        pruebaErrorLexico();
        pruebaMultiplesErrores();
        
        std::cout << "\n Todas las pruebas completadas." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\n Error durante las pruebas: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n Error desconocido durante las pruebas." << std::endl;
        return 1;
    }
    
    return 0;
}