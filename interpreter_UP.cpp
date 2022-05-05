#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

enum BlockType {SIMPLE, CONDITIONAL, ITERATION};
enum LineType {DECLARATION, ASSIGNING, READING, WRITING, DIRECTIVE};

struct Line
{
    bool is_subline;
    LineType type;
    std::vector<std::string> value;
};

struct Block
{
    BlockType type;
    std::vector<Line> value;
};

struct N_var {std::string name; double value;};
struct S_var {std::string name; char value;};
struct B_var {std::string name; bool value;};
struct L_var {std::string name; std::string value;};

struct PN_var {std::string name; double* value;};
struct PS_var {std::string name; char* value;};
struct PB_var {std::string name; bool* value;};
struct PL_var {std::string name; std::string* value;};

struct Data
{
    std::vector<N_var> n_variables;
    std::vector<S_var> s_variables;
    std::vector<B_var> b_variables;
    std::vector<L_var> l_variables;

    std::vector<PN_var> pn_variables;
    std::vector<PS_var> ps_variables;
    std::vector<PB_var> pb_variables;
    std::vector<PL_var> pl_variables;
};

std::vector<std::string> read_strings(std::string filename);
void print_strings(std::vector<std::string> strings);
bool is_valid(std::string filename);
std::vector<Line> lines_from_code(std::vector<std::string> code);
Line convert_to_line(std::string str);


std::vector<Block> blocks_from_lines(std::vector<Line> lines);
void print_blocks(std::vector<Block> blocks);
void print_lines(std::vector<Line> lines);
void error(std::string str);


bool is_decl_keyword(std::string keyword);
bool is_declared(Data data, std::string word);
std::string detect_type(Data data, std::string word);


void execute_declaration(Data& data, Line line);
void execute_writing(Data data, Line line);
void execute_reading(Data& data, Line line);
void execute_assigning(Data& data, Line line);
bool bool_expression(Data data, std::vector<std::string> expr);
bool is_number(std::string str);
double number_from_string(std::string str);


int main()
{
    std::cout << "\nUP language interpreter !\n\n";
    std::string filename; 

    do
    {
        std::cout << "Source-code filename : ";
        std::cin >> filename;
        if(filename == "q") return 0;
    } while(!is_valid(filename));

    std::cout << std::endl;

    std::vector<std::string> code = read_strings(filename);
    print_strings(code);
    std::vector<Line> lines = lines_from_code(code); 

    Data data;

    /*std::cout << std::endl;
    print_lines(lines);
    std::cout << std::endl;*/

    std::vector<Block> blocks = blocks_from_lines(lines);
    print_blocks(blocks);
}

// this function checks whether a filename
// is a valid UP language source code filename
// with .up expansion
bool is_valid(std::string filename)
{
    std::ifstream fin;
    fin.open(filename);
    bool result = fin.is_open();
    result = result && filename[filename.size() - 1] == 'p';
    result = result && filename[filename.size() - 2] == 'u';
    result = result && filename[filename.size() - 3] == '.';
    if(fin.is_open()) fin.close();
    return result;
}

// this function reads a vector of strings
// each member of this vector is a line
// in the .up file
std::vector<std::string> read_strings(std::string filename)
{
    std::ifstream fin;
    fin.open(filename);
    std::vector<std::string> result;
    std::string command;
    while(!fin.eof())
    {
        std::getline(fin, command);
        result.push_back(command);
    }
    result.pop_back();
    return result;
}

// this function prints the source code
void print_strings(std::vector<std::string> code)
{
    for(int i{}; i < code.size(); i++)
    {
        std::cout << code[i] << std::endl;
    }
}

// this function groups the source code in Blocks
// Blocks are code sections whose commands are of
// the same context(simple, cinditional, iterative)
std::vector<Line> lines_from_code(std::vector<std::string> code)
{
    std::vector<Line> lines;
    for(int i{}; i < code.size(); i++)
    {
        lines.push_back(convert_to_line(code[i]));
    }
    return lines;
}

