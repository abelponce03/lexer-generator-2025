#ifndef UN_OP_HPP
#define UN_OP_HPP

#include <memory>
#include <string>
#include <stdexcept>

// Forward declaration
class Expression;

/// Representa un operador unario en un AST de expresión regular.
enum class UnaryOperator {
    /// Operador de estrella de Kleene (cero o más repeticiones)
    KleeneStar,
    /// Operador de más (una o más repeticiones)
    Plus,
    /// Operador de interrogación (cero o una ocurrencia)
    QuestionMark
};

/// Representa un nodo de operación unaria en un árbol de sintaxis abstracta de expresión regular.
class UnOp {
public:
    /// La expresión a la que se aplica el operador unario
    std::shared_ptr<Expression> operand;
    /// El operador unario
    UnaryOperator op;

    /// Constructor que inicializa una operación unaria
    UnOp(std::shared_ptr<Expression> operand, UnaryOperator op)
        : operand(operand), op(op) {
        if (!operand) {
            throw std::invalid_argument("UnOp: operand cannot be null");
        }
    }

    /// Convierte la operación unaria a una representación de string
    std::string toString() const;
};

#endif // UN_OP_HPP