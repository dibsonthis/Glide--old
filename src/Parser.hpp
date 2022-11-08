#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <algorithm>

#include "Lexer.hpp"
#include "utils.hpp"
#include "errors.hpp"

class Parser
{
    int index = 0;
    std::shared_ptr<Node> current_node;

public:
    std::string file_name;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::string> errors;

    Parser(std::string& file_name, std::vector<std::shared_ptr<Node>>& nodes) : file_name(file_name), nodes(nodes) 
    {
        if (nodes.size() == 0)
        {
            current_node = nullptr;
            return;
        }

        current_node = nodes[0];
    }

    void init(std::string _file_name, std::vector<std::shared_ptr<Node>> lexer_nodes);
    void advance(int n = 1);
    std::shared_ptr<Node> peek(int n = 1);
    void print_nodes();

    void error_and_continue(std::string message, int line, int column);

    NodeType binop_to_unop_type(NodeType type);
    
    void parse_paren(std::vector<NodeType> end_node_types);
    void parse_block(std::vector<NodeType> end_node_types);
    void parse_list(std::vector<NodeType> end_node_types);
    void parse_bin_op(std::vector<NodeType> types, std::vector<NodeType> end_node_types);
    void parse_un_op(std::vector<NodeType> types, std::vector<NodeType> end_node_types);
    void parse_postfix_un_op(std::vector<NodeType> types, std::vector<NodeType> end_node_types);
    void parse_arrow(std::vector<NodeType> end_node_types);
    void parse_right_arrow(std::vector<NodeType> end_node_types);
    void parse_comma(std::vector<NodeType> end_node_types);
    void parse_at(std::vector<NodeType> end_node_types);
    void parse_hash(std::vector<NodeType> end_node_types);
    void parse_not(std::vector<NodeType> end_node_types);
    void parse_equal(std::vector<NodeType> end_node_types);
    void parse_function_call(std::vector<NodeType> end_node_types);
    void parse_colon(std::vector<NodeType> end_node_types);
    void parse_keywords(std::vector<NodeType> end_node_types);
    void remove_node_type(std::vector<NodeType> types, std::vector<NodeType> end_node_types);
    void flatten_comma_list(std::shared_ptr<Node>& node);
    std::vector<std::shared_ptr<Node>> parse(std::vector<NodeType> end_node_types = {NodeType::END_OF_FILE});
};