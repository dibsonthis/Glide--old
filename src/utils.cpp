#include "utils.hpp"

bool is_type(std::shared_ptr<Node> node, std::vector<NodeType> types)
{
    // TODO: remove this, cause things will silently fail
    if (node == nullptr)
    {
        return false;
    }
    
    if (types.size() == 1 && types[0] == NodeType::OP)
    {
        return node->type == NodeType::OP;
    }

    if (types.size() == 1 && types[0] == NodeType::PARTIAL_OP)
    {
        return node->type == NodeType::PARTIAL_OP;
    }

    if (node->type == NodeType::OP || node->type == NodeType::PARTIAL_OP)
    {
        if (std::count(types.begin(), types.end(), node->OP.op_type)) return true;
    }
    else
    {
        if (std::count(types.begin(), types.end(), node->type)) return true;
    }

    return false;
}

bool is_underscore(std::shared_ptr<Node> node)
{
    // TODO: remove this, cause things will silently fail
    if (node == nullptr)
    {
        return false;
    }
    
    return (is_type(node, {NodeType::ID}) && node->ID.value == "_");
}