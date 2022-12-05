#pragma once
#include "../Node.hpp"
#include "../utils.hpp"

struct Type
{
    Type() {}
    Type(std::shared_ptr<Node> allowed_type, std::shared_ptr<Node> value_type) : allowed_type(allowed_type), value_type(value_type) {}
    Type(std::shared_ptr<Node> allowed_type) : allowed_type(allowed_type) {}
    std::shared_ptr<Node> allowed_type;
    std::shared_ptr<Node> value_type;
};

struct Typechecker 
{
    std::string file_name;
    std::vector<std::string> errors;
    std::vector<std::shared_ptr<Node>> nodes;
    std::unordered_map<std::string, Type> symbol_table;
    int line, column;

    Typechecker(std::string file_name, std::vector<std::shared_ptr<Node>> nodes) : file_name(file_name), nodes(nodes) {}

    bool key_compare (std::unordered_map<std::string, std::shared_ptr<Node>> const &lhs, std::unordered_map<std::string, std::shared_ptr<Node>> const &rhs);
    std::string node_type_to_string(std::shared_ptr<Node> node);

    void update_loc(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type_dot(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type_add(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type_sub(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type_mul(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type_div(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type_mod(std::shared_ptr<Node> node);
    std::shared_ptr<Node> get_type_equality(std::shared_ptr<Node> node);

    bool match_types(std::shared_ptr<Node> type_a, std::shared_ptr<Node> type_b);
    bool typecheck(std::shared_ptr<Node> node);
    std::string make_error(std::string name, std::string message);
    void run();
};