#pragma once
#include <memory>
#include "Node.hpp"
#include "utils.hpp"

std::shared_ptr<Node> custom_error(std::shared_ptr<Node> node, std::string message);
std::string error_message(std::shared_ptr<Node> node, std::string file_name, std::string message);