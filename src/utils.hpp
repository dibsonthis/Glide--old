#pragma once
#include <memory>
#include <algorithm>
#include "Node.hpp"
#include "errors.hpp"

bool is_type(std::shared_ptr<Node> node, std::vector<NodeType> types);
bool is_underscore(std::shared_ptr<Node> node);