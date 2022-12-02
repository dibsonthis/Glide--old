#include "Typecheck.hpp"

void Typechecker::update_loc(std::shared_ptr<Node> node)
{
    line = node->line;
    column = node->column;
}

std::string node_type_to_string(std::shared_ptr<Node> node)
{
    auto type = node->type;

    switch(type) {
        case NodeType::INT: {
            return "int";
        }
        case NodeType::FLOAT: {
            return "float";
        }
        case NodeType::BOOL: {
            return "bool";
        }
        case NodeType::STRING: {
            return "string";
        }
        case NodeType::LIST: {
            return "list";
        }
        case NodeType::COMMA_LIST: {
            return "comma_list";
        }
        case NodeType::OBJECT: {
            return "object";
        }
        case NodeType::LAMBDA: {
            return "function";
        }
        default: {
            return "<no-repr>";
        }
    }
}

bool Typechecker::is_simple(std::string type)
{
    if (
        type == "int" ||
        type == "float" ||
        type == "string" ||
        type == "bool" ||
        type == "list" ||
        type == "comma_list" ||
        type == "object" ||
        type == "function" ||
        type == "any"
    ) return true;

    return false;
}

bool Typechecker::simple_check(std::string type, std::shared_ptr<Node> node)
{
    if (type == "any")
    {
        return true;
    }

    if (node_type_to_string(node) == type) {
        return true;
    }

    return false;
}

bool Typechecker::validate(std::shared_ptr<Node> type, std::shared_ptr<Node> value)
{
    if (is_type(type, {NodeType::ID}))
    {
        if (is_simple(type->ID.value))
        {
            auto check = simple_check(type->ID.value, value);
            return check;
        }
    }
    if (is_type(type, {NodeType::COMMA_LIST}))
    {
        for (auto elem: type->COMMA_LIST.nodes)
        {
            auto check = validate(elem, value);
            if (check) {
                return true;
            }
        }
        return false;
    }
}

std::string Typechecker::get_type(std::shared_ptr<Node> node)
{
    if (is_type(node, {NodeType::INT, NodeType::FLOAT, NodeType::BOOL, NodeType::STRING, NodeType::LIST, NodeType::OBJECT, NodeType::LAMBDA}))
    {
        return node_type_to_string(node);
    }
    if (is_type(node, {NodeType::OP}))
    {
        auto left = get_type(node->left);
        
        auto right = get_type(node->right);
        if (is_type(node, {NodeType::PLUS}))
        {

            if (left == "int" && right == "int") {
                return "int";
            }
            if (left == "int" && right == "float") {
                return "float";
            }
            if (left == "int" && right == "list") {
                return "list";
            }
            if (left == "int" && right == "string") {
                return "string";
            }

            errors.push_back(make_error("Type", "Cannot perform '+' on types: " + left + ", " + right));
            return "error";
        }
    }
    return "any";
}

bool Typechecker::typecheck(std::shared_ptr<Node> node)
{
    if (is_type(node, {NodeType::INT, NodeType::FLOAT, NodeType::BOOL, NodeType::STRING}))
    {
        return true;
    }
    if (is_type(node, {NodeType::OP}))
    {
        auto type = get_type(node);
    }
    if (is_type(node, {NodeType::EQUAL}))
    {
        if (is_type(node->left, {NodeType::DOUBLE_COLON}))
        {
            auto var = node->left->left;
            auto type = node->left->right;
            auto value = node->right;

            if (is_type(value, {NodeType::ID})) {
                value = symbol_table[value->ID.value].value;
            }

            auto check = validate(type, value);
            if (!check) {
                errors.push_back(make_error("Type", "Cannot assign a value of type '" + node_type_to_string(value) + "' to variable of type '" + type->ID.value + "'"));
                return false;
            }

            symbol_table[var->ID.value] = Type(type->repr(), value);

            return true;
        }

        auto var = node->left;
        auto value = node->right;
        symbol_table[var->ID.value] = Type(node_type_to_string(value), value);
    }
}

void Typechecker::run()
{
    for (auto node: nodes)
    {
        update_loc(node);
        bool check = typecheck(node);
        if (errors.size() > 0) {
            for (auto error : errors) {
                std::cout << error << "\n" << std::flush;
            }
            break;
        }
    }
}

std::string Typechecker::make_error(std::string name, std::string message)
{
    return name + " Error (" + std::to_string(line) + ", " + std::to_string(column) + "): " + message;
}