#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <sstream>

#include "terrapi/sensor.h"

namespace terra
{
enum class TokenID : char
{
    Number = '0',
    Identifier = 'a',
    LessThan = '<',
    And = '&',
    Or = '|',
    End = 0
};

#define SYMBOL_TOKENS "<&|"

struct Token
{
    TokenID id;

    float       number = 0;
    std::string identifier;

    Token() = default;
    explicit Token(TokenID id) : id(id) {}
    Token(TokenID id, float number) : id(id), number(number) {
        assert(id == TokenID::Number);
    }
    Token(TokenID id, std::string ident) : id(id), identifier(std::move(ident)) {
        assert(id == TokenID::Identifier);
    }

    int op_precedence() const;
};

struct tokenize_error : std::logic_error
{
    explicit tokenize_error(const std::string& str) : std::logic_error(str) {}
};

struct parse_error : std::logic_error
{
    explicit parse_error(const std::string& str) : std::logic_error(str) {}
};

struct Tokenizer
{
    explicit Tokenizer(const std::string& str) : m_ss(str) {}

    std::vector<Token> parse();

private:
    Token next();

    std::stringstream m_ss;
};

//------------------------------------------------------------------------------

struct ExprBase
{
    virtual bool evaluate() const = 0;
    virtual float get_value() const { return 0; }
};

struct Expr final
{
private:
    using const_pointer = std::shared_ptr<const ExprBase>;

public:
    Expr() = default;

    /**
     * Implicitly convertible from the internal shared ptr.
     *
     * This is a little black magic. Intuitively, this is the equivalent of
     *   expr(const_pointer e) : ptr(std::move(e)) {}
     * However, that would cause trouble when trying to implicitly convert e.g. from
     * shared_ptr<Number>, because that would require 2 user-defined conversions
     * (shared_ptr<Number> -> shared_ptr<const expr_base> -> expr), whereas C++
     * only allows one (see https://stackoverflow.com/questions/8610511/chaining-implicit-conversion-operators)
     *
     * This merges the 2 conversions into one, so you may write e.g.
     * `expr e = std::make_shared<Number>(42);`.
     */
    template <typename T, typename = std::enable_if_t<std::is_convertible<T*, const ExprBase*>::value>>
    Expr(std::shared_ptr<T> e) : m_ptr(std::static_pointer_cast<const ExprBase>(std::move(e))) {}

    // implicitly convertible to the internal shared ptr, implement operator->
    // that means you can call methods directly via `expr`, e.g. `e->simplify()`
    operator const_pointer const& () const
    {
        return m_ptr;
    }

    const ExprBase* operator->() const
    {
        assert(m_ptr.get() != nullptr);
        return m_ptr.get();
    }

private:
    const_pointer m_ptr;
};

//------------------------------------------------------------------------------

// builders
Expr number(float num);
Expr variable(const std::string& identifier);
Expr operator<(Expr a, Expr b);
Expr operator|(Expr a, Expr b);
Expr operator&(Expr a, Expr b);

//------------------------------------------------------------------------------

struct Num : public ExprBase
{
    explicit Num(float value) : m_value(value) {}

    bool evaluate() const override { return false; }
    float get_value() const override { return m_value; }

private:
    float m_value;
};

struct Var : public ExprBase
{
    /// \pre Sensor with \p sensor_name with \p physical_quantity must exists.
    Var(std::string sensor_name, EPhysicalQuantity physical_quantity)
        : m_sensor_name(std::move(sensor_name)), m_physical_quantity(physical_quantity) {}

    bool evaluate() const override { return false; }
    float get_value() const override;

private:
    std::string       m_sensor_name;
    EPhysicalQuantity m_physical_quantity;
};

struct LessThan : public ExprBase
{
    LessThan(Expr& lhs, Expr& rhs)
        : m_lhs(lhs), m_rhs(rhs) {}

    bool evaluate() const override
    {
        return m_lhs->get_value() < m_rhs->get_value();
    }

private:
    Expr m_lhs, m_rhs;
};

struct And : public ExprBase
{
    And(Expr& lhs, Expr& rhs)
        : m_lhs(lhs), m_rhs(rhs) {}

    bool evaluate() const override
    {
        return m_lhs->evaluate() && m_rhs->evaluate();
    }

private:
    Expr m_lhs, m_rhs;
};

struct Or : public ExprBase
{
    Or(Expr& lhs, Expr& rhs)
        : m_lhs(lhs), m_rhs(rhs) {}

    bool evaluate() const override
    {
        return m_lhs->evaluate() || m_rhs->evaluate();
    }

private:
    Expr m_lhs, m_rhs;
};

//------------------------------------------------------------------------------

Expr create_expr_tree(const std::string& expr);
}
