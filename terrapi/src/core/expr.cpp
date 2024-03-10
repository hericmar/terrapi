// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include "expr.h"

#include <cstring>
#include <ios>
#include <stack>

#include "core/context.h"
#include "core/sensor.h"
#include "logger.h"
#include "utils.h"

namespace terra
{
int Token::op_precedence() const
{
    switch (id)
    {
    case TokenID::LParen:
        return 0;
    case TokenID::LessThan:
        return 2;
    default:
        return 1;
    }
}

std::vector<Token> Tokenizer::parse()
{
    std::vector<Token> result;

    Token token{};
    do {
        token = next();
        result.push_back(token);
    } while (token.id != TokenID::End);

    return result;
}

Token Tokenizer::next()
{
    m_ss >> std::skipws;
    std::istream::sentry sentry(m_ss);
    if (!sentry)
        return Token{TokenID::End};

    auto c = m_ss.peek();
    Token result;
    bool consume = true;

    if (std::isdigit(c)) {
        float number  = 0;

        m_ss >> number;
        {
            char next = m_ss.peek();
            if (next == ':') {
                m_ss.get();

                float min;
                m_ss >> min;
                if (!m_ss.fail()) {
                    std::tm tm{};
                    tm.tm_hour = (int) number;
                    tm.tm_min = min;

                    number = (float) to_seconds(tm);
                }
            } else {
                m_ss.clear(m_ss.rdstate() & ~std::ios::failbit);
            }
        }
        result = Token{TokenID::Number, number};
        consume = false;

    } else if (is_str_literal(c)) {
        std::string ident;
        m_ss.ignore();
        do {
            ident.push_back(c);
            c = m_ss.get();
        } while (is_str_literal(c));
        if (c != std::istream::traits_type::eof())
            m_ss.putback(c);
        // clearly retarded API (pun intended): clear does not clear the bits,
        // but sets them
        m_ss.clear(m_ss.rdstate() & ~std::ios::failbit);
        result = Token(TokenID::Identifier, std::move(ident));
        consume = false;
    } else {
        if (std::strchr(SYMBOL_TOKENS, c) == nullptr) {
            std::stringstream ss;
            ss << "unknown token " << static_cast<char>(c);
            throw tokenize_error(ss.str());
        }
        result = Token(static_cast<TokenID>(c));
    }
    if (m_ss.fail())
        throw tokenize_error("failed conversion");
    if (consume)
        m_ss.get();

    return result;
}

static void create_operator(std::vector<Expr>& expr_queue, Token token);

//----------------------------------------------------------------------------//

/// @throws parse_error
Expr expr_from_impl(const SensorMap& sensors, const std::string& str)
{
    Tokenizer t{str};

    const auto tokens = t.parse();

    std::stack<Token> operators = std::stack<Token>();
    std::vector<Expr> exprs_queue;

    for (const auto& token : tokens) {
        switch (token.id)
        {
        case TokenID::End: {
            break;
        }

        case TokenID::Number: {
            exprs_queue.push_back(number(token.number));
            break;
        }
        case TokenID::Identifier: {
            exprs_queue.push_back(variable(sensors, token.identifier));
            break;
        }
        case TokenID::LParen: {
            operators.push(token);
            break;
        }
        case TokenID::LessThan:
        case TokenID::And:
        case TokenID::Or:
        case TokenID::Equal: {
            Token popped_token;

            while (!operators.empty()) {
                popped_token = operators.top();
                if (popped_token.op_precedence() > token.op_precedence()) {
                    create_operator(exprs_queue, popped_token);
                    operators.pop();
                } else {
                    break;
                }
            }
            operators.push(token);
            break;
        }
        case TokenID::RParen: {
            /** while the operator at the top of the operator stack is not a left parenthesis:
                pop the operator from the operator stack onto the output queue.
                If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
                if there is a left parenthesis at the top of the operator stack, then:
                pop the operator from the operator stack and discard it */
            Token popped_token;

            while (!operators.empty()) {
                popped_token = operators.top();
                operators.pop();

                if (popped_token.id == TokenID::LParen)
                    break;
                else
                    create_operator(exprs_queue, popped_token);
            }

            break;
        }
        }
    }

    /** while there are still operator tokens on the stack:
        If the operator token on the top of the stack is a parenthesis, then there are mismatched parentheses.
    pop the operator from the operator stack onto the output queue. */
    Token popped_token;
    while (!operators.empty()) {
        popped_token = operators.top();
        operators.pop();
        create_operator(exprs_queue, popped_token);
    }
    if (!exprs_queue.empty())
        return exprs_queue.back();

    throw parse_error("! parse error ");
}

std::optional<Expr> Expr::from(const SensorMap& sensors, const std::string& str)
{
    try {
        return expr_from_impl(sensors, str);
    } catch (std::exception& ex) {
        LOG(ERR, ex.what());

        return std::nullopt;
    }
}

Expr number(float num)
{
    return std::make_shared<Num>(num);
}

Expr variable(const SensorMap& sensors, const std::string& identifier)
{
    auto name_pq_pair = split(identifier, ".");
    if (identifier == "time") {
        name_pq_pair = { "time", "time" };
    }

    if (name_pq_pair.size() != 2) {
        throw parse_error("invalid format: expected sensor_name.physical_quantity");
    }

    const auto& sensor_name = name_pq_pair[0];

    if (value_type_to_str.count(name_pq_pair[1]) == 0) {
        throw parse_error("invalid format: invalid physical quantity name");
    }

    const auto value_type = value_type_to_str.at(name_pq_pair[1]);

    if (sensors.count(sensor_name) == 0) {
        throw parse_error("no such sensor");
    }

    auto& sensor = *sensors.at(sensor_name);

    if (!sensor.measures_value(value_type)) {
        throw parse_error("invalid rule: sensor does not measure given physical quantity");
    }

    return std::make_shared<Var>(sensor, value_type);
}

Expr operator<(Expr a, Expr b)
{
    return std::make_shared<LessThan>(a, b);
}

Expr operator|(Expr a, Expr b)
{
    return std::make_shared<Or>(a, b);
}

Expr operator&(Expr a, Expr b)
{
    return std::make_shared<And>(a, b);
}

Expr operator==(Expr a, Expr b)
{
    return std::make_shared<Equal>(a, b);
}

//------------------------------------------------------------------------------

float Var::get_value() const
{
    return sensor.value(value_type);
}

//------------------------------------------------------------------------------

static void create_operator(std::vector<Expr>& expr_queue, Token token)
{
    Expr lhs;
    Expr rhs;

    if (expr_queue.size() < 2)
        throw parse_error("! expr queue is empty ");

    switch (token.id) {
    case TokenID::LessThan: {
        rhs = expr_queue.back(); expr_queue.pop_back();
        lhs = expr_queue.back(); expr_queue.pop_back();
        expr_queue.emplace_back(lhs < rhs);
        break;
    }
    case TokenID::And: {
        rhs = expr_queue.back(); expr_queue.pop_back();
        lhs = expr_queue.back(); expr_queue.pop_back();
        expr_queue.emplace_back(lhs & rhs);
        break;
    }
    case TokenID::Or: {
        rhs = expr_queue.back(); expr_queue.pop_back();
        lhs = expr_queue.back(); expr_queue.pop_back();
        expr_queue.emplace_back(lhs | rhs);
        break;
    }
    case TokenID::Equal: {
        rhs = expr_queue.back(); expr_queue.pop_back();
        lhs = expr_queue.back(); expr_queue.pop_back();
        expr_queue.emplace_back(lhs == rhs);
    }
    default:
        break;
    }
}
}