// this function takes as input a single-line
// string then resolves it as a Line object
Line convert_to_line(std::string str)
{
    Line result;
    if(str[0] == 9)
    {
        result.is_subline = true;
        switch(str[2])
        {
            case '-':
            case 'n':
            case 's':
            case 'b':
            case 'l': result.type = DECLARATION; break;
            case 'a': result.type = ASSIGNING; break;
            case 'r': result.type = READING; break;
            case 'w': result.type = WRITING; break;
            default : result.type = DIRECTIVE; break;
        }
        std::vector<std::string> temp;
        std::string temp_s;
        for(int i = 1; i < str.size(); i++)
        {
            if(str[i] != 0x20) 
            {
                temp_s.push_back(str[i]);
            }
            else 
            {
                temp.push_back(temp_s); 
                temp_s.clear();
            } 
        }
        if(str[str.size() - 1] != 0x20) temp.push_back(temp_s);
        result.value = temp;
        temp.clear();
    }
    else
    {
        result.is_subline = false;
        switch (str[0])
        {
            case '-':
            case 'n':
            case 's':
            case 'b':
            case 'l': result.type = DECLARATION; break;
            case 'r': result.type = READING; break;
            case 'w': result.type = WRITING; break;
            default : result.type = DIRECTIVE; break;
        }
        if(str.substr(0,3) == "ass") result.type = ASSIGNING;
        if(str[0] == 'a' && str[1] == 's' && str[2] == 0x20) result.type = DIRECTIVE;
        if(str[0] == 'i' && str[1] == 'f' && str[2] == 0x20) result.type = DIRECTIVE;

        std::vector<std::string> temp;
        std::string temp_s;
        for(int i = 0; i < str.size(); i++)
        {
            if(str[i] != 0x20) 
            {
                temp_s.push_back(str[i]);
            }
            else 
            {
                temp.push_back(temp_s); 
                temp_s.clear();
            } 
        }
        if(str[str.size() - 1] != 0x20) temp.push_back(temp_s);
        result.value = temp;
        temp.clear();
    }
    return result;
}

// this function takes as input a vector of Lines
// then groups them in a vector of Blocks
// then returns the vector of Blocks, each
// member of which is a separate execution unit
std::vector<Block> blocks_from_lines(std::vector<Line> lines)
{
    std::vector<Block> blocks;
    Block current;
    current.type = SIMPLE;
    for(int i{}; i < lines.size(); i++)
    {
        if(lines[i].is_subline == 0 && lines[i].type != 4)
        {
            if(current.type == SIMPLE)
            {
                current.value.push_back(lines[i]);
            }
            else
            {
                blocks.push_back(current);
                current.type = SIMPLE;
                current.value.clear();
                current.value.push_back(lines[i]);
            }
        }
        if(lines[i].is_subline == 0 && lines[i].type == 4)
        {
            blocks.push_back(current);
            if(lines[i].value[0][0] == 'i')
            {
                current.type = CONDITIONAL;
                current.value.clear();
                current.value.push_back(lines[i]);
            }
            else
            {
                current.type = ITERATION;
                current.value.clear();
                current.value.push_back(lines[i]);
            }
        }
        if(lines[i].is_subline)
        {
            current.value.push_back(lines[i]);
        }
        if(i == lines.size() - 1)
        {
            blocks.push_back(current);
        }
    }
    return blocks;
}

