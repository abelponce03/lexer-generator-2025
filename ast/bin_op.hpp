#ifndef BIN_OP_HPP
#define BIN_OP_HPP

#include <memory>
#include <string>
#include <stdexcept>

// Forward declaration
class Expression;

/// Representa un operador binario en un AST de expresión regular.
enum class BinaryOperator {
    /// Operador de concatenación
    Concat,
    /// Operador de unión (alternación)
    Union
};

/// Representa un nodo de operación binaria en un árbol de sintaxis abstracta de expresión regular.
class BinOp {
public:
    /// La expresión del lado izquierdo
    std::shared_ptr<Expression> left;
    /// La expresión del lado derecho
    std::shared_ptr<Expression> right;
    /// El operador binario aplicado entre las dos expresiones
    BinaryOperator op;

    /// Constructor que inicializa una operación binaria
    BinOp(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, BinaryOperator op)
        : left(left), right(right), op(op) {
        if (!left || !right) {
            throw std::invalid_argument("BinOp: operands cannot be null");
        }
    }

    /// Convierte la operación binaria a una representación de string
    std::string toString() const;
};

#endif // BIN_OP_HPP