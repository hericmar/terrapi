#include <ios>
#include <stack>

#include "app.h"
#include "config.h"
#include "rule.h"
#include "chrono.h"

namespace terra
{
//------------------------------------------------------------------------------

int Token::op_precedence() const
{
    switch (id)
    {
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

                    number = (float) tm_to_seconds(tm);
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

//------------------------------------------------------------------------------

Expr number(float num)
{
    return std::make_shared<Num>(num);
}

Expr variable(const std::string& identifier)
{
    if (identifier == "water.signal") {
        int i = 0;
    }

    if (identifier == "time") {
        return std::make_shared<Var>(identifier, EPhysicalQuantity::Time);
    }

    const auto name_pq_pair = string_utils::split(identifier, ".");

    if (name_pq_pair.size() != 2) {
        throw parse_error("invalid format: expected sensor_name.physical_quantity");
    }

    std::string sensor_name = std::string(name_pq_pair[0]);
    if (ctx().get_sensor_idx(sensor_name) == -1) {
        log::err("Sensor \"{}\" does not exists.", name_pq_pair[0]);
        throw parse_error("invalid config: invalid rule");
    }

    const auto& sensor = ctx().get_sensor(sensor_name);

    auto maybe_pq = from_string(std::string(name_pq_pair[1]));
    if (!maybe_pq.has_value()) {
        throw parse_error("invalid format: invalid physical quantity name");
    }

    if (sensor->get_sensor_properties().count(*maybe_pq) == 0) {
        log::err(R"(Sensor "{}" does not measure "{}".)", sensor_name, name_pq_pair[1]);
        throw parse_error("invalid config: invalid rule");
    }

    return std::make_shared<Var>(std::string(name_pq_pair[0]), maybe_pq.value());
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
    if (m_sensor_name == "time") {
        return ctx().get_clock()->value(terra::EPhysicalQuantity::Time);
    }

    return ctx().get_sensor(m_sensor_name)->value(m_physical_quantity);
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

Expr create_expr_tree(const std::string& expr)
{
    Tokenizer t{expr};

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
            exprs_queue.push_back(variable(token.identifier));
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
}
