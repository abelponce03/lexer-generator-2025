#ifndef REGEX_PARSER_DRIVER_HPP
#define REGEX_PARSER_DRIVER_HPP

#include <string>
#include <memory>
#include <vector>
#include "../ast/expression.hpp"
#include "regex_parser.tab.hpp"

// Declaración de la función de inicialización del lexer
void lexer_init(const std::string& input);

// Declaración para hacer referencia al scanner
#define YY_DECL yy::parser::symbol_type yylex(RegexParserDriver& driver)
YY_DECL;

class RegexParserDriver {
public:
    // Constructor y métodos de acceso
    RegexParserDriver();
    virtual ~RegexParserDriver();

    // Analiza la expresión regular proporcionada
    std::shared_ptr<Expression> parse(const std::string& input);
    
    // Gestión de errores
    void error(const yy::location& loc, const std::string& message);
    void error(const std::string& message);
    
    // Ubicación para seguimiento de errores
    void increaseLocation(int count);
    yy::location location;
    
    // Resultado del análisis
    std::shared_ptr<Expression> result;
    
    // Indicador de depuración
    bool trace_scanning;
    bool trace_parsing;

private:
    // Para almacenar los mensajes de error
    std::vector<std::string> errors;
    
    // Para el análisis de escaneo
    void scan_begin();
    void scan_end();
    
    // Para el análisis sintáctico
    void parse_begin();
    void parse_end();
    
    // El nombre del archivo siendo procesado
    std::string file;
};

#endif // REGEX_PARSER_DRIVER_HPP