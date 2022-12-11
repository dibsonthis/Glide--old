#include "Lexer.hpp"

void Lexer::load_source(std::string filename)
{
	std::ifstream stream(filename);
	std::string source_str((std::istreambuf_iterator<char>(stream)),
		std::istreambuf_iterator<char>());
	source = source_str;
}

void Lexer::error_and_exit(std::string message)
{
	std::string error_message = "\n\n[Lexer] Lexical Error in '" + file_name + "' @ (" + std::to_string(line) + ", " + std::to_string(column) + "): " + message;
	std::cout << error_message;
	std::cout << "\nCompilation failed. Press any key to exit...";
	std::cin.get();
	exit(1);
}

void Lexer::error_and_continue(std::string message)
{
	std::string error_message = "[Lexer] Lexical Error in '" + file_name + "' @ (" + std::to_string(line) + ", " + std::to_string(column) + "): " + message;
	errors.push_back(error_message);
}

void Lexer::advance()
{
	index++;
	column++;
	if (index >= source_length)
	{
		current_char = '\0';
	}
	else if (current_char == '\n')
	{
		line++;
		column = 0;
		advance(); // consume '\n'
	}
	else
	{
		current_char = source[index];
	}
}

char Lexer::peek(int n)
{
	int peek_index = index + n;
	if (peek_index >= source_length)
	{
		return '\0';
	}
	else
	{
		return source[peek_index];
	}
}

void Lexer::build_identifier()
{
	std::shared_ptr<Node> node(new Node(NodeType::ID, line, column));

	std::string name = std::string();

	while (isalpha(current_char) || current_char == '_' || isdigit(current_char))
	{
		name += current_char;
		advance();
	}

	if (name == "true")
	{
		node->type = NodeType::BOOL;
		node->BOOL.value = true;
	}

	else if (name == "false")
	{
		node->type = NodeType::BOOL;
		node->BOOL.value = false;
	}
	else
	{
		node->ID.value = name;
	}

	nodes.push_back(node);
}

void Lexer::build_number()
{
	std::shared_ptr<Node> node(new Node(line, column));

	std::string value;
	int num_dots = 0;

	while (isdigit(current_char) || current_char == '.')
	{
		value += current_char;

		if (current_char == '.')
		{
			num_dots++;
		}

		advance();

		if (current_char == '.' && peek(1) == '.' && peek(2) == '.')
		{
			break;
		}

		if (current_char == '.' && peek(1) == '.')
		{
			break;
		}

	}

	if (num_dots == 0)
	{
		node->type = NodeType::INT;
		node->INT.value = std::stol(value);
	}
	else if (num_dots == 1)
	{
		node->type = NodeType::FLOAT;
		node->FLOAT.value = std::stod(value);
	}
	else
	{
		node->type = NodeType::ERROR;
		error_and_continue("Unacceptable number of dots in number node.");
	}

	nodes.push_back(node);
}

void Lexer::build_string()
{
	std::shared_ptr<Node> node(new Node(NodeType::STRING, line, column));

	std::string str = std::string();

	advance();

	while (current_char != '"')
	{
		if (current_char == '\0')
		{
			node->type = NodeType::ERROR;
			error_and_continue("Warning: Missing end '\"', end of file reached.");
			return;
		}

		str += current_char;
		advance();
	}

	advance();

	node->STRING.value = std::string();

	for (int i = 0; i < (str).length(); i++)
	{
		if ((str)[i] == '\\' && (str)[i + 1] == 'n')
		{
			(str)[i] = '\n';
			(node->STRING.value).push_back('\n');
			i++;
		}
		else if ((str)[i] == '\\' && (str)[i + 1] == 'r')
		{
			(str)[i] = '\r';
			(node->STRING.value).push_back('\r');
			i++;
		}
		else if ((str)[i] == '\\' && (str)[i + 1] == 't')
		{
			(str)[i] = '\t';
			(node->STRING.value).push_back('\t');
			i++;
		}
		else
		{
			(node->STRING.value).push_back((str)[i]);
		}
	}

	nodes.push_back(node);
}

void Lexer::handle_line_comment()
{
	advance();
	advance();

	while (current_char != '\n')
	{
		if (current_char == '\0')
		{
			return;
		}

		advance();
	}

	return;
}

void Lexer::handle_block_comment()
{
	advance(); // consume '/'
	advance(); // consume '*'

	while (!(current_char == '*' && peek() == '/'))
	{
		if (current_char == '\0')
		{
			error_and_continue("Warning: No end to block comment, end of file reached.");
			return;
		}
		if (current_char == '/' && peek() == '*')
		{
			handle_block_comment();
		}

		advance();
	}

	advance(); // consume '*'
	advance(); // consume '/'

	return;
}

void Lexer::init(std::string src)
{
	source = src;
	file_name = "stdin";

	source_length = source.length();
	index = 0;
	current_char = source[0];
	line = 1;
	column = 1;
}

void Lexer::check_for_errors()
{
	if (errors.size() > 0)
	{
		std::cout << "[Lexer] Lexing unsuccessful - " + std::to_string(errors.size()) + " error(s) found.\n";
		for (std::string error_message : errors)
		{
			std::cout << error_message << '\n';
		}
	}
	else
	{
		if (debug)
			std::cout << "[Lexer] Lexing successful - no errors found.\n";
	}
}

std::string& Lexer::get_file_name()
{
	return file_name;
}

std::vector<std::shared_ptr<Node>> Lexer::get_nodes()
{
	return nodes;
}

std::string& Lexer::get_source()
{
	return source;
}

