#include "errors.hpp"

std::shared_ptr<Node> custom_error(std::shared_ptr<Node> node, std::string message)
{
    std::shared_ptr<Node> error(new Node(NodeType::ERROR));
    error->line = node->line;
    node->column = node->column;
    std::string _message = "Error [" + std::to_string(node->line) + ":" + std::to_string(node->column) + "] - " + message;
    error->ERROR.errors.push_back(_message);
    return error;
}

std::string error_message(std::shared_ptr<Node> node, std::string file_name, std::string message)
{
    std::string line = node == nullptr ? "-" : std::to_string(node->line);
    std::string column = node == nullptr ? "-" : std::to_string(node->column);
    return "Error in '" + file_name + "' [" + line + ":" + column + "] - " + message + "\n";
}