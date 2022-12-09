#include "Typecheck.hpp"

void Typechecker::update_loc(std::shared_ptr<Node> node)
{
    line = node->line;
    column = node->column;
}

std::string Typechecker::node_type_to_string(std::shared_ptr<Node> node)
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
            std::string repr = "[ ";
            for (auto elem : node->LIST.nodes)
            {
                repr += node_type_to_string(get_type(elem)) + " ";
            }
            repr += "]";
            return repr;
        }
        case NodeType::COMMA_LIST: {
            std::string repr = "( ";
            for (auto elem : node->COMMA_LIST.nodes)
            {
                repr += node_type_to_string(get_type(elem)) + " ";
            }
            repr += ")";
            return repr;
        }
        case NodeType::OBJECT: {
            std::string repr = "{\n";
            for (auto prop : node->OBJECT.properties)
            {
                repr += "\t" + prop.first + ": " + node_type_to_string(prop.second) + "\n";
            }
            repr += "}";
            return repr;
        }
        case NodeType::LAMBDA: {
            return "function";
        }
        case NodeType::FUNC_T: {
            std::string repr = "[ ";
            for (auto param : node->FUNC_T.params)
            {
                repr += node_type_to_string(param.second) + " ";
            }
            repr += "] => { " + node_type_to_string(node->FUNC_T.return_type) + " }";
            return repr;
        }
        case NodeType::ANY: {
            return "any";
        }
        case NodeType::ERROR: {
            return "error";
        }
        case NodeType::EMPTY: {
            return "null";
        }
        default: {
            return "<no-repr>";
        }
    }
}

std::shared_ptr<Node> Typechecker::get_type_add(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);
    auto right = get_type(node->right);

    if (left->type == NodeType::ANY)
    {
        return left;
    }
    if (right->type == NodeType::ANY)
    {
        return right;
    }

    if (left->type == NodeType::EMPTY)
    {
        return left;
    }

    if (right->type == NodeType::EMPTY)
    {
        return right;
    }

    if (left->type == NodeType::INT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::INT && right->type == NodeType::FLOAT)
    {
        return right;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::FLOAT)
    {
        return left;
    }

    if (left->type == NodeType::STRING && right->type == NodeType::STRING)
    {
        return left;
    }

    if (left->type == NodeType::STRING && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::INT && right->type == NodeType::STRING)
    {
        return right;
    }

    if (left->type == NodeType::STRING && right->type == NodeType::FLOAT)
    {
        return left;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::STRING)
    {
        return left;
    }

    if (left->type == NodeType::STRING && right->type == NodeType::BOOL)
    {
        return left;
    }

    if (left->type == NodeType::BOOL && right->type == NodeType::STRING)
    {
        return right;
    }

    if (left->type == NodeType::LIST && right->type == NodeType::LIST)
    {
        auto type = std::make_shared<Node>(NodeType::LIST);
        for (auto elem : left->LIST.nodes)
        {
            type->LIST.nodes.push_back(elem);
        }
         for (auto elem : right->LIST.nodes)
        {
            type->LIST.nodes.push_back(elem);
        }

        std::sort(type->LIST.nodes.begin(), type->LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
        type->LIST.nodes.erase(std::unique(type->LIST.nodes.begin(), type->LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), type->LIST.nodes.end());
        return type;
    }

    if (left->type == NodeType::LIST)
    {
        auto type = std::make_shared<Node>(NodeType::LIST);

        for (auto elem : left->LIST.nodes)
        {
            type->LIST.nodes.push_back(elem);
        }
        
        type->LIST.nodes.push_back(right);
        type->LIST.nodes.erase(std::unique(type->LIST.nodes.begin(), type->LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), type->LIST.nodes.end());
        std::sort(type->LIST.nodes.begin(), type->LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
        return type;
    }
    
    if (right->type == NodeType::LIST)
    {
        auto type = std::make_shared<Node>(NodeType::LIST);

        for (auto elem : right->LIST.nodes)
        {
            type->LIST.nodes.push_back(elem);
        }
        
        type->LIST.nodes.push_back(left);

        std::sort(type->LIST.nodes.begin(), type->LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
        type->LIST.nodes.erase(std::unique(type->LIST.nodes.begin(), type->LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), type->LIST.nodes.end());
        return type;
    }

    if (left->type == NodeType::OBJECT && right->type == NodeType::OBJECT)
    {
        auto obj = std::make_shared<Node>(NodeType::OBJECT);
        auto block = std::make_shared<Node>(NodeType::BLOCK);
        for (auto prop : left->OBJECT.properties)
        {
            obj->OBJECT.properties[prop.first] = prop.second;
        }
        for (auto prop : right->OBJECT.properties)
        {
            obj->OBJECT.properties[prop.first] = prop.second;
        }

        std::vector<std::pair<std::string, std::shared_ptr<Node>>> ordered_keys;

        for (auto& prop : obj->OBJECT.properties)
        {
            ordered_keys.push_back(prop);
        }

        std::sort(ordered_keys.begin(), ordered_keys.end(), [] (std::pair<std::string, std::shared_ptr<Node>> const &lhs, std::pair<std::string, std::shared_ptr<Node>> const &rhs) {return lhs.first < rhs.first;});
        obj->OBJECT.properties.clear();

        for (auto& elem : ordered_keys)
        {
            obj->OBJECT.properties[elem.first] = elem.second;
        }

        return obj;
    }

    errors.push_back(make_error("Type", "Cannot perform '+' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
    auto error = std::make_shared<Node>(NodeType::ERROR);
    return error;
}

std::shared_ptr<Node> Typechecker::get_type_sub(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);
    auto right = get_type(node->right);

    if (left->type == NodeType::ANY)
    {
        return left;
    }
    if (right->type == NodeType::ANY)
    {
        return right;
    }

    if (left->type == NodeType::EMPTY)
    {
        return left;
    }

    if (right->type == NodeType::EMPTY)
    {
        return right;
    }

    if (left->type == NodeType::INT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::INT && right->type == NodeType::FLOAT)
    {
        return right;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::FLOAT)
    {
        return left;
    }

    errors.push_back(make_error("Type", "Cannot perform '-' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
    auto error = std::make_shared<Node>(NodeType::ERROR);
    return error;
}

std::shared_ptr<Node> Typechecker::get_type_mul(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);
    auto right = get_type(node->right);

    if (left->type == NodeType::ANY)
    {
        return left;
    }
    if (right->type == NodeType::ANY)
    {
        return right;
    }

    if (left->type == NodeType::EMPTY)
    {
        return left;
    }
    if (right->type == NodeType::EMPTY)
    {
        return right;
    }

    if (left->type == NodeType::INT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::INT && right->type == NodeType::FLOAT)
    {
        return right;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::FLOAT)
    {
        return left;
    }

    if (left->type == NodeType::INT && right->type == NodeType::STRING)
    {
        return right;
    }

    if (left->type == NodeType::STRING && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::LIST && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::INT && right->type == NodeType::LIST)
    {
        return right;
    }

    errors.push_back(make_error("Type", "Cannot perform '*' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
    auto error = std::make_shared<Node>(NodeType::ERROR);
    return error;
}

std::shared_ptr<Node> Typechecker::get_type_div(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);
    auto right = get_type(node->right);

    if (left->type == NodeType::ANY)
    {
        return left;
    }
    if (right->type == NodeType::ANY)
    {
        return right;
    }

    if (left->type == NodeType::EMPTY)
    {
        return left;
    }

    if (right->type == NodeType::EMPTY)
    {
        return right;
    }

    if (left->type == NodeType::INT && right->type == NodeType::INT)
    {
        return std::make_shared<Node>(NodeType::FLOAT);
    }

    if (left->type == NodeType::INT && right->type == NodeType::FLOAT)
    {
        return right;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::FLOAT)
    {
        return left;
    }

    errors.push_back(make_error("Type", "Cannot perform '/' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
    auto error = std::make_shared<Node>(NodeType::ERROR);
    return error;
}

std::shared_ptr<Node> Typechecker::get_type_mod(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);
    auto right = get_type(node->right);

    if (left->type == NodeType::ANY)
    {
        return left;
    }
    if (right->type == NodeType::ANY)
    {
        return right;
    }

    if (left->type == NodeType::EMPTY)
    {
        return left;
    }

    if (right->type == NodeType::EMPTY)
    {
        return right;
    }

    if (left->type == NodeType::INT && right->type == NodeType::INT)
    {
        return right;
    }

    if (left->type == NodeType::INT && right->type == NodeType::FLOAT)
    {
        return right;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::INT)
    {
        return left;
    }

    if (left->type == NodeType::FLOAT && right->type == NodeType::FLOAT)
    {
        return right;
    }

    errors.push_back(make_error("Type", "Cannot perform '/' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
    auto error = std::make_shared<Node>(NodeType::ERROR);
    return error;
}

std::shared_ptr<Node> Typechecker::get_type_equality(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);
    if (left->type == NodeType::ERROR)
    {
        return left;
    }
    auto right = get_type(node->right);
    if (right->type == NodeType::ERROR)
    {
        return right;
    }
    return std::make_shared<Node>(NodeType::BOOL);
}

std::shared_ptr<Node> Typechecker::get_type_dot(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);

    if (left->type == NodeType::ERROR)
    {
        return left;
    }

    if (node->right->type == NodeType::FUNC_CALL)
    {
        // find the function inside the object
        if (left->OBJECT.properties.find(node->right->FUNC_CALL.name->ID.value) != left->OBJECT.properties.end())
        {
            auto func_type = left->OBJECT.properties[node->right->FUNC_CALL.name->ID.value];
            auto name = node->right->FUNC_CALL.name->ID.value;
            auto args = node->right->FUNC_CALL.args;

            // inject the args into the type, which might have existing args (curried)

            for (auto arg : args)
            {
                func_type->FUNC_T.args.push_back(arg);
            }

            args = func_type->FUNC_T.args;

            // typecheck the args
            // TODO: additional overflow args do not get typechecked, do we want to change this?

            for (int i = 0; i < func_type->FUNC_T.params.size(); i++)
            {
                if (i >= args.size())
                {
                    break;
                }

                auto arg = get_type(args[i]);
                if (arg->type == NodeType::ERROR)
                {
                    return arg;
                }

                if (!match_types(func_type->FUNC_T.params[i].second, arg))
                {
                    errors.push_back(make_error("Type", "Function '" + name + "' - Cannot assign argument of type '" + node_type_to_string(args[i]) + "' to parameter of type '" + node_type_to_string(func_type->FUNC_T.params[i].second) + "'"));
                    return std::make_shared<Node>(NodeType::ERROR);
                }
            }

            if (args.size() >= func_type->FUNC_T.params.size())
            {
                func_type->FUNC_T.args.clear();
                return func_type->FUNC_T.return_type;
            }

            return func_type;
        }
        
        errors.push_back(make_error("Value", "Function '" + node->right->FUNC_CALL.name->ID.value + "' does not exist on object"));
        return std::make_shared<Node>(NodeType::ERROR);
    }

    auto right = std::make_shared<Node>(*node->right);

    if (is_type(node->right, {NodeType::ID}))
    {
        right->type = NodeType::STRING;
        right->STRING.value = node->right->ID.value;
    }

    if (left->type == NodeType::STRING && right->type == NodeType::STRING)
    {
        if (right->STRING.value == "to_chars")
        {
            auto type = std::make_shared<Node>(NodeType::LIST);
            type->LIST.nodes.push_back(std::make_shared<Node>(NodeType::STRING));
            return type;
        }

        if (right->STRING.value == "length")
        {
            auto type = std::make_shared<Node>(NodeType::INT);
            return type;
        }

        errors.push_back(make_error("Key", "Property '" + right->STRING.value + "' does not exist on type 'string'"));
        return std::make_shared<Node>(NodeType::ERROR);
    }

    if (left->type == NodeType::LIST && right->type == NodeType::INT)
    {
        std::shared_ptr<Node> type;
        if (left->LIST.nodes.size() == 0)
        {
            type->type = NodeType::ANY;
            return type;
        }
        if (left->LIST.nodes.size() > 1)
        {
            type->type = NodeType::COMMA_LIST;
            type->COMMA_LIST.nodes = left->LIST.nodes;
            return type;
        }

        type = left->LIST.nodes[0];
        return type;
    }

    if (left->type == NodeType::LIST && right->type == NodeType::STRING)
    {
        if (right->STRING.value == "length")
        {
            auto type = std::make_shared<Node>(NodeType::INT);
            return type;
        }
        
        errors.push_back(make_error("Key", "Property '" + right->STRING.value + "' does not exist on type 'list'"));
        return std::make_shared<Node>(NodeType::ERROR);
    }

    if (left->type == NodeType::LIST && right->type == NodeType::LIST)
    {
        auto type = std::make_shared<Node>(NodeType::COMMA_LIST);
        type->COMMA_LIST.nodes.push_back(std::make_shared<Node>(NodeType::EMPTY));
        for (auto node : left->LIST.nodes)
        {
            type->COMMA_LIST.nodes.push_back(node);
        }

        std::sort(type->LIST.nodes.begin(), type->LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
        type->LIST.nodes.erase(std::unique(type->LIST.nodes.begin(), type->LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), type->LIST.nodes.end());
        return type;
    }

    if (left->type == NodeType::OBJECT && right->type == NodeType::STRING)
    {
        auto name = right->STRING.value;

        if (name == "_keys")
        {
            auto type = std::make_shared<Node>(NodeType::LIST);
            type->LIST.nodes.push_back(std::make_shared<Node>(NodeType::STRING));
            return type;
        }

        if (name == "_values")
        {
            auto type = std::make_shared<Node>(NodeType::LIST);
            type->LIST.nodes.push_back(std::make_shared<Node>(NodeType::ANY));
            return type;
        }

        if (name == "_name")
        {
            auto type = std::make_shared<Node>(NodeType::STRING);
            return type;
        }

        if (name == "_items")
        {
            auto type = std::make_shared<Node>(NodeType::LIST);
            auto obj = std::make_shared<Node>(NodeType::OBJECT);
            obj->OBJECT.properties["key"] = std::make_shared<Node>(NodeType::STRING);
            obj->OBJECT.properties["value"] = std::make_shared<Node>(NodeType::ANY);
            type->LIST.nodes.push_back(obj);
            return type;
        }

        if (left->OBJECT.properties.find(name) == left->OBJECT.properties.end())
        {
            auto type = std::make_shared<Node>(NodeType::EMPTY);
            return type;
        }

        return left->OBJECT.properties[name];
    }

    return std::make_shared<Node>(NodeType::ANY);
}

std::shared_ptr<Node> Typechecker::get_type_arrow(std::shared_ptr<Node> node)
{
    auto left = get_type(node->left);

    if (left->type == NodeType::ERROR)
    {
        return left;
    }

    auto right = std::make_shared<Node>(NodeType::ERROR);

    if (node->right->type == NodeType::FUNC_CALL)
    {
        right = std::make_shared<Node>(*node->right);
    }
    else
    {
        right = get_type(node->right);
    }

    if (left->type == NodeType::ANY)
    {
        return left;
    }
    if (right->type == NodeType::ANY)
    {
        return right;
    }

    // TODO: proper typing on op injection
    if (right->type == NodeType::OP || right->type == NodeType::PARTIAL_OP)
    {
        return std::make_shared<Node>(NodeType::ANY);
    }

    if (right->type == NodeType::FUNC_T)
    {
        if (left->type == NodeType::COMMA_LIST)
        {
            for (auto elem : left->COMMA_LIST.nodes)
            {
                right->FUNC_T.args.push_back(elem);
            }
        }
        else
        {
            right->FUNC_T.args.push_back(node->left);
        }

        for (int i = 0; i < right->FUNC_T.params.size(); i++)
        {
            if (i >= right->FUNC_T.args.size())
            {
                break;
            }
            auto arg = get_type(right->FUNC_T.args[i]);
            if (arg->type == NodeType::ERROR)
            {
                return arg;
            }

            if (!match_types(right->FUNC_T.params[i].second, arg))
            {
                errors.push_back(make_error("Type", "Function '" + right->FUNC_T.name + "' - Cannot assign argument of type '" + node_type_to_string(arg) + "' to parameter of type '" + node_type_to_string(right->FUNC_T.params[i].second) + "'"));
                return std::make_shared<Node>(NodeType::ERROR);
            }
        }

        if (right->FUNC_T.return_type == nullptr)
        {
            right->FUNC_T.return_type = std::make_shared<Node>(NodeType::EMPTY);
        }

        if (right->FUNC_T.args.size() >= right->FUNC_T.params.size())
        {
            right->FUNC_T.args.clear();
            return right->FUNC_T.return_type;
        }

        return right;
    }

    if (right->type == NodeType::FUNC_CALL)
    {
        // get the function type, so we can inject it with args
        auto func_name = right->FUNC_CALL.name;
        auto func = get_type(func_name);

        if (func->type == NodeType::ERROR)
        {
            return func;
        }

        for (auto arg : right->FUNC_CALL.args)
        {
            func->FUNC_T.args.push_back(arg);
        }

        if (left->type == NodeType::COMMA_LIST)
        {
            for (auto elem : left->COMMA_LIST.nodes)
            {
                right->FUNC_CALL.args.push_back(elem);
                func->FUNC_T.args.push_back(elem);
            }
        }
        else
        {
            right->FUNC_CALL.args.push_back(left);
            func->FUNC_T.args.push_back(left);
        }

        return get_type(right);
    }

    errors.push_back(make_error("Type", "Cannot perform '->' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
    auto error = std::make_shared<Node>(NodeType::ERROR);
    return error;
}

std::shared_ptr<Node> Typechecker::get_type(std::shared_ptr<Node> node)
{
    update_loc(node);
    if (is_type(node, {NodeType::ID}))
    {
        auto name = node->ID.value;

        if (name == "int")
        {
            return std::make_shared<Node>(NodeType::INT);
        }
        if (name == "float")
        {
            return std::make_shared<Node>(NodeType::FLOAT);
        }
        if (name == "bool")
        {
            return std::make_shared<Node>(NodeType::BOOL);
        }
        if (name == "string")
        {
            return std::make_shared<Node>(NodeType::STRING);
        }
        if (name == "list")
        {
            return std::make_shared<Node>(NodeType::LIST);
        }
        if (name == "comma_list")
        {
            return std::make_shared<Node>(NodeType::COMMA_LIST);
        }
        if (name == "object")
        {
            return std::make_shared<Node>(NodeType::OBJECT);
        }
        if (name == "function")
        {
            return std::make_shared<Node>(NodeType::LAMBDA);
        }
        if (name == "any")
        {
            return std::make_shared<Node>(NodeType::ANY);
        }
        if (name == "null")
        {
            return std::make_shared<Node>(NodeType::EMPTY);
        }
        if (name == "empty")
        {
            return std::make_shared<Node>(NodeType::EMPTY);
        }
        if (name == "print")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::EMPTY);
            return func;
        }
        if (name == "type")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::OBJECT);
            return func;
        }
        if (name == "address")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::INT);
            return func;
        }
        if (name == "shape")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::OBJECT);
            return func;
        }
        if (name == "to_string")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::STRING);
            return func;
        }
        if (name == "to_int")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::INT);
            return func;
        }
        if (name == "to_float")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::FLOAT);
            return func;
        }
        if (name == "to_list")
        {
            auto func = std::make_shared<Node>(NodeType::FUNC_T);
            func->FUNC_T.return_type = std::make_shared<Node>(NodeType::LIST);
            func->FUNC_T.return_type->LIST.nodes.push_back(std::make_shared<Node>(NodeType::ANY));
            return func;
        }
        if (name == "this" || name == "_catch")
        {
            return std::make_shared<Node>(NodeType::ANY);
        }

        if (symbol_table.find(node->ID.value) == symbol_table.end()) 
        {
            errors.push_back(make_error("Reference", "Variable '" + node->ID.value + "' is undefined"));
            return std::make_shared<Node>(NodeType::ERROR);
        } 
        else 
        {
            return symbol_table[node->ID.value].value_type;
        }
    }
    if (is_type(node, {NodeType::INT, NodeType::FLOAT, NodeType::STRING, NodeType::BOOL, NodeType::EMPTY, NodeType::ERROR, NodeType::FUNC_T}))
    {
        return node;
    }
    if (is_type(node, {NodeType::OBJECT}))
    {
        auto type_obj = std::make_shared<Node>(NodeType::OBJECT);
        for (auto prop: node->right->BLOCK.nodes)
        {
            if (is_type(prop->left, {NodeType::ID}))
            {
                type_obj->OBJECT.properties[prop->left->ID.value] = get_type(prop->right);
            }
            else
            {
                type_obj->OBJECT.properties[prop->left->STRING.value] = get_type(prop->right);
            }
        }

        std::vector<std::pair<std::string, std::shared_ptr<Node>>> ordered_keys;

        for (auto& prop : type_obj->OBJECT.properties)
        {
            ordered_keys.push_back(prop);
        }

        std::sort(ordered_keys.begin(), ordered_keys.end(), [] (std::pair<std::string, std::shared_ptr<Node>> const &lhs, std::pair<std::string, std::shared_ptr<Node>> const &rhs) {return lhs.first < rhs.first;});
        type_obj->OBJECT.properties.clear();

        for (auto& elem : ordered_keys)
        {
            type_obj->OBJECT.properties[elem.first] = elem.second;
        }

        return type_obj;
    }
    if (is_type(node, {NodeType::COMMA_LIST}))
    {
        auto comma_list = std::make_shared<Node>(NodeType::COMMA_LIST);
        for (auto elem : node->COMMA_LIST.nodes)
        {
            comma_list->COMMA_LIST.nodes.push_back(get_type(elem));
        }
        return comma_list;
    }
    if (is_type(node, {NodeType::LIST}))
    {
        auto list = std::make_shared<Node>(NodeType::LIST);
        for (auto elem : node->LIST.nodes)
        {
            list->LIST.nodes.push_back(get_type(elem));
        }
        if (list->LIST.nodes.size() == 0)
        {
            list->LIST.nodes.push_back(std::make_shared<Node>(NodeType::ANY));
        }
        // reduce list to sorted unique types
        std::sort(list->LIST.nodes.begin(), list->LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
        list->LIST.nodes.erase(std::unique(list->LIST.nodes.begin(), list->LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), list->LIST.nodes.end());
        return list;
    }
    if (is_type(node, {NodeType::LAMBDA}))
    {
        auto func_type = std::make_shared<Node>(NodeType::FUNC_T);
        func_type->FUNC_T.name = (node->ID.value != "" ? node->ID.value : "Lambda");
        std::shared_ptr<Node> list_node;

        // check if return type present
        if (is_type(node->left, {NodeType::DOUBLE_COLON}))
        {
            auto ret_type = get_type(node->left->right);

            if (ret_type->type == NodeType::COMMA_LIST)
            {
                std::sort(ret_type->COMMA_LIST.nodes.begin(), ret_type->COMMA_LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
                ret_type->COMMA_LIST.nodes.erase(std::unique(ret_type->COMMA_LIST.nodes.begin(), ret_type->COMMA_LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), ret_type->COMMA_LIST.nodes.end());
            }

            func_type->FUNC_T.return_type = ret_type;

            list_node = node->left->left;

            // remove type
            node->left = node->left->left;
        }
        else
        {
            func_type->FUNC_T.return_type = std::make_shared<Node>(NodeType::ANY);
            list_node = node->left;
        }

        for (auto& elem : list_node->LIST.nodes)
        {
            if (is_type(elem, {NodeType::DOUBLE_COLON}))
            {
                func_type->FUNC_T.params.push_back(std::make_pair(elem->left->ID.value, get_type(elem->right)));

                // remove type
                elem = elem->left;
            }
            else
            {
                func_type->FUNC_T.params.push_back(std::make_pair(elem->ID.value, std::make_shared<Node>(NodeType::ANY)));
            }
        }

        // check that the function actually returns the same type as the return type
        Typechecker tc(file_name, node->right->BLOCK.nodes);

        for (auto elem : symbol_table)
        {
            tc.symbol_table[elem.first] = elem.second;
        }

        for (auto param : func_type->FUNC_T.params)
        {
            tc.symbol_table[param.first] = Type(param.second, param.second);
        }

        auto branched_return_types = std::make_shared<Node>(NodeType::COMMA_LIST);
        auto last_node = std::make_shared<Node>(NodeType::EMPTY);
        int num_branch_statements = 0;
        bool has_return = false;

        for (int i = 0; i < tc.nodes.size(); i++)
        {
            tc.update_loc(tc.nodes[i]);
            auto type = tc.get_type(tc.nodes[i]);
            if (type->type == NodeType::ERROR)
            {
                for (auto error : tc.errors) 
                {
                    std::cout << error << "\n" << std::flush;
                }

                return type;
            }

            if (tc.nodes[i]->type == NodeType::KEYWORD && tc.nodes[i]->ID.value == "ret")
            {
                has_return = true;
                last_node = type;
                break;
            }

            if (tc.nodes[i]->type == NodeType::IF_STATEMENT || tc.nodes[i]->type == NodeType::IF_BLOCK)
            {
                num_branch_statements++;

                if (type->type == NodeType::COMMA_LIST)
                {
                    // need to flatten the if block to separate types
                    for (auto t : type->COMMA_LIST.nodes)
                    {
                        branched_return_types->COMMA_LIST.nodes.push_back(t);
                    }
                }
                else
                {
                    branched_return_types->COMMA_LIST.nodes.push_back(type);
                }
            }

            if (i == tc.nodes.size()-1)
            {
                last_node = type;
            }
        }

        if (num_branch_statements == tc.nodes.size())
        {
            // if all statements are if/if blocks, then we add an empty type
            branched_return_types->COMMA_LIST.nodes.push_back(std::make_shared<Node>(NodeType::EMPTY));
        }

        if ((tc.nodes[tc.nodes.size()-1]->type == NodeType::IF_STATEMENT || tc.nodes[tc.nodes.size()-1]->type == NodeType::IF_BLOCK) && !has_return)
        {
            // last element is an if statement/if block, and no return found, we add an empty type
            branched_return_types->COMMA_LIST.nodes.push_back(std::make_shared<Node>(NodeType::EMPTY));
        }

        if (branched_return_types->COMMA_LIST.nodes.size() > 0)
        {
            if (last_node->type == NodeType::COMMA_LIST)
            {
                for (auto t : last_node->COMMA_LIST.nodes)
                {
                    branched_return_types->COMMA_LIST.nodes.push_back(t);
                }
            }
            else
            {
                branched_return_types->COMMA_LIST.nodes.push_back(last_node);
            }

            std::sort(branched_return_types->COMMA_LIST.nodes.begin(), branched_return_types->COMMA_LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
            branched_return_types->COMMA_LIST.nodes.erase(std::unique(branched_return_types->COMMA_LIST.nodes.begin(), branched_return_types->COMMA_LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), branched_return_types->COMMA_LIST.nodes.end());

            last_node = branched_return_types;

            if (last_node->COMMA_LIST.nodes.size() == 1)
            {
                last_node = last_node->COMMA_LIST.nodes[0];
            }
        }

        if (!tc.match_types(func_type->FUNC_T.return_type, last_node))
        {
            errors.push_back(tc.make_error("Type", "Function '" + func_type->FUNC_T.name + "' must return value of type '" + node_type_to_string(func_type->FUNC_T.return_type) + "' but instead returns '" + node_type_to_string(last_node) + "'"));
            return std::make_shared<Node>(NodeType::ERROR);
        }

        func_type->FUNC_T.return_type = last_node;

        return func_type;
    }
    if (is_type(node, {NodeType::FUNC_CALL}))
    {
        auto name = node->FUNC_CALL.name->ID.value;
        auto args = node->FUNC_CALL.args;

        if (name == "import")
        {
            if (args.size() != 1)
            {
                errors.push_back(make_error("Builtin", "function 'import' only accepts one parameter"));
                return std::make_shared<Node>(NodeType::ERROR);
            }

            auto file_path = args[0]->STRING.value;

            Lexer lexer(file_path);
            lexer.tokenize();

            Parser parser(lexer.file_name, lexer.nodes);
            parser.parse();

            Typechecker tc(lexer.file_name, parser.nodes);
            bool check = tc.run();

            if (!check)
            {
                return std::make_shared<Node>(NodeType::ERROR);
            }
            
            auto import_obj = std::make_shared<Node>(NodeType::OBJECT);

            for (auto elem : tc.symbol_table)
            {
                if (elem.second.value_type->type == NodeType::FUNC_T)
                {
                    elem.second.allowed_type->FUNC_T.name = elem.first;
                    elem.second.value_type->FUNC_T.name = elem.first;
                    // clear args that may have been put in during typechecking
                    elem.second.allowed_type->FUNC_T.args.clear();
                    elem.second.value_type->FUNC_T.args.clear();
                }
                import_obj->OBJECT.properties[elem.first] = elem.second.value_type;
            }

            return import_obj;
        }

        std::shared_ptr<Node> func_type = std::make_shared<Node>(NodeType::ANY);

        if (name == "print")
        {
            func_type->type = NodeType::EMPTY;
            return func_type;
        }
        if (name == "to_int")
        {
            func_type->type = NodeType::INT;
            return func_type;
        }
        if (name == "to_string")
        {
            func_type->type = NodeType::STRING;
            return func_type;
        }
        if (name == "to_float")
        {
            func_type->type = NodeType::FLOAT;
            return func_type;
        }
        if (name == "to_list")
        {
            func_type->type = NodeType::LIST;
            func_type->LIST.nodes.push_back(std::make_shared<Node>(NodeType::ANY));
            return func_type;
        }
        if (name == "shape")
        {
            func_type->type = NodeType::OBJECT;
            return func_type;
        }
        if (name == "address")
        {
            func_type->type = NodeType::INT;
            return func_type;
        }
        if (name == "type")
        {
            func_type->type = NodeType::STRING;
            return func_type;
        }

        if (symbol_table.find(name) == symbol_table.end()) 
        {
            for (int i = 0; i < args.size(); i++)
            {
                auto arg = get_type(args[i]);
                if (arg->type == NodeType::ERROR)
                {
                    return arg;
                }
            }

            return func_type;
        } 

        func_type = symbol_table[name].value_type;

        // inject the args into the type, which might have existing args (curried)

        for (auto arg : args)
        {
            func_type->FUNC_T.args.push_back(arg);
        }

        args = func_type->FUNC_T.args;

        // typecheck the args
        // TODO: additional overflow args do not get typechecked, do we want to change this?

        for (int i = 0; i < func_type->FUNC_T.params.size(); i++)
        {
            if (i >= args.size())
            {
                break;
            }
            auto arg = get_type(args[i]);
            if (arg->type == NodeType::ERROR)
            {
                return arg;
            }

            if (!match_types(func_type->FUNC_T.params[i].second, arg))
            {
                errors.push_back(make_error("Type", "Function '" + name + "' - Cannot assign argument of type '" + node_type_to_string(arg) + "' to parameter of type '" + node_type_to_string(func_type->FUNC_T.params[i].second) + "'"));
                return std::make_shared<Node>(NodeType::ERROR);
            }
        }

        if (func_type->FUNC_T.return_type == nullptr)
        {
            func_type->FUNC_T.return_type = std::make_shared<Node>(NodeType::EMPTY);
        }

        if (args.size() >= func_type->FUNC_T.params.size())
        {
            func_type->FUNC_T.args.clear();
            return func_type->FUNC_T.return_type;
        }

        return func_type;
    }
    if (is_type(node, {NodeType::DOT}))
    {
        return get_type_dot(node);
    }
    if (is_type(node, {NodeType::PLUS}))
    {
        return get_type_add(node);
    }
    if (is_type(node, {NodeType::MINUS}))
    {
        return get_type_sub(node);
    }
    if (is_type(node, {NodeType::STAR}))
    {
        return get_type_mul(node);
    }
    if (is_type(node, {NodeType::SLASH}))
    {
        return get_type_div(node);
    }
    if (is_type(node, {NodeType::PERCENT}))
    {
        return get_type_mod(node);
    }
    if (is_type(node, {NodeType::RIGHT_ARROW_SINGLE}))
    {
        return get_type_arrow(node);
    }
    if (is_type(node, {NodeType::EXCLAMATION, NodeType::L_ANGLE, NodeType::R_ANGLE, NodeType::LT_EQUAL, NodeType::GT_EQUAL, NodeType::EQ_EQ, NodeType::NOT_EQUAL, NodeType::AND, NodeType::OR}))
    {
        return get_type_equality(node);
    }
    if (is_type(node, {NodeType::POS, NodeType::NEG}))
    {
        return get_type(node->right);
    }
    if (is_type(node, {NodeType::DOUBLE_DOT}))
    {
        auto left = get_type(node->left);
        auto right = get_type(node->right);

        if (left->type != NodeType::INT && right->type != NodeType::INT)
        {
            errors.push_back(make_error("Type", "Range only accepts integer values"));
            return std::make_shared<Node>(NodeType::ERROR);
        }

        auto type = std::make_shared<Node>(NodeType::LIST);
        type->LIST.nodes.push_back(std::make_shared<Node>(NodeType::INT));
        return type;
    }
    if (is_type(node, {NodeType::EQUAL}))
    {
        if (is_type(node->left, {NodeType::DOUBLE_COLON}))
        {
            auto var = node->left->left;

            if (var->type == NodeType::ID)
            {
                // we want to search for the ID in symbol table first
                if (symbol_table.find(var->ID.value) != symbol_table.end())
                {
                    errors.push_back(make_error("Type", "'" + var->ID.value + "' - Cannot reassign type"));
                    return std::make_shared<Node>(NodeType::ERROR);
                }
            }

            // assign name if right is lambda
            if (node->right->type == NodeType::LAMBDA)
            {
                node->right->ID.value = var->ID.value;
            }

            auto val_type = get_type(node->right);

            if (is_type(val_type, {NodeType::ERROR}))
            {
                return val_type;
            }

            auto provided_type = get_type(node->left->right);

            if (match_types(provided_type, val_type))
            {
                symbol_table[node->left->left->ID.value] = Type(provided_type, val_type);
                // remove type details from node
                node->left = node->left->left;
                return std::make_shared<Node>(NodeType::EMPTY);
            }

            errors.push_back(make_error("Type", "'" + var->ID.value + "' - Cannot assign value of type '" + node_type_to_string(val_type) + "' to variable of type '" + node_type_to_string(provided_type) + "'"));
            return std::make_shared<Node>(NodeType::ERROR);
        }
        else
        {
            auto var = node->left;

            // assign name if right is lambda
            if (node->right->type == NodeType::LAMBDA)
            {
                node->right->ID.value = var->ID.value;
            }

            auto val_type = get_type(node->right);

            if (is_type(val_type, {NodeType::ERROR}))
            {
                return val_type;
            }

            if (symbol_table.find(var->ID.value) == symbol_table.end())
            {
                // new variable
                symbol_table[var->ID.value] = Type(val_type, val_type);
                return std::make_shared<Node>(NodeType::EMPTY);
            }

            auto var_type = symbol_table[var->ID.value];

            // found variable, check if type matches value type
            if (match_types(var_type.allowed_type, val_type))
            {
                if (val_type->type == NodeType::FUNC_T)
                {
                    val_type->FUNC_T.name = var->ID.value;
                }
                symbol_table[var->ID.value] = Type(var_type.allowed_type, val_type);
                return std::make_shared<Node>(NodeType::EMPTY);;
            }

            errors.push_back(make_error("Type", "'" + var->ID.value + "' - Cannot assign value of type '" + node_type_to_string(val_type) + "' to variable of type '" + node_type_to_string(var_type.allowed_type) + "'"));
            return std::make_shared<Node>(NodeType::ERROR);
        }
    }
    if (is_type(node, {NodeType::PLUS_EQ}))
    {
        auto plus_op = std::make_shared<Node>(NodeType::OP);
        plus_op->left = node->left;
        plus_op->right = node->right;
        auto res = get_type_add(plus_op);

        if (res->type == NodeType::ERROR)
        {
            return res;
        }

        auto eq_op = std::make_shared<Node>(NodeType::EQUAL);
        eq_op->left = node->left;
        eq_op->right = res;

        return get_type(eq_op);
    }
    if (is_type(node, {NodeType::MINUS_EQ}))
    {
        auto minus_op = std::make_shared<Node>(NodeType::OP);
        minus_op->left = node->left;
        minus_op->right = node->right;
        auto res = get_type_sub(minus_op);

        if (res->type == NodeType::ERROR)
        {
            return res;
        }

        auto eq_op = std::make_shared<Node>(NodeType::EQUAL);
        eq_op->left = node->left;
        eq_op->right = res;

        return get_type(eq_op);
    }
    if (is_type(node, {NodeType::COPY}))
    {
        return get_type(node->right);
    }
    if (is_type(node, {NodeType::OP}))
    {
        auto left = get_type(node->left);
        auto right = get_type(node->right);

        if (left->type == NodeType::ERROR || right->type == NodeType::ERROR)
        {
            return std::make_shared<Node>(NodeType::ERROR);
        }
        return std::make_shared<Node>(NodeType::ANY);
    }
    if (is_type(node, {NodeType::KEYWORD}))
    {
        if (node->ID.value == "ret")
        {
            if (node->right == nullptr)
            {
                return std::make_shared<Node>(NodeType::EMPTY);
            }
            
            return get_type(node->right);
        }
    }
    if (is_type(node, {NodeType::BLOCK}))
    {
        for (auto elem : node->BLOCK.nodes)
        {
            auto type = get_type(elem);
            if (type->type == NodeType::ERROR)
            {
                return type;
            }
        }

        return get_type(node->BLOCK.nodes[node->BLOCK.nodes.size()-1]);
    }
    if (is_type(node, {NodeType::IF_STATEMENT}))
    {
        auto left = get_type(node->left->FUNC_CALL.args[0]);
        if (left->type == NodeType::ERROR)
        {
            return left;
        }

        auto right = get_type(node->right);
        if (right->type == NodeType::ERROR)
        {
            return right;
        }

        return right;
    }
    if (is_type(node, {NodeType::IF_BLOCK}))
    {
        std::vector<std::shared_ptr<Node>> types;
        for (auto cond : node->right->BLOCK.nodes)
        {
            auto left = std::make_shared<Node>(NodeType::ERROR);
            if (cond->left->type == NodeType::ID && cond->left->ID.value == "default")
            {
                left->type = NodeType::STRING;
            }
            else
            {
                left = get_type(cond->left);
            }

            if (left->type == NodeType::ERROR)
            {
                return left;
            }

            auto right = get_type(cond->right);
            if (right->type == NodeType::ERROR)
            {
                return right;
            }

            types.push_back(right);
        }

        std::sort(types.begin(), types.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
        types.erase(std::unique(types.begin(), types.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), types.end());

        auto final_type = std::make_shared<Node>(NodeType::ERROR);
        
        if (types.size() == 0)
        {
            errors.push_back(make_error("Syntax", "If blocks must contain at least one condition"));
            return final_type;
        }
        if (types.size() > 1)
        {
            final_type->type = NodeType::COMMA_LIST;
            final_type->COMMA_LIST.nodes = types;
        }
        else
        {
            final_type = types[0];
        }

        return final_type;
    }
    
    return std::make_shared<Node>(NodeType::ANY);
}

bool Typechecker::key_compare (std::unordered_map<std::string, std::shared_ptr<Node>> const &lhs, std::unordered_map<std::string, std::shared_ptr<Node>> const &rhs) {

    auto pred = [this] (auto a, auto b)
                   { return (a.first == b.first) && this->match_types(a.second, b.second); };

    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

bool Typechecker::match_types(std::shared_ptr<Node> type_a, std::shared_ptr<Node> type_b)
{
    if (type_a->type == NodeType::COMMA_LIST && type_b->type == NodeType::COMMA_LIST)
    {
        if (type_a->COMMA_LIST.nodes.size() != type_b->COMMA_LIST.nodes.size())
        {
            return false;
        }

        for (int i = 0; i < type_a->COMMA_LIST.nodes.size(); i++)
        {
            if (!match_types(type_a->COMMA_LIST.nodes[i], type_b->COMMA_LIST.nodes[i]))
            {
                return false;
            }
        }

        return true;
    }

    if (type_a->type == NodeType::COMMA_LIST)
    {
        for (auto elem : type_a->COMMA_LIST.nodes)
        {
            if (match_types(elem, type_b))
            {
                return true;
            }
        }

        return false;
    }
    if (type_a->type == NodeType::LIST && type_b->type == NodeType::LIST)
    {
        if (type_a->LIST.nodes.size() > 0 && type_a->LIST.nodes[0]->type == NodeType::ANY)
        {
            return true;
        }

        if (type_a->LIST.nodes.size() != type_b->LIST.nodes.size())
        {
            return false;
        }

        for (int i = 0; i < type_a->LIST.nodes.size(); i++)
        {
            if (!(match_types(type_a->LIST.nodes[i], type_b->LIST.nodes[i])))
            {
                return false;
            }
        }

        return true;
    }
    if (type_a->type == NodeType::OBJECT && type_b->type == NodeType::OBJECT)
    {
        return key_compare(type_a->OBJECT.properties, type_b->OBJECT.properties);
    }

    if (type_a->type == NodeType::FUNC_T && type_b->type == NodeType::FUNC_T)
    {
        auto params_a = std::make_shared<Node>(NodeType::LIST);
        auto params_b = std::make_shared<Node>(NodeType::LIST);;

        for (auto param : type_a->FUNC_T.params)
        {
            params_a->LIST.nodes.push_back(param.second);
        }
        for (auto param : type_b->FUNC_T.params)
        {
            params_b->LIST.nodes.push_back(param.second);
        }

        if (!match_types(params_a, params_b))
        {
            return false;
        }
        if (!match_types(type_a->FUNC_T.return_type, type_b->FUNC_T.return_type))
        {
            return false;
        }

        return true;
    }

    if (type_a->type == NodeType::ANY || type_b->type == NodeType::ANY)
    {
        return true;
    }
    if (type_a->type == type_b->type)
    {
        return true;
    }

    return false;
}

bool Typechecker::run()
{
    for (auto node: nodes)
    {
        update_loc(node);
        auto type = get_type(node);
        if (type->type == NodeType::ERROR)
        {
            for (auto error : errors) 
            {
                std::cout << error << "\n" << std::flush;
            }

            return false;
        }
    }

    return true;
}

std::string Typechecker::make_error(std::string name, std::string message)
{
    return name + " Error in '" + file_name + "' (" + std::to_string(line) + ", " + std::to_string(column) + "): " + message;
}