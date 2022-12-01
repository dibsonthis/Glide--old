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
            if (!check) {
                errors.push_back(make_error("Type", "Cannot assign a value of type '" + node_type_to_string(value) + "' to variable of type '" + type->ID.value + "'"));
                return false;
            }
            return true;
        }
    }
    if (is_type(type, {NodeType::COMMA_LIST}))
    {
        for (auto elem: type->COMMA_LIST.nodes)
        {
            if (is_type(elem, {NodeType::ID}))
            {
                if (is_simple(elem->ID.value))
                {
                    auto check = simple_check(elem->ID.value, value);
                    if (check) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}

bool Typechecker::typecheck(std::shared_ptr<Node> node)
{
    if (is_type(node, {NodeType::EQUAL}))
    {
        if (is_type(node->left, {NodeType::DOUBLE_COLON}))
        {
            auto var = node->left->left;
            auto type = node->left->right;
            auto value = node->right;

            auto check = validate(type, value);
            if (!check) {
                errors.push_back(make_error("Type", "Cannot assign a value of type '" + node_type_to_string(value) + "' to variable of type '" + type->ID.value + "'"));
                return false;
            }

            symbol_table[var->ID.value] = Type(type->repr(), type);
        }
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