void Lexer::tokenize()
{
	std::shared_ptr<Node> SOF(new Node(NodeType::START_OF_FILE, line, column));
	nodes.push_back(SOF);

	while (current_char != '\0')
	{
		if (current_char == ' ' || current_char == '\t')
		{
			advance();
		}
		else if (isalpha(current_char) || current_char == '_')
		{
			build_identifier();
		}
		else if (isdigit(current_char))
		{
			build_number();
		}
		else if (current_char == '/' && peek() == '/')
		{
			handle_line_comment();
		}
		else if (current_char == '/' && peek() == '*')
		{
			handle_block_comment();
		}
		else if (current_char == '"')
		{
			build_string();
		}
		else if (current_char == '.' && peek() == '.' && peek(2) == '.')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::TRIPLE_DOT;
			node->OP.value = "...";
			nodes.push_back(node);
			advance(); // consume .
			advance(); // consume .
			advance(); // consume .
		}
		else if (current_char == '{' && peek() == '{')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::DOUBLE_L_BRACE;
			node->OP.value = "{{";
			nodes.push_back(node);
			advance(); // consume {
			advance(); // consume {
		}
		else if (current_char == '}' && peek() == '}')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::DOUBLE_R_BRACE;
			node->OP.value = "}}";
			nodes.push_back(node);
			advance(); // consume }
			advance(); // consume }
		}
		else if (current_char == '.' && peek() == '.')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::DOUBLE_DOT;
			node->OP.value = "..";
			nodes.push_back(node);
			advance(); // consume .
			advance(); // consume .
		}
		else if (current_char == '=' && peek() == '=')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::EQ_EQ;
			node->OP.value = "==";
			nodes.push_back(node);
			advance(); // consume =
			advance(); // consume =
		}
		else if (current_char == '!' && peek() == '=')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::NOT_EQUAL;
			node->OP.value = "!=";
			nodes.push_back(node);
			advance(); // consume !
			advance(); // consume =
		}
		else if (current_char == '<' && peek() == '=')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::LT_EQUAL;
			node->OP.value = "<=";
			nodes.push_back(node);
			advance(); // consume <
			advance(); // consume =
		}
		else if (current_char == '>' && peek() == '=')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::GT_EQUAL;
			node->OP.value = ">=";
			nodes.push_back(node);
			advance(); // consume >
			advance(); // consume =
		}
		else if (current_char == '+' && peek() == '=')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::PLUS_EQ;
			node->OP.value = "+=";
			nodes.push_back(node);
			advance(); // consume +
			advance(); // consume =
		}
		else if (current_char == '-' && peek() == '=')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::MINUS_EQ;
			node->OP.value = "-=";
			nodes.push_back(node);
			advance(); // consume -
			advance(); // consume =
		}
		else if (current_char == '+' && peek() == '+')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::PLUS_PLUS;
			node->OP.value = "++";
			nodes.push_back(node);
			advance(); // consume +
			advance(); // consume +
		}
		else if (current_char == '-' && peek() == '-')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::MINUS_MINUS;
			node->OP.value = "--";
			nodes.push_back(node);
			advance(); // consume -
			advance(); // consume -
		}
		else if (current_char == '=' && peek() == '>')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::RIGHT_ARROW;
			node->OP.value = "=>";
			nodes.push_back(node);
			advance(); // consume =
			advance(); // consume >
		}
		else if (current_char == '-' && peek() == '>')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::RIGHT_ARROW_SINGLE;
			node->OP.value = "->";
			nodes.push_back(node);
			advance(); // consume -
			advance(); // consume >
		}
		else if (current_char == '&' && peek() == '&')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::AND;
			node->OP.value = "&&";
			nodes.push_back(node);
			advance(); // consume &
			advance(); // consume &
		}
		else if (current_char == '|' && peek() == '|')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::OR;
			node->OP.value = "||";
			nodes.push_back(node);
			advance(); // consume |
			advance(); // consume |
		}
		else if (current_char == ':' && peek() == ':')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::DOUBLE_COLON;
			node->OP.value = "::";
			nodes.push_back(node);
			advance(); // consume :
			advance(); // consume :
		}
		else if (current_char == '=')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::EQUAL;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '(')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::L_PAREN;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == ')')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::R_PAREN;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '{')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::L_BRACE;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '}')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::R_BRACE;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '[')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::L_BRACKET;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == ']')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::R_BRACKET;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '<')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::L_ANGLE;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '>')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::R_ANGLE;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '.')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::DOT;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '\\')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::BACKSLASH;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '\'')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::APOSTROPHE;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '!')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::EXCLAMATION;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '@')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::AT;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '#')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::HASH;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '$')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::DOLLAR;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '^')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::CARET;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '?')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::QUESTION;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '%')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::PERCENT;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '"')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::DOUBLE_QUOTE;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '-')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::MINUS;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '+')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::PLUS;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '/')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::SLASH;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '*')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::STAR;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == ',')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::COMMA;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == '|')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::PIPE;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == ':')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::COLON;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else if (current_char == ';')
		{
			std::shared_ptr<Node> node(new Node(NodeType::OP, line, column));
			node->OP.op_type = NodeType::SEMICOLON;
			node->OP.value = current_char;
			nodes.push_back(node);
			advance(); // consume symbol
		}
		else
		{
			error_and_continue("Unexpected node '" + std::string(1, current_char) + "'.");
			std::shared_ptr<Node> node(new Node(NodeType::ERROR, line, column));
			nodes.push_back(node);
			advance();
		}
	}

	std::shared_ptr<Node> node(new Node(NodeType::END_OF_FILE, line, column));
	nodes.push_back(node);

	check_for_errors();
}

void Lexer::print_nodes()
{
	std::cout << "[ ";
	for (std::shared_ptr<Node> node : nodes)
	{
		node->print();
		std::cout << " ";
	}
	std::cout << "]" << std::flush;
}
