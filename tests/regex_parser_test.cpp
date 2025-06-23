#include <iostream>
#include <string>
#include "regex_parser_driver.hpp"

void testRegexParser(const std::string& regex) {
    RegexParserDriver driver;
    
    try {
        auto result = driver.parse(regex);
        
        if (result) {
            std::cout << "Regex: \"" << regex << "\" -> AST: " << result->toString() << std::endl;
        } else {
            std::cout << "Error al parsear: \"" << regex << "\"" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Excepción al procesar \"" << regex << "\": " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Error desconocido al procesar \"" << regex << "\"" << std::endl;
    }
    
    // Agregar una línea de separación para mayor claridad
    std::cout << "----------------------------------------" << std::endl;
}

int main() {
    std::cout << "== Probando el parser de expresiones regulares ==" << std::endl;
    
    // Expresiones simples
    testRegexParser("a");
    testRegexParser("ab");
    testRegexParser("a|b");
    testRegexParser("a*");
    testRegexParser("a+");
    testRegexParser("a?");
    
    // Rangos y clases de caracteres
    testRegexParser("[a-z]");
    testRegexParser("[^0-9]");
    testRegexParser("[a-zA-Z0-9]");
    
    // Intentar cada expresión compleja por separado para aislar el problema
    testRegexParser("(a)");
    testRegexParser("(a|b)");
    testRegexParser("(a|b)*");
    testRegexParser("c");
    testRegexParser("(a|b)*c");
    
    // Más expresiones complejas si las anteriores funcionan
    testRegexParser("a(b|c)+d");
    testRegexParser("(ab)*|cd?");
    
    // Expresiones con secuencias de escape
    testRegexParser("\\.");
    testRegexParser("a\\.b");
    testRegexParser("[\\^\\-]");
    
    // Expresiones con combinaciones de operadores
    testRegexParser("(a|b|c)+");
    testRegexParser("a?b*c+");
    testRegexParser("(a?b*)+");
    
    // Expresiones con múltiples niveles de anidamiento
    testRegexParser("((a|b)(c|d))*");
    testRegexParser("(a|(b|c))+d");
    testRegexParser("a(b(c|d)*e)+f");
    
    // Expresiones con patrones comunes
    testRegexParser("[a-zA-Z][a-zA-Z0-9_]*");  // Identificador
    testRegexParser("[+-]?[0-9]+(\\.[0-9]+)?");  // Número decimal
    testRegexParser("\"[^\"]*\"");  // Cadena entre comillas
    
    // Expresiones muy complejas
    testRegexParser("(a|b)+(c|d)*(e|f)?g");
    testRegexParser("(a|[b-d]|[^x-z])+");
    
    return 0;
}