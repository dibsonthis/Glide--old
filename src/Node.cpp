#include "Node.hpp"

std::string Node::repr()
{
    switch(type) {
        case (NodeType::ID):
        {
            return ID.value;
        }
        case (NodeType::VARIABLE):
        {
            return VAR.value->repr();
        }
        case (NodeType::INT):
        {
            return std::to_string(INT.value);
        }
        case (NodeType::FLOAT):
        {
            return std::to_string(FLOAT.value);
        }
        case (NodeType::BOOL):
        {
            return (BOOL.value ? "true" : "false");
        }
        case (NodeType::STRING):
        {
            return "'" + STRING.value + "'";
        }
        case (NodeType::LIST):
        {
            std::string value = "[ ";
            for (std::shared_ptr<Node> child : LIST.nodes)
            {
                value += child->repr() + " ";
            }
            value += "]";
            return value;
        }
        case (NodeType::COMMA_LIST):
        {
            std::string value = "( ";
            for (std::shared_ptr<Node> child : LIST.nodes)
            {
                value += child->repr() + " ";
            }
            value += ")";
            return value;
        }
        case (NodeType::BLOCK):
        {
            std::string value = "[ ";
            for (std::shared_ptr<Node> child : BLOCK.nodes)
            {
                value += child->repr() + " ";
            }
            value += "]";
            return value;
        }
        case (NodeType::OP):
        {
            return OP.value;
        }
        case (NodeType::PARTIAL_OP):
        {
            return OP.value;
        }
        case (NodeType::ERROR):
        {
            return "'invalid/error expression'";
        }
        case (NodeType::START_OF_FILE):
        {
            return "";
        }
        case (NodeType::END_OF_FILE):
        {
            return "";
        }

        default:
        {
            return "<undefined repr>";
        }
    }

}

void Node::print()
{
    switch(type) {
        case (NodeType::ID):
        {
            std::cout << ID.value;
            break;
        }
        case (NodeType::VARIABLE):
        {
            VAR.value->print();
            break;
        }
        case (NodeType::INT):
        {
            std::cout << std::to_string(INT.value);
            break;
        }
        case (NodeType::FLOAT):
        {
            std::cout << std::to_string(FLOAT.value);
            break;
        }
        case (NodeType::BOOL):
        {
            std::cout << (BOOL.value ? "true" : "false");
            break;
        }
        case (NodeType::STRING):
        {
            std::cout << '"' << STRING.value << '"';
            break;
        }
        case (NodeType::LIST):
        {
            std::cout << "[ ";
            for (std::shared_ptr<Node> child : LIST.nodes)
            {
                child->print();
                std::cout << " ";
            }
            std::cout << "]";
            break;
        }
        case (NodeType::COMMA_LIST):
        {
            std::cout << "( ";
            for (std::shared_ptr<Node> child : COMMA_LIST.nodes)
            {
                child->print();
                std::cout << " ";
            }
            std::cout << ")";
            break;
        }
        case (NodeType::BLOCK):
        {
            std::cout << "{ ";
            for (std::shared_ptr<Node> child : BLOCK.nodes)
            {
                child->print();
                std::cout << " ";
            }
            std::cout << "}";
            break;
        }
        case (NodeType::OP):
        {
            std::cout << OP.value;
            break;
        }
        case (NodeType::PARTIAL_OP):
        {
            std::cout << OP.value;
            break;
        }
        case (NodeType::EMPTY):
        {
            std::cout << "EMPTY";
            break;
        }
        case (NodeType::BREAK):
        {
            std::cout << "BREAK";
            break;
        }
        case (NodeType::RETURN):
        {
            std::cout << "RETURN";
            break;
        }
        case (NodeType::ITERATOR):
        {
            std::cout << "ITERATOR";
            break;
        }
        case (NodeType::TYPE):
        {
            std::cout << TYPE.name << " - { ";
            for (auto child : TYPE.interface)
            {
                std::cout << child.first << ": " << child.second->TYPE.name;
                std::cout << " ";
            }
            std::cout << "}";
            break;
        }
        case (NodeType::ERROR):
        {
            for (auto error : ERROR.errors)
            {
                std::cout << error << '\n';
            }
            break;
        }
        case (NodeType::START_OF_FILE):
        {
            break;
        }
        case (NodeType::END_OF_FILE):
        {
            break;
        }

        default:
        {
            break;
        }
    }

}

