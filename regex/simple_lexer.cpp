#include "regex_parser.tab.hpp"
#include "regex_parser_driver.hpp"
#include <string>
#include <iostream>

// La cadena que estamos analizando
static std::string input_string;
static size_t current_pos = 0;
static bool in_char_class = false; // Flag para detectar si estamos dentro de []

// Inicializar el lexer con una cadena
void lexer_init(const std::string& input) {
    input_string = input;
    current_pos = 0;
    in_char_class = false;
    
    // Imprimir la entrada con caracteres especiales visibles para depuración
    std::string debug_input;
    for (char c : input) {
        if (c == '\t') debug_input += "\\t";
        else if (c == '\n') debug_input += "\\n";
        else debug_input += c;
    }
    std::cout << "Inicializando lexer con entrada: '" << debug_input << "'" << std::endl;
}

// Implementación simplificada de yylex
yy::parser::symbol_type yylex(RegexParserDriver& driver) {
    // Saltar espacios en blanco
    while (current_pos < input_string.length() && 
           (input_string[current_pos] == ' ' || 
            input_string[current_pos] == '\t' || 
            input_string[current_pos] == '\n' || 
            input_string[current_pos] == '\r')) {
        current_pos++;
        driver.increaseLocation(1);
    }
    
    // Verificar fin de entrada
    if (current_pos >= input_string.length()) {
        return yy::parser::make_END(driver.location);
    }
    
    // Obtener carácter actual
    char c = input_string[current_pos++];
    
    // Actualizar ubicación
    driver.increaseLocation(1);
    
    // Retornar token según el carácter
    switch (c) {
        case '(': return yy::parser::make_LPAREN(driver.location);
        case ')': return yy::parser::make_RPAREN(driver.location);
        
        case '[': 
            in_char_class = true;
            return yy::parser::make_LBRACKET(driver.location);
            
        case ']': 
            in_char_class = false;
            return yy::parser::make_RBRACKET(driver.location);
            
        case '-': 
            // Si estamos dentro de una clase de caracteres, - es un rango
            return yy::parser::make_DASH(driver.location);
            
        case '|': return yy::parser::make_PIPE(driver.location);
        
        // Operadores especiales - interpretarlos como CHAR si están dentro de []
        case '*': 
            if (in_char_class) return yy::parser::make_CHAR('*', driver.location);
            else return yy::parser::make_STAR(driver.location);
            
        case '+': 
            if (in_char_class) return yy::parser::make_CHAR('+', driver.location);
            else return yy::parser::make_PLUS(driver.location);
            
        case '?': 
            if (in_char_class) return yy::parser::make_CHAR('?', driver.location);
            else return yy::parser::make_QUESTION(driver.location);
            
        case '.': 
            if (in_char_class) return yy::parser::make_CHAR('.', driver.location);
            else return yy::parser::make_DOT(driver.location);
            
        case '^': 
            // ^ al principio de una clase es negación, en cualquier otro lugar es un carácter
            if (in_char_class && current_pos - 2 >= 0 && 
                input_string[current_pos - 2] == '[') {
                return yy::parser::make_CARET(driver.location);
            } else {
                return yy::parser::make_CHAR('^', driver.location);
            }
            
        case '\\':
            // Manejo de caracteres escapados
            if (current_pos < input_string.length()) {
                char next = input_string[current_pos++];
                driver.increaseLocation(1);
                
                switch (next) {
                    case 'n': return yy::parser::make_CHAR('\n', driver.location);
                    case 't': return yy::parser::make_CHAR('\t', driver.location);
                    case 'r': return yy::parser::make_CHAR('\r', driver.location);
                    case 'f': return yy::parser::make_CHAR('\f', driver.location);
                    case 's': 
                        // Implementación mejorada de \s para que represente cualquier espacio en blanco
                        if (in_char_class) {
                            // Incluso dentro de una clase de caracteres, \s debe representar
                            // todos los caracteres de espacio en blanco
                            return yy::parser::make_CHAR(' ', driver.location);
                        } else {
                            // Aquí deberíamos manejar \s como un carácter especial que
                            // representa la clase de todos los espacios en blanco
                            return yy::parser::make_CHAR(' ', driver.location);
                        }
                    default: return yy::parser::make_CHAR(next, driver.location);
                }
            }
            // Si no hay siguiente carácter, tratar como carácter normal
            return yy::parser::make_CHAR('\\', driver.location);
            
        default:
            // Cualquier otro carácter
            return yy::parser::make_CHAR(c, driver.location);
    }
}

// Implementación de yylex_destroy para el lexer manual
extern "C" int yylex_destroy(void) {
    // No necesitamos hacer nada especial para nuestro lexer manual
    input_string = "";
    current_pos = 0;
    in_char_class = false;
    return 0;
}

