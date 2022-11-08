#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include <unordered_map>
#include <iostream>
#include <memory>

#include "Node.hpp"

class Lexer
{
	std::string source;
	int source_length;
	int index = 0;
	char current_char;
	int line = 1;
	int column = 1;
	std::vector<std::string> errors;
	bool debug = false;

	void error_and_exit(std::string message);

	void error_and_continue(std::string message);

	void advance();

	char peek(int n = 1);

	void build_identifier();

	void build_number();

	void build_string();

	void handle_line_comment();

	void handle_block_comment();

public:

	Lexer() = default;
	
	Lexer(std::string src, bool is_file = true)
	{
		if (is_file)
		{
			load_source(src);
			file_name = src;
		}
		else
		{
			source = src;
			file_name = "stdin";
		}

		source_length = source.length();
		index = 0;
		current_char = source[0];
		line = 1;
		column = 1;
	}

	Lexer(std::string file_name, std::string path, bool is_file = true) 
	: file_name(file_name), path(path) 
	{
		if (is_file)
		{
			load_source(path);
			file_name = file_name;
		}
		else
		{
			source = path;
			file_name = "stdin";
		}

		source_length = source.length();
		index = 0;
		current_char = source[0];
		line = 1;
		column = 1;
	}

	std::string path;

	std::string file_name;
	
	std::vector<std::shared_ptr<Node>> nodes;

	void init(std::string source);

	void load_source(std::string filename);

	void check_for_errors();

	std::string& get_file_name();

	std::vector<std::shared_ptr<Node>> get_nodes();

	std::string& get_source();

	void tokenize();

	void delete_nodes();

	void print_nodes();
};