// this function prints blocks
void print_blocks(std::vector<Block> blocks)
{
    for(int i{}; i < blocks.size(); i++)
    {
        std::cout << blocks[i].type << std::endl;
        for(int j{}; j < blocks[i].value.size(); j++)
        {
            std::cout << blocks[i].value[j].is_subline << "  ";
            std::cout << blocks[i].value[j].type << "  ";
            for(int k{}; k < blocks[i].value[j].value.size(); k++)
            {
                std::cout << blocks[i].value[j].value[k] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n\n";
    }
}

// this function prints lines
void print_lines(std::vector<Line> lines)
{
    for(int i{}; i < lines.size(); i++)
    {
        std::cout << lines[i].is_subline << "  "
                    << lines[i].type << "  ";
        for(int j{}; j < lines[i].value.size(); j++)
        {
            std::cout << lines[i].value[j] << " ";
        }
        std::cout << std::endl;
    }
}

// error message
void error(std::string str)
{
    std::cout << "ERROR : " << str << std::endl;
    exit();
}

// this function executes a line of type DECLARATION
void execute_declaration(Data& data, Line line)
{
    if(line.value.size() != 2)
    {
        error("syntax error");
    }
    else
    {
        std::string keyword = line.value[0];
        std::string word = line.value[1];
        if(!is_decl_keyword(keyword))
        {
            error("syntax error");
        }
        else
        {
            if(is_declared(data, word)) error("two variables with the sanme name");
            else
            {
                if(keyword == "number")
                {
                    N_var var;
                    var.name = word;
                    var.value = 0;
                    data.n_variables.push_back(var);
                }
                if(keyword == "symbol")
                {
                    S_var var;
                    var.name = word;
                    var.value = 0;
                    data.s_variables.push_back(var);
                }
                if(keyword == "bool")
                {
                    B_var var;
                    var.name = word;
                    var.value = false;
                    data.b_variables.push_back(var);
                }
                if(keyword == "line")
                {
                    L_var var;
                    var.name = word;
                    var.value = nullptr;
                    data.l_variables.push_back(var);
                }
                if(keyword == "->number")
                {
                    PN_var var;
                    var.name = word;
                    var.value = nullptr;
                    data.pn_variables.push_back(var);
                }
                if(keyword == "->symbol")
                {
                    PS_var var;
                    var.name = word;
                    var.value = nullptr;
                    data.ps_variables.push_back(var);
                }
                if(keyword == "->bool")
                {
                    PB_var var;
                    var.name = word;
                    var.value = nullptr;
                    data.pb_variables.push_back(var);
                }
                if(keyword == "->line")
                {
                    PL_var var;
                    var.name = word;
                    var.value = nullptr;
                    data.pl_variables.push_back(var);
                }
            }

        }
    }
}

// this function checks whether a string
// is a declaration keyword
bool is_decl_keyword(std::string keyword)
{
    if(keyword == "number") return true;
    if(keyword == "symbol") return true;
    if(keyword == "bool") return true;
    if(keyword == "line") return true;
    if(keyword == "->number") return true;
    if(keyword == "->symbol") return true;
    if(keyword == "->bool") return true;
    if(keyword == "->line") return true;
    return false;
}

// this function check whether there is a variable with a certain name
bool is_declared(Data data, std::string word)
{
    for(int i{}; i < data.n_variables.size(); i++)
        if(data.n_variables[i].name == word) return true;
    for(int i{}; i < data.s_variables.size(); i++)
        if(data.s_variables[i].name == word) return true;
    for(int i{}; i < data.b_variables.size(); i++)
        if(data.b_variables[i].name == word) return true;
    for(int i{}; i < data.l_variables.size(); i++)
        if(data.l_variables[i].name == word) return true;
    for(int i{}; i < data.pn_variables.size(); i++)
        if(data.pn_variables[i].name == word) return true;
    for(int i{}; i < data.ps_variables.size(); i++)
        if(data.ps_variables[i].name == word) return true;
    for(int i{}; i < data.pb_variables.size(); i++)
        if(data.pb_variables[i].name == word) return true;
    for(int i{}; i < data.pl_variables.size(); i++)
        if(data.pl_variables[i].name == word) return true;
    return false;
}

// this function returns the typename of an existing variable
std::string detect_type(Data data, std::string word)
{
    for(int i{}; i < data.n_variables.size(); i++)
        if(data.n_variables[i].name == word) return "number";
    for(int i{}; i < data.s_variables.size(); i++)
        if(data.s_variables[i].name == word) return "symbol";
    for(int i{}; i < data.b_variables.size(); i++)
        if(data.b_variables[i].name == word) return "bool";
    for(int i{}; i < data.l_variables.size(); i++)
        if(data.l_variables[i].name == word) return "line";
    for(int i{}; i < data.pn_variables.size(); i++)
        if(data.pn_variables[i].name == word) return "->number";
    for(int i{}; i < data.ps_variables.size(); i++)
        if(data.ps_variables[i].name == word) return "->symbol";
    for(int i{}; i < data.pb_variables.size(); i++)
        if(data.pb_variables[i].name == word) return "->bool";
    for(int i{}; i < data.pl_variables.size(); i++)
        if(data.pl_variables[i].name == word) return "->line";
}

// this function executes WRITING on screen
void execute_writing(Data data, Line line)
{
    std::string keyword;
    std::string word;
    if(keyword != "write" || !is_declared(data, word)) error("syntax error");
    else
    {
        for(int i{}; i < data.n_variables.size(); i++)
            if(word == data.n_variables[i].name) std::cout << data.n_variables[i].value;
        for(int i{}; i < data.s_variables.size(); i++)
            if(word == data.s_variables[i].name) std::cout << data.s_variables[i].value;
        for(int i{}; i < data.b_variables.size(); i++)
            if(word == data.b_variables[i].name) std::cout << data.b_variables[i].value;
        for(int i{}; i < data.l_variables.size(); i++)
            if(word == data.l_variables[i].name) std::cout << data.l_variables[i].value;
        for(int i{}; i < data.pn_variables.size(); i++)
            if(word == data.pn_variables[i].name) std::cout << data.pn_variables[i].value;
        for(int i{}; i < data.ps_variables.size(); i++)
            if(word == data.ps_variables[i].name) std::cout << data.ps_variables[i].value;
        for(int i{}; i < data.pb_variables.size(); i++)
            if(word == data.pb_variables[i].name) std::cout << data.pb_variables[i].value;
        for(int i{}; i < data.pl_variables.size(); i++)
            if(word == data.pl_variables[i].name) std::cout << data.pl_variables[i].value;
    }
}

// this function executes READING from keyboard
void execute_reading(Data& data, Line line)
{
    std::string keyword;
    std::string word;
    if(keyword != "read" || !is_declared(data, word)) error("syntax error");
    else
    {
        for(int i{}; i < data.n_variables.size(); i++)
            if(word == data.n_variables[i].name) std::cin >> data.n_variables[i].value;
        for(int i{}; i < data.s_variables.size(); i++)
            if(word == data.s_variables[i].name) std::cin >> data.s_variables[i].value;
        for(int i{}; i < data.b_variables.size(); i++)
            if(word == data.b_variables[i].name) std::cin >> data.b_variables[i].value;
        for(int i{}; i < data.l_variables.size(); i++)
            if(word == data.l_variables[i].name) std::cin >> data.l_variables[i].value;
    }
    
}

// this function executes ASSIGNING(incomolete)
void execute_assigning(Data& data, Line line)
{
    std::string keyword = line.value[0] + line.value[line.value.size() - 2];
    std::string word = line.value[line.value.size() - 1];
    std::vector<std::string> expression;
    for(int i{}; i < line.value.size() - 2; i++)
    {
        expression.push_back(line.value[i]);
    }
    if(is_declared(data, word) || keyword != "assign to")
    {
        error("compile error");
    }
}

// this function calculates boolean expressions(incomplete)
bool bool_expression(Data data, std::vector<std::string> expr)
{
    if((expr.size() != 3) && (expr.size() != 2))
    {
        error("syntax error");
    }
    else
    {
        if((expr.size() == 2) && (expr[0] == "not"))
        {
            if(is_declared(data, expr[1]) && (detect_type(data, expr[1]) == "bool"))
            {
                bool value;
                for(int i{}; i < data.b_variables.size(); i++)
                {
                    if(data.b_variables[i].name == expr[1])
                    {
                        value = !data.b_variables[i].value;
                        return value;
                    }
                }
            }
        }
    }
}

// this function check whether it is a number in string
bool is_number(std::string str)
{
    bool result = true;
    unsigned dots{};
    for(int i{}; i < str.size(); i++)
    {
        if((str[i] < 48 || str[i] > 57) && str[i] != '.') result = false;
        if(str[i] == '.') dots++;
        if(dots > 1) result = false;
    }
    if(str[0] == '.' || str[str.size() - 1] == '.') result = false;
    return result;
}

// this function converts a nuber expressing string to a number
double number_from_string(std::string str)
{
    double result{};
    unsigned dots{};
    for(int i{}; i < str.size(); i++)
        if(str[i] == '.') dots++;
    if(!dots)
    {
        for(int i = 0; i < str.size(); i++)
            result += (str[i] - 0x30) * pow(10, str.size() - 1 - i);
    }
    else
    {
        unsigned index{};
        for(int i{}; i < str.size(); i++)
            if(str[i] == '.') 
            {
                index = i; 
                break;
            }
        str.erase(str.begin() + index);
        for(int i = 0; i < str.size(); i++)
            result += (str[i] - 0x30) * pow(10, str.size() - 1 - i);
        result /= pow(10, str.size() - index);
    }
    return result;
}

