#include "StackObject.hpp"

std::string StackObject::repr()
{
    switch(type)
    {
        case SO_Type::BOOL:
        {
            return (BOOL.value ? "true" : "false");
        }
        case SO_Type::INT:
        {
            return std::to_string(INT.value);
        }
        case SO_Type::FLOAT:
        {
            return std::to_string(FLOAT.value);
        }
        case SO_Type::STRING:
        {
            return STRING.value;
        }
        case SO_Type::OP:
        {
            return OP.repr;
        }
        case SO_Type::EMPTY:
        {
            return "EMPTY";
        }
        case SO_Type::LIST:
        {
            std::string repr = "[ ";
            for (auto elem : LIST.objects)
            {
                std::string elem_repr = "";
                if (elem->type == SO_Type::STRING)
                {
                    elem_repr = "\"" + elem->repr() + "\"";
                }
                else
                {
                    elem_repr = elem->repr();
                }
                
                repr += (elem_repr + " ");
            }
            repr += "]";
            return repr;
        }
        case SO_Type::COMMA_LIST:
        {
            std::string repr = "( ";
            for (auto elem : COMMA_LIST.objects)
            {
                repr += (elem->repr() + " ");
            }
            repr += ")";
            return repr;
        }
        case SO_Type::OBJECT:
        {
            std::string repr = "{\n";
            for (auto elem : OBJECT.properties)
            {
                if (elem.second->type == SO_Type::OBJECT)
                {
                    repr += ("  " + elem.first  + ": { object }\n");
                }
                else
                {
                    repr += ("  " + elem.first + ": " + elem.second->repr() + "\n");
                }
            }
            repr += "}";
            return repr;
        }
        case SO_Type::FUNCTION:
        {
            std::string repr = "fx {\n";
            repr += ("  name: " + FUNCTION.name + "\n");
            repr += "  parameters: [";
            for (auto param : FUNCTION.parameters)
            {
                repr += (param + " ");
            }
            repr += "]\n";
            repr += "  arguments: [ ";
            for (auto arg : FUNCTION.arguments)
            {
                repr += (arg->repr() + " ");
            }
            repr += "]\n}";
            return repr;
        }
        default:
        {
            return "<no repr>";
            break;
        }
    }
}

Operation_Type node_op_type_to_so_op_type(NodeType type)
{
    switch (type)
    {
        case NodeType::PLUS:
        {
            return Operation_Type::ADD;
        }
        case NodeType::MINUS:
        {
            return Operation_Type::SUB;
        }
        case NodeType::STAR:
        {
            return Operation_Type::MUL;
        }
        case NodeType::SLASH:
        {
            return Operation_Type::DIV;
        }
        case NodeType::POS:
        {
            return Operation_Type::POS;
        }
        case NodeType::NEG:
        {
            return Operation_Type::NEG;
        }
        case NodeType::EXCLAMATION:
        {
            return Operation_Type::NOT;
        }
        case NodeType::DOUBLE_DOT:
        {
            return Operation_Type::RANGE;
        }
        case NodeType::L_ANGLE:
        {
            return Operation_Type::LT;
        }
        case NodeType::R_ANGLE:
        {
            return Operation_Type::GT;
        }
        case NodeType::LT_EQUAL:
        {
            return Operation_Type::LTE;
        }
        case NodeType::GT_EQUAL:
        {
            return Operation_Type::GTE;
        }
        case NodeType::EQ_EQ:
        {
            return Operation_Type::EQ_EQ;
        }
        case NodeType::NOT_EQUAL:
        {
            return Operation_Type::NOT_EQ;
        }
        case NodeType::EQUAL:
        {
            return Operation_Type::EQUAL;
        }
        case NodeType::RIGHT_ARROW:
        {
            return Operation_Type::ARROW;
        }
        case NodeType::AND:
        {
            return Operation_Type::AND;
        }
        case NodeType::OR:
        {
            return Operation_Type::OR;
        }
    }
}

std::shared_ptr<StackObject> node_to_stack_object(std::shared_ptr<Node> node)
{
    if (node == nullptr)
    {
        return so_make_empty();
    }
    
    auto object = std::make_unique<StackObject>();
    object->line = node->line;
    object->column = node->column;

    switch(node->type)
    {
        case NodeType::ID:
        {
            object->type = SO_Type::STRING;
            object->STRING.value = node->ID.value;
        } break;

        case NodeType::OP:
        {
            object->type = SO_Type::OP;
            object->OP.repr = node->OP.value;
            object->OP.type = node_op_type_to_so_op_type(node->OP.op_type);
            object->OP.left = node_to_stack_object(node->left);
            object->OP.right = node_to_stack_object(node->right);

        } break;

        case NodeType::PARTIAL_OP:
        {
            object->type = SO_Type::OP;
            object->OP.repr = node->OP.value;
            object->OP.type = node_op_type_to_so_op_type(node->OP.op_type);
            object->OP.left = node_to_stack_object(node->left);
            object->OP.right = node_to_stack_object(node->right);

            if (is_type(node, {NodeType::POS}) 
                || is_type(node, {NodeType::NEG})
                || is_type(node, {NodeType::EXCLAMATION}))
            {
                object->OP.is_unary = true;
            }

            if (is_type(node, {NodeType::PLUS_PLUS}) 
                || is_type(node, {NodeType::MINUS_MINUS}))
            {
                object->OP.is_post_fix = true;
            }

        } break;

        case NodeType::INT:
        {
            object->type = SO_Type::INT;
            object->INT.value = node->INT.value;
        } break;

        case NodeType::FLOAT:
        {
            object->type = SO_Type::FLOAT;
            object->FLOAT.value = node->FLOAT.value;
        } break;

        case NodeType::BOOL:
        {
            object->type = SO_Type::BOOL;
            object->BOOL.value = node->BOOL.value;
        } break;

        case NodeType::STRING:
        {
            object->type = SO_Type::STRING;
            object->STRING.value = node->STRING.value;
        } break;

        case NodeType::EMPTY:
        {
            object->type = SO_Type::EMPTY;
        } break;
    }

    return object;
}

std::shared_ptr<StackObject> so_make_bool(bool value)
{
    auto object = std::make_shared<StackObject>(SO_Type::BOOL);
    object->BOOL.value = value;
    return object;
}

std::shared_ptr<StackObject> so_make_int(long value)
{
    auto object = std::make_shared<StackObject>(SO_Type::INT);
    object->INT.value = value;
    return object;
}

std::shared_ptr<StackObject> so_make_float(double value)
{
    auto object = std::make_shared<StackObject>(SO_Type::FLOAT);
    object->FLOAT.value = value;
    return object;
}

std::shared_ptr<StackObject> so_make_string(std::string value)
{
    auto object = std::make_shared<StackObject>(SO_Type::STRING);
    object->STRING.value = value;
    return object;
}

std::shared_ptr<StackObject> so_make_list()
{
    auto object = std::make_shared<StackObject>(SO_Type::LIST);
    return object;
}

std::shared_ptr<StackObject> so_make_comma_list()
{
    auto object = std::make_shared<StackObject>(SO_Type::COMMA_LIST);
    return object;
}

std::shared_ptr<StackObject> so_make_object()
{
    auto object = std::make_shared<StackObject>(SO_Type::OBJECT);
    return object;
}

std::shared_ptr<StackObject> so_make_function()
{
    auto object = std::make_shared<StackObject>(SO_Type::FUNCTION);
    return object;
}

std::shared_ptr<StackObject> so_make_empty()
{
    auto object = std::make_shared<StackObject>(SO_Type::EMPTY);
    return object;
}