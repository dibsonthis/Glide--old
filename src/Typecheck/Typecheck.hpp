#pragma once
#include "../Node.hpp"
#include "../utils.hpp"

struct Type
{
    Type() {}
    Type(std::string name, std::shared_ptr<Node> value) : name(name), value(value) {}

    std::string name;
    std::shared_ptr<Node> value;
};

struct Typechecker 
{
    std::string file_name;
    std::vector<std::string> errors;
    std::vector<std::shared_ptr<Node>> nodes;
    std::unordered_map<std::string, Type> symbol_table;
    int line, column;

    Typechecker(std::string file_name, std::vector<std::shared_ptr<Node>> nodes) : file_name(file_name), nodes(nodes) {}

    void update_loc(std::shared_ptr<Node> node);
    bool is_simple(std::string type);
    bool simple_check(std::string type, std::shared_ptr<Node> node);
    bool validate(std::shared_ptr<Node> type, std::shared_ptr<Node> value);
    bool typecheck(std::shared_ptr<Node> node);
    std::string make_error(std::string name, std::string message);
    void run();
};