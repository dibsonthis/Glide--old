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
        case NodeType::ANY: {
            return "any";
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

std::shared_ptr<Node> Typechecker::get_type(std::shared_ptr<Node> node)
{
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

        if (symbol_table.find(node->ID.value) == symbol_table.end()) 
        {
            return std::make_shared<Node>(NodeType::ERROR);
        } 
        else 
        {
            return symbol_table[node->ID.value];
        }
    }
    if (is_type(node, {NodeType::INT, NodeType::FLOAT, NodeType::STRING, NodeType::BOOL}))
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
        // reduce list to sorted unique types
        list->LIST.nodes.erase(std::unique(list->LIST.nodes.begin(), list->LIST.nodes.end(), [this] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return match_types(type_a, type_b);}), list->LIST.nodes.end());
        std::sort(list->LIST.nodes.begin(), list->LIST.nodes.end(), [] (std::shared_ptr<Node>& type_a, std::shared_ptr<Node>& type_b) {return type_a->type < type_b->type;});
        return list;
    }
    if (is_type(node, {NodeType::LAMBDA}))
    {
        auto func_type = std::make_shared<Node>(NodeType::FUNC_T);
        std::shared_ptr<Node> list_node;

        // check if return type present
        if (is_type(node->left, {NodeType::DOUBLE_COLON}))
        {
            auto ret_type = get_type(node->left->right);
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
                func_type->FUNC_T.params[elem->left->ID.value] = get_type(elem->right);
                
                // remove type
                elem = elem->left;
            }
            else
            {
                func_type->FUNC_T.params[elem->ID.value] = std::make_shared<Node>(NodeType::ANY);
            }
        }

        std::vector<std::pair<std::string, std::shared_ptr<Node>>> ordered_keys;

        for (auto& prop : func_type->FUNC_T.params)
        {
            ordered_keys.push_back(prop);
        }

        std::sort(ordered_keys.begin(), ordered_keys.end(), [] (std::pair<std::string, std::shared_ptr<Node>> const &lhs, std::pair<std::string, std::shared_ptr<Node>> const &rhs) {return lhs.first < rhs.first;});
        func_type->FUNC_T.params.clear();

        for (auto& elem : ordered_keys)
        {
            func_type->FUNC_T.params[elem.first] = elem.second;
        }

        // check that the function actually returns the same type as the return type
        Typechecker tc(file_name, node->right->BLOCK.nodes);

        for (auto param : func_type->FUNC_T.params)
        {
            tc.symbol_table[param.first] = param.second;
        }

        for (auto node : tc.nodes)
        {
            tc.update_loc(node);
            bool check = tc.typecheck(node);
            if (tc.errors.size() > 0) {
                for (auto error : tc.errors) {
                    std::cout << error << "\n";
                }
                return std::make_shared<Node>(NodeType::ERROR);
            }
        }

        auto type = tc.get_type(tc.nodes[tc.nodes.size()-1]);

        if (!tc.match_types(type, func_type->FUNC_T.return_type))
        {
            errors.push_back(make_error("Type", "Lambda must return value of type '" + node_type_to_string(func_type->FUNC_T.return_type) + "' but instead returns '" + node_type_to_string(type) + "'"));
            return std::make_shared<Node>(NodeType::ERROR);
        }

        return func_type;
    }
    if (is_type(node, {NodeType::FUNC_CALL}))
    {
        auto name = node->FUNC_CALL.name->ID.value;
        auto args = node->FUNC_CALL.args;

        auto func_type = std::make_shared<Node>(NodeType::ANY);

        if (symbol_table.find(name) == symbol_table.end()) 
        {
            return func_type;
        } 

        func_type = symbol_table[name];

        std::vector<std::shared_ptr<Node>> param_types;

        for (auto& elem : func_type->FUNC_T.params)
        {
            param_types.push_back(elem.second);
        }

        // typecheck the args
        // TODO: additional overflow args do not get typechecked, do we want to change this?

        for (int i = 0; i < param_types.size(); i++)
        {
            if (!match_types(param_types[i], args[i]))
            {
                errors.push_back(make_error("Type", "Function '" + name + "' - Cannot assign argument of type '" + node_type_to_string(args[i]) + "' to parameter of type '" + node_type_to_string(param_types[i]) + "'"));
                return std::make_shared<Node>(NodeType::ERROR);
            }
        }

        return func_type->FUNC_T.return_type;
    }
    if (is_type(node, {NodeType::OP}))
    {
        auto left = get_type(node->left);
        auto right = get_type(node->right);

        if (is_type(node, {NodeType::PLUS}))
        {
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

            if (left->type == NodeType::STRING && right->type == NodeType::STRING)
            {
                return right;
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
                return right;
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
                return right;
            }

            if (left->type == NodeType::LIST)
            {
                return left;
            }
            
            if (right->type == NodeType::LIST)
            {
                return right;
            }

            if (left->type == NodeType::OBJECT && right->type == NodeType::OBJECT)
            {
                return right;
            }

            errors.push_back(make_error("Type", "Cannot perform '+' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
            auto error = std::make_shared<Node>(NodeType::ERROR);
            return error;
        }
        if (is_type(node, {NodeType::MINUS}))
        {
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

            errors.push_back(make_error("Type", "Cannot perform '-' on types: " + node_type_to_string(left) + ", " + node_type_to_string(right)));
            auto error = std::make_shared<Node>(NodeType::ERROR);
            return error;
        }
        if (is_type(node, {NodeType::STAR}))
        {
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
        if (is_type(node, {NodeType::STAR}))
        {
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

bool Typechecker::typecheck(std::shared_ptr<Node> node)
{
    if (is_type(node, {NodeType::INT, NodeType::FLOAT, NodeType::BOOL, NodeType::STRING}))
    {
        return true;
    }
    if (is_type(node, {NodeType::EQUAL}))
    {
        if (is_type(node->left, {NodeType::DOUBLE_COLON}))
        {
            auto var_type = get_type(node->left->left);

            if (is_type(var_type, {NodeType::ERROR}))
            {
                // new variable, check that type provided matches value type

                auto val_type = get_type(node->right);

                if (is_type(val_type, {NodeType::ERROR}))
                {
                    return false;
                }

                auto provided_type = get_type(node->left->right);

                if (match_types(provided_type, val_type))
                {
                    symbol_table[node->left->left->ID.value] = val_type;
                    // remove type details from node
                    node->left = node->left->left;
                    return true;
                }

                errors.push_back(make_error("Type", "'" + node->left->left->ID.value + "' - Cannot assign value of type '" + node_type_to_string(val_type) + "' to variable of type '" + node_type_to_string(provided_type) + "'"));
                return false;
            }

            // found variable, error because cannot change a variable's type
            errors.push_back(make_error("Type", "'" + node->left->left->ID.value + "' - Cannot reassign type"));
            return false;
        }
        else
        {
            auto var_type = get_type(node->left);
            auto val_type = get_type(node->right);

            if (is_type(val_type, {NodeType::ERROR}))
            {
                return false;
            }

            if (is_type(var_type, {NodeType::ERROR}))
            {
                // new variable
                symbol_table[node->left->ID.value] = val_type;
                return true;
            }

            // found variable, check if type matches value type
            if (match_types(var_type, val_type))
            {
                symbol_table[node->left->ID.value] = val_type;
                return true;
            }

            errors.push_back(make_error("Type", "'" + node->left->ID.value + "' - Cannot assign value of type '" + node_type_to_string(val_type) + "' to variable of type '" + node_type_to_string(var_type) + "'"));
            return false;
        }
    }
    else
    {
        auto type = get_type(node);
        if (is_type(type, {NodeType::ERROR})) {
            return false;
        }
        return true; 
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