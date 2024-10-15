//COMPILE USING: g++ -std=c++14 *.cpp -o mypython
// run using ./mypython <filename.py>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stack>
#include <cctype>
#include <stdexcept>
#include <exception>
#include <utility>

/* ----------- LEXER ---------- */
enum class TokenType {
    IDENTIFIER, 
    NUMBER, 
    ASSIGN, 
    PRINT, 
    STRING,
    IF,
    ELSE,
    DEF, // Function definition

    PLUS, 
    MULTIPLY, 
    MINUS, 
    DIVIDE,

    EQUAL_EQUAL, 
    BANG_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    LEFT_PAREN, RIGHT_PAREN, COMMA, COLON,

    INDENT, DEDENT,

    NEWLINE, END_OF_FILE, ERROR,

    RETURN, MODULUS
};

class Token {
    public:
        TokenType type;
        std::string lexeme;

        Token(TokenType type, const std::string& lexeme) : type(type), lexeme(lexeme) {}

        std::string tokenTypeToString() const {
            switch (type) {
                case TokenType::IDENTIFIER: return "IDENTIFIER";
                case TokenType::NUMBER: return "NUMBER";
                case TokenType::ASSIGN: return "ASSIGN";
                case TokenType::PRINT: return "PRINT";
                case TokenType::STRING: return "STRING";
                case TokenType::IF: return "IF";
                case TokenType::ELSE: return "ELSE";
                case TokenType::DEF: return "DEF";
                case TokenType::PLUS: return "PLUS";
                case TokenType::MULTIPLY: return "MULTIPLY";
                case TokenType::MINUS: return "MINUS";
                case TokenType::DIVIDE: return "DIVIDE";
                case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
                case TokenType::BANG_EQUAL: return "BANG_EQUAL";
                case TokenType::LESS: return "LESS";
                case TokenType::LESS_EQUAL: return "LESS_EQUAL";
                case TokenType::GREATER: return "GREATER";
                case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
                case TokenType::LEFT_PAREN: return "LEFT_PAREN";
                case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
                case TokenType::COMMA: return "COMMA";
                case TokenType::COLON: return "COLON";
                case TokenType::INDENT: return "INDENT";
                case TokenType::DEDENT: return "DEDENT";
                case TokenType::NEWLINE: return "NEWLINE";
                case TokenType::END_OF_FILE: return "END_OF_FILE";
                case TokenType::ERROR: return "ERROR";
                case TokenType::RETURN: return "RETURN";
                case TokenType::MODULUS: return "MODULUS";
                default: return "UNKNOWN";
            }
        }

        void print() const {
            std::cout << "Token Type: " << tokenTypeToString() << ", Lexeme: '" << lexeme << "'" << std::endl;
        }

};

class Lexer {
    public:
        Lexer(const std::string& source) : source(source) {
            tokenize();
        }

        const std::vector<Token>& getTokens() const {
            return tokens;
        }

    private:
        std::string source;
        std::vector<Token> tokens;
        size_t start = 0;
        size_t current = 0;
        size_t line = 1;
        std::stack<int> indentLevels;
        bool isBlock = false;

        void tokenize() {

            isBlock = false;
            indentLevels.push(0);
            // handleIndentation();
            
            while (!isAtEnd()) {
                scanToken();
            }
            addToken(TokenType::END_OF_FILE, "");
        }

        void scanToken() {

            char c = advance();
            std::cout << "Scanning character: " << c << std::endl;  // debugging


            switch (c) {
                case '#':
                    while (peek() != '\n' && !isAtEnd()) advance();
                    break; // Comments go until the end of the line
                case '+':
                    addToken(TokenType::PLUS, "+");
                    break;
                case '*':
                    addToken(TokenType::MULTIPLY, "*");
                    break;
                case '-':
                    addToken(TokenType::MINUS, "-");
                    break;
                case '/':
                    addToken(TokenType::DIVIDE, "/");
                    break;
                case '(':
                    addToken(TokenType::LEFT_PAREN, "(");
                    break;
                case ')':
                    addToken(TokenType::RIGHT_PAREN, ")");
                    break;
                case ',':
                    addToken(TokenType::COMMA, ",");
                    break;
                case ':':
                    addToken(TokenType::COLON, ":");
                    break;
                case '!':
                    addToken(peek() == '=' ? TokenType::BANG_EQUAL : TokenType::ERROR, peek() == '=' ? "N" : "!");
                    if (peek() == '=') advance();
                    break;
                case '=':
                    addToken(peek() == '=' ? TokenType::EQUAL_EQUAL : TokenType::ASSIGN, peek() == '=' ? "E" : "=");
                    if (peek() == '=') advance();
                    break;
                case '<':
                    addToken(peek() == '=' ? TokenType::LESS_EQUAL : TokenType::LESS, peek() == '=' ? "L" : "<");
                    if (peek() == '=') advance();
                    break;
                case '>':
                    addToken(peek() == '=' ? TokenType::GREATER_EQUAL : TokenType::GREATER, peek() == '=' ? "G" : ">");
                    if (peek() == '=') advance();
                    break;
                case '%':
                    addToken(TokenType::MODULUS, "%");
                    break;
                case '\n':
                    line++;
                    addToken(TokenType::NEWLINE, "");
                    start = current;
                    handleIndentation();
                    break;
                case 'r':
                    if (checkReturnKeyword()) {
                        std::cout << "Handling 'return' keyword\n"; // Debugging output
                        addToken(TokenType::RETURN, "return");
                        current += 5; // Advance past "eturn"
            }       else {
                        handleIdentifier();
            }
            break;
                case ' ': case '\r': case '\t':
                    // Ignore whitespace
                    break;
                case '"': case '\'':
                    handleString(c);
                    break;
                default:
                    if (isdigit(c)) {
                        handleNumber();
                    } else if (isalpha(c) || c == '_') {
                        handleIdentifier();
                    } else {
                        std::cout << "Unexpected character: " << c << std::endl;
                        addToken(TokenType::ERROR, std::string(1, c));
                    }

                    break;

            }

        }
        
        bool checkReturnKeyword() {
            if (current - 1 + 6 > source.size() || source.substr(current - 1, 6) != "return") return false;
            if (source.substr(current - 1, 6) == "return")
                return false;
            char nextChar = source[current + 5];
            return nextChar == ' ' || nextChar == ';' || nextChar == '\n' || nextChar == '(' || nextChar == ')' || nextChar == '{' || isWhitespace(nextChar);
}

        bool isalnum(char c) {
            return std::isalnum(c) != 0;
        }

        bool isAtEnd() const {
            return current >= source.size();
        }

        bool isWhitespace(char c) { 
            return c == ' ' || c == '\t'; 
        }

        char advance() {
            return source[current++];
        }

        char peek() const {
            if (current >= source.size()) return '\0';
            return source[current];
        }

        void addToken(TokenType type, const std::string& lexeme) {
            tokens.push_back(Token(type, lexeme));
        }

        void handleString(char quoteType) {
            start = current;
            while (!isAtEnd() && peek() != quoteType) advance();
            if (isAtEnd()) throw std::runtime_error("Unterminated string.");
            advance(); // Skip the closing quote
            addToken(TokenType::STRING, source.substr(start, current - start - 1));
        }

        void handleNumber() {
            start = current - 1;
            while (!isAtEnd() && isdigit(peek())) advance();
            addToken(TokenType::NUMBER, source.substr(start, current - start));
        }

        void handleIdentifier() {
            start = current - 1;
            
            while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
                advance();
            }
            
            std::string text = source.substr(start, current - start);
            std::cout << "Handling identifier: " << text << std::endl; // Debug output
            if (text == "print") {
                addToken(TokenType::PRINT, text);
            } else if (text == "if") {
                addToken(TokenType::IF, text);
            } else if (text == "else") {
                addToken(TokenType::ELSE, text);
            } else if (text == "def") {
                addToken(TokenType::DEF, text);
            } else if (text == "return") {
                addToken(TokenType::RETURN, text);
            } else {
                addToken(TokenType::IDENTIFIER, text);
            }

        }

        void handleIndentation() {
            int indent = 0;

            while (isWhitespace(peek())) {
                indent++;
                advance();
    
                //Debugging
                //std::cout << indent << std::endl;
            }

            if (peek() == '\n') {
                // Only whitespaces
                return;
            }

            /*
            if(indent && !isBlock) {
                throw std::runtime_error("Unexpected indentation at line " + std::to_string(line));
            } */

            int prevIndentLvl = indentLevels.top(); 

            if(indent > prevIndentLvl) {
                indentLevels.push(indent);
                addToken(TokenType::INDENT, "");
                isBlock = true;

            } else if(indent < prevIndentLvl){

                while(!indentLevels.empty() && indent < indentLevels.top()) {
                    indentLevels.pop();
                    addToken(TokenType::DEDENT, "");
                }

                if (indentLevels.empty() || indent != indentLevels.top()) {
                    throw std::runtime_error("Inconsistent indentation at line " + std::to_string(line));
                }


                if(indentLevels.size() == 1 && indentLevels.top() == 0) {
                    isBlock = false;
                }

            }
            
        
        }
           
};


/* ----------- AST ----------- */

enum class ASTNodeType {
    Int,
    String,
    Identifier,
    Assign,
    Print,
    BinaryOp,
    If,
    Block,
    Function,
    Return,
    FunctionCall,
    Def
};

/* --- Forward declarations --- */
class ASTNode;
class IntNode;
class StringNode;
class IdentifierNode;
class AssignNode;
class PrintNode;
class BinaryOpNode;
class IfNode;
class BlockNode;
class FunctionNode;
class ReturnNode;
class NodeVisitor;
class FunctionCallNode;



class NodeVisitor {
    public:
        virtual ~NodeVisitor() {}
        virtual void visit(IntNode* node) = 0;
        virtual void visit(StringNode* node) = 0;
        virtual void visit(IdentifierNode* node) = 0;
        virtual void visit(AssignNode* node) = 0;
        virtual void visit(PrintNode* node) = 0;
        virtual void visit(BinaryOpNode* node) = 0;
        virtual void visit(IfNode* node) = 0;
        virtual void visit(BlockNode* node) = 0;
        virtual void visit(FunctionNode* node) = 0;
        virtual void visit(ReturnNode* node) = 0;
        virtual void visit(FunctionCallNode* node) = 0;

};

class ASTNode { // Base class for ASTNode sub-types
    public:
        virtual ~ASTNode() {}
        virtual void accept(NodeVisitor* visitor) = 0;
        virtual ASTNodeType getType() const = 0;

        static std::string nodeTypeToString(ASTNodeType type) {
            switch (type) {
                case ASTNodeType::Assign: return "AssignNode";
                case ASTNodeType::Print: return "PrintNode";
                case ASTNodeType::Int: return "IntNode";
                case ASTNodeType::String: return "StringNode";
                case ASTNodeType::Identifier: return "IdentifierNode";
                case ASTNodeType::BinaryOp: return "BinaryOpNode";
                case ASTNodeType::If: return "IfNode";
                case ASTNodeType::Block: return "BlockNode";
                case ASTNodeType::Function: return "FunctionNode";
                case ASTNodeType::Return: return "ReturnNode";
                case ASTNodeType::FunctionCall: return "FunctionCallNode";
                default: return "UnknownNode";
            }
        }
};

class ReturnNode : public ASTNode {
    private:
        std::unique_ptr<ASTNode> value;
    public:
        ReturnNode(std::unique_ptr<ASTNode> val) : value(std::move(val)) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::Return;
        }

        ASTNode* getValue() const {
            return value.get();
        }

};



class IntNode : public ASTNode {
    private:
        int value;
    public:
        IntNode(int val) : value(val) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::Int;
        }

        int getValue() const {
            return value;
        }
};

class StringNode : public ASTNode {
    private:
        std::string value;
    public:
        StringNode(const std::string& val) : value(val) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::String;
        }

        const std::string& getValue() const {
            return value;
        }
};

class IdentifierNode : public ASTNode {
    private:
        std::string identifier;

    public:
        IdentifierNode(const std::string& id) : identifier(id) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::Identifier;
        }

        const std::string& getIdentifier() const {
            return identifier;
        }
};

class AssignNode : public ASTNode {
    private:
        std::string identifier;
        std::unique_ptr<ASTNode> value;

    public:
        AssignNode(std::string id, std::unique_ptr<ASTNode> val): identifier(std::move(id)), value(std::move(val)) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::Assign;
        }

        const std::string& getIdentifier() const {
            return identifier;
        }

        ASTNode* getValue() const { 
            return value.get(); 
        }
};

class PrintNode : public ASTNode {
    private:
        std::vector<std::unique_ptr<ASTNode>> expressions;

    public:
        PrintNode(std::vector<std::unique_ptr<ASTNode>> exprs): expressions(std::move(exprs)) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::Print;
        }

        const std::vector<std::unique_ptr<ASTNode>>& getExpressions() const { 
            return expressions; 
        }

};

class BinaryOpNode : public ASTNode {
    private:
        std::unique_ptr<ASTNode> left;
        std::unique_ptr<ASTNode> right;
        char op;

    public:
        BinaryOpNode(std::unique_ptr<ASTNode> left, char op, std::unique_ptr<ASTNode> right): left(std::move(left)), op(op), right(std::move(right)) {}

        void accept(NodeVisitor* visitor) override { 
            visitor->visit(this); 
        }
        
        ASTNodeType getType() const override { 
            return ASTNodeType::BinaryOp; 
        }
        
        const std::unique_ptr<ASTNode>& getLeft() const {
            return left;
        }
        
        const std::unique_ptr<ASTNode>& getRight() const {
            return right;
        }
        
        char getOp() const { 
            return op; 
        }

};

class IfNode : public ASTNode {
    private:
        std::unique_ptr<ASTNode> condition;
        std::unique_ptr<BlockNode> thenBranch;  // Changed from vector to BlockNode
        std::unique_ptr<BlockNode> elseBranch;  // Added elseBranch
    public:
        IfNode(std::unique_ptr<ASTNode> condition, std::unique_ptr<BlockNode> thenBranch, std::unique_ptr<BlockNode> elseBranch = nullptr)
            : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

        void accept(NodeVisitor* visitor) override { 
            visitor->visit(this); 
        }
        
        ASTNodeType getType() const override { 
            return ASTNodeType::If; 
        }

        const std::unique_ptr<ASTNode>& getCondition() const {
            return condition;
        }

        const std::unique_ptr<BlockNode>& getThenBranch() const {
            return thenBranch;
        }

        const std::unique_ptr<BlockNode>& getElseBranch() const {
            return elseBranch;
        }
        
};



class BlockNode : public ASTNode {
    private:
        std::vector<std::unique_ptr<ASTNode>> statements;
    public:
        BlockNode(std::vector<std::unique_ptr<ASTNode>> stmts) : statements(std::move(stmts)) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::Block;
        }

        const std::vector<std::unique_ptr<ASTNode>>& getStatements() const {
            return statements;
        }
};

class FunctionNode : public ASTNode {
    private:
        std::string name;
        std::vector<std::string> parameters;
        std::unique_ptr<BlockNode> body;

    public:
        FunctionNode(const std::string& name, const std::vector<std::string>& parameters, std::unique_ptr<BlockNode> body)
            : name(name), parameters(parameters), body(std::move(body)) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::Function;
        }

        const std::string& getName() const {
            return name;
        }

        const std::vector<std::string>& getParameters() const {
            return parameters;
        }

        BlockNode* getBody() const {
            return body.get();
        }
};

class FunctionCallNode : public ASTNode {
    private:
        std::string name;
        std::vector<std::unique_ptr<ASTNode>> arguments;

    public:
        FunctionCallNode(const std::string& name, std::vector<std::unique_ptr<ASTNode>> arguments)
            : name(name), arguments(std::move(arguments)) {}

        void accept(NodeVisitor* visitor) override {
            visitor->visit(this);
        }

        ASTNodeType getType() const override {
            return ASTNodeType::FunctionCall;
        }

        const std::string& getName() const {
            return name;
        }

        const std::vector<std::unique_ptr<ASTNode>>& getArguments() const {
            return arguments;
        }
};

/* ----------- PARSER ----------- */
class Parser {
        std::vector<Token> tokens;
        size_t current = 0;  // Current token being processed

    public:
        Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

        std::vector<std::unique_ptr<ASTNode>> parse() {
            std::vector<std::unique_ptr<ASTNode>> statements;

            while (!isAtEnd()) {
                if (match(TokenType::NEWLINE) || match(TokenType::DEDENT)) {  // Skip newlines and dedents
                    continue;
                }

                std::unique_ptr<ASTNode> stmt = parseStatement();
                if (stmt) {
                    statements.push_back(std::move(stmt));
                }
            }

            return statements;
        }

    private:
        std::unique_ptr<ASTNode> parseStatement() { // Handles statements with identifiers
            std::cout << "Entering parseStatement: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging

            if (match(TokenType::IF)) {
                std::cout << "Parsing IF statement" << std::endl; //debugging
                return parseIfStatement();

            } else if (match(TokenType::PRINT)) {
                std::cout << "Parsing PRINT statement" << std::endl; //debugging
                return parsePrintStatement();

            } else if (peek().type == TokenType::IDENTIFIER && peekNext().type == TokenType::ASSIGN) {
                std::cout << "Parsing ASSIGNMENT statement" << std::endl; //debugging
                return parseAssignStatement();

            } else if (match(TokenType::DEF)) {
                std::cout << "Parsing FUNCTION DEFINITION" << std::endl; //debugging
                return parseFunctionDefinition();

            } else if (peek().type == TokenType::RETURN) {
                std::cout << "Ready to parse RETURN statement, current token: " << peek().tokenTypeToString() << std::endl; //debugging
                std::cout << "Parsing RETURN statement" << std::endl; //debugging
                return parseReturnStatement();
            }
            
            
            throw std::runtime_error("Unexpected token in parseStatement(): " + peek().tokenTypeToString());

        }
  
        std::unique_ptr<IfNode> parseIfStatement() {
            std::cout << "Entering parseIfStatement: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            auto condition = parseExpression();  // Parse the condition
            consume(TokenType::COLON, "Expect ':' after if condition.");
            
            consume(TokenType::NEWLINE, "Expect newline after colon.");
            std::cout << "Consuming Token: " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            consume(TokenType::INDENT, "Expected indent at the start of block");
            std::cout << "Consuming Token: " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging

            auto thenBranch = parseBlock();
            
            std::unique_ptr<BlockNode> elseBranch = nullptr; // Initialize elsebranch
            consume(TokenType::DEDENT, "Expect dedent after if block.");

            if (match(TokenType::ELSE)) {
                consume(TokenType::COLON, "Expect ':' after else.");
                consume(TokenType::NEWLINE, "Expect newline after else colon.");
                consume(TokenType::INDENT, "Expect indent after else.");
                elseBranch = parseBlock();
                consume(TokenType::DEDENT, "Expect dedent after else block.");
            }


            return std::make_unique<IfNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
        }

        std::unique_ptr<BlockNode> parseBlock() {
            std::cout << "Entering parseBlock: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            std::vector<std::unique_ptr<ASTNode>> blockStatements;

            while (!check(TokenType::DEDENT) && !isAtEnd()) {
                if (match(TokenType::NEWLINE)) {  // Skip empty lines within the block
                    continue;
                }
                blockStatements.push_back(parseStatement());
            }

            return std::make_unique<BlockNode>(std::move(blockStatements));
        }

        std::unique_ptr<ASTNode> parseAssignStatement() {
            std::cout << "Entering parseAssignStatement: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            std::string identifier = consume(TokenType::IDENTIFIER, "Expect identifier.").lexeme;
            consume(TokenType::ASSIGN, "Expect '=' after identifier.");
            auto value = parseExpression();
            consume(TokenType::NEWLINE, "parseAssign: Expect newline after expression."); 
            return std::make_unique<AssignNode>(identifier, std::move(value));
        }

        std::unique_ptr<ASTNode> parsePrintStatement() {
            std::cout << "Entering parsePrintStatement: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            consume(TokenType::LEFT_PAREN, "Expect '(' after 'print'.");

            std::vector<std::unique_ptr<ASTNode>> expressions;

            if (!check(TokenType::RIGHT_PAREN)) {

                do {
                    expressions.push_back(parseExpression());
                } while (match(TokenType::COMMA));

            }
            
            consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

            if (peekNext().type == TokenType::NEWLINE) {
                consume(TokenType::NEWLINE, "Expect newline after print statement."); 
            }
            
            return std::make_unique<PrintNode>(std::move(expressions));

        }

        std::unique_ptr<ASTNode> parseReturnStatement() {
            std::cout << "Entering parseReturnStatement: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            std::cout << "Current token before expecting 'return': " << peek().tokenTypeToString() << std::endl;
            consume(TokenType::RETURN, "Expect 'return' keyword.");
            std::cout << "Token after consuming 'return': " << peek().tokenTypeToString() << std::endl;
            auto value = parseExpression();
            std::cout << "Parsed return expression, next Token should be: " << peek().tokenTypeToString() << std::endl; //debugging
            consume(TokenType::NEWLINE, "Expect newline after return statement.");
            std::cout << "parseReturnStatement: Successfully parsed return statement" << std::endl; //debugging
            return std::make_unique<ReturnNode>(std::move(value)); 
}


        std::unique_ptr<FunctionNode> parseFunctionDefinition() {
            std::cout << "Entering parseFunctionDefinition: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            if (peek().type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expect function name. Found: " + peek().tokenTypeToString());
}
            std::string functionName = consume(TokenType::IDENTIFIER, "Expect function name.").lexeme;
            std::cout << "Function name: " << functionName << std::endl; // More detailed debugging
            consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

            std::vector<std::string> parameters;

            while (!check(TokenType::RIGHT_PAREN)) {
                parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name.").lexeme);
                if (!match(TokenType::COMMA)) break;
}

            consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
            consume(TokenType::COLON, "Expect ':' after function parameters.");
            consume(TokenType::NEWLINE, "Expect newline after ':'");
            consume(TokenType::INDENT, "Expect indent before function body.");
            auto body = parseBlock();
            consume(TokenType::DEDENT, "Expect dedent after function body.");

            std::cout << "Finished parsing function: " << functionName << std::endl; //debugging
            return std::make_unique<FunctionNode>(functionName, parameters, std::move(body));

        }

        std::unique_ptr<ASTNode> parseFunctionCall() {
            std::string funcName = previous().lexeme;
            std::cout << "Parsing function call for function: " << funcName << std::endl; // Debugging
            consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
            std::vector<std::unique_ptr<ASTNode>> arguments;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
            std::cout << "Finished parsing function call for function: " << funcName << std::endl; // Debugging
            return std::make_unique<FunctionCallNode>(funcName, std::move(arguments));
        }

        std::unique_ptr<ASTNode> parseExpression() {
            std::cout << "Entering parseExpression: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            return parseEquality();
        }

        std::unique_ptr<ASTNode> parseEquality() {
            std::cout << "Entering parseEquality: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            std::unique_ptr<ASTNode> expr = parseComparison();

            while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL) ) {
                char op = previous().lexeme[0];  // Get operator from the token just consumed
                std::unique_ptr<ASTNode> right = parseComparison();
                expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
            }

            return expr;
        }

        std::unique_ptr<ASTNode> parseComparison() {
            std::cout << "Entering parseComparison: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            std::unique_ptr<ASTNode> expr = parseAddition();

            while (match(TokenType::GREATER) || match(TokenType::LESS) ||match(TokenType::GREATER_EQUAL) || match(TokenType::LESS_EQUAL)) {
                char op = previous().lexeme[0];
                auto right = parseAddition();
                expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
            }

            return expr;
        }

        std::unique_ptr<ASTNode> parseAddition() { // For ADD & SUB
            std::cout << "Entering parseAddition: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            auto expr = parseMultiplication(); // Calls parseMultiplication first b/c MUL & DIV are higher precedence
            std::cout << "Parsed expression, next Token should be: " << peekNext().tokenTypeToString() << std::endl; //debugging

            while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
                char op = previous().lexeme[0];
                auto right = parseMultiplication();

                expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
            }

            return expr;
        }

        std::unique_ptr<ASTNode> parseMultiplication() { // For MUL & DIV
            std::cout << "Entering parseMultiplication: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            auto expr = parsePrimary();
            std::cout << "Parsed expression, next Token should be: " << peekNext().tokenTypeToString() << std::endl; //debugging

            while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULUS)) {
                char op = previous().lexeme[0];
                auto right = parsePrimary();

                expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
            }

            return expr;
        }

        std::unique_ptr<ASTNode> parsePrimary() {
            std::cout << "Entering parsePrimary: Current Token = " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
            if (match(TokenType::NUMBER)) {
                return std::make_unique<IntNode>(std::stoi(previous().lexeme));

            } else if (match(TokenType::IDENTIFIER)) {
                if(check(TokenType::LEFT_PAREN)) {
                    return parseFunctionCall();
                }
                return std::make_unique<IdentifierNode>(previous().lexeme);

            } else if (match(TokenType::STRING)) {
                return std::make_unique<StringNode>(previous().lexeme);
            }
              else if (match(TokenType::RETURN)) {
                return parseReturnStatement();
              }

            throw std::runtime_error("Unexpected token in parsePrimary.");
        }

        bool match(TokenType type) {
            if (check(type)) {
                advance();
                return true;
            }
            return false;
        }

        Token consume(TokenType type, const std::string& message) {
            if (!check(type)) {
                std::string error_message = message + " - Found: " + peek().tokenTypeToString();
            
                throw std::runtime_error(error_message); // Throw an exception with the error message.
            }

            return advance();
        }

        bool check(TokenType type) {
            if (isAtEnd()) return false;
            return peek().type == type;
        }

        Token advance() {
            if (!isAtEnd()) current++;
            std::cout << "Advancing to: " << peek().tokenTypeToString() << ", Lexeme = '" << peek().lexeme << "'" << std::endl; //debugging
    
            return previous();
        }

        bool isAtEnd() {
            return peek().type == TokenType::END_OF_FILE;
        }

        Token peek() {
            return tokens[current];
        }

        Token peekNext() {
            if (current + 1 >= tokens.size()) return tokens.back();
            return tokens[current + 1];
        }

        Token previous() {
            return tokens[current - 1];
        }
};


/* ----------- SCOPE ----------- */
class Scope {
    private:
        std::unordered_map<std::string, int> variables;
        std::shared_ptr<Scope> parent;
        int returnValue;

    public:
        Scope(std::shared_ptr<Scope> parent = nullptr) : parent(parent) {}

        void setReturnValue(int value) {
            returnValue = value;
        }
        int getReturnValue() const {
            if (parent && returnValue == 0) {
                return parent->getReturnValue();
        }
            return returnValue;
    }

        void setVariable(const std::string& name, int value) {
            variables[name] = value;
            //std::cout << "Variable Set - " << name << ": " << value << std::endl; //debugging
        }

        int getVariable(const std::string& name) {
            if (variables.find(name) != variables.end()) {
             //   std::cout << "Variable Get - " << name << ": " << variables[name] << std::endl; //debugging
                return variables[name];
            } else if (parent != nullptr) {
                return parent->getVariable(name);
            } else {
                throw std::runtime_error("Variable not defined: " + name);
            }
        }

        std::shared_ptr<Scope> getParent() const {
            return parent;
        }

        bool isDefinedLocally(const std::string& name) {
            return variables.find(name) != variables.end();
        }
};

/* ----------- INTERPRETER ----------- */

class Interpreter : public NodeVisitor {
    private:
        std::shared_ptr<Scope> currentScope;
        std::unordered_map<std::string, FunctionNode*> functions;  // Holds function definitions


    public:
        Interpreter() : currentScope(std::make_shared<Scope>()) {}

        void interpret(ASTNode* root) {
            root->accept(this);  // Start interpretation from the root node
        }

        void visit(IntNode* node) override {}

        void visit(StringNode* node) override {}

        void visit(ReturnNode* node) override{
            int result = evaluate(node->getValue());
            currentScope->setReturnValue(result);
            //std::cout << "Returning: " << result << std::endl;
        }
        
        void visit(IdentifierNode* node) override {
            try {
                int value = currentScope->getVariable(node->getIdentifier());

                std::cout << node->getIdentifier() << " = " << std::endl;

            } catch (const std::runtime_error& e) {
                std::cerr << "Runtime Error: " << e.what() << std::endl;
            }
        }

        void visit(AssignNode* node) override {

            // Evaluate the right-hand side and assign to the identifier in the current scope
            int value = evaluate(node->getValue());
            currentScope->setVariable(node->getIdentifier(), value);

            // Debugging 
            //std::cout << "Assigned " << node->getIdentifier() << " = " << value << std::endl;
        }

        void visit(PrintNode* node) override {
            
            for (const auto& expr : node->getExpressions()) { // Determining the type of expr and handling it accordingly

                
                if (expr->getType() == ASTNodeType::Identifier) {  
                    IdentifierNode* idNode = dynamic_cast<IdentifierNode*>(expr.get());

                    if (idNode) {  // Always check the result of dynamic_cast
                        int value = currentScope->getVariable(idNode->getIdentifier());
                        std::cout << value << " ";
                    }

                } else if (expr->getType() == ASTNodeType::String) {
                    StringNode* strNode = dynamic_cast<StringNode*>(expr.get());

                    if (strNode) {  // Always check the result of dynamic_cast
                        std::cout << strNode->getValue() << " ";
                    }

                } 

            }

            std::cout << std::endl;
        }


        void visit(BinaryOpNode* node) override {

            // getLeft() and getRight() return std::unique_ptr<ASTNode>
            ASTNode* left = node->getLeft().get();   // Using .get() to extract the raw pointer
            ASTNode* right = node->getRight().get(); // Using .get() to extract the raw pointer

            // Now you can use `left` and `right` as raw pointers
            char op = node->getOp();
            int result = 0;

            switch (op) {
                case '+':
                    result = evaluate(left) + evaluate(right);
                    break;
                case '-':
                    result = evaluate(left) - evaluate(right);
                    break;
                case '*':
                    result = evaluate(left) * evaluate(right);
                    break;
                case '/':
                    if (evaluate(right) == 0) throw std::runtime_error("Division by zero.");
                    result = evaluate(left) / evaluate(right);
                    break;
                case 'E':  // '=='
                    result = evaluate(left) == evaluate(right);
                    break;
                case 'N':  // '!='
                    result = evaluate(left) != evaluate(right);
                    break;
                case 'L':  // '<='
                    result = evaluate(left) <= evaluate(right);
                    break;
                case 'G':  // '>='
                    result = evaluate(left) >= evaluate(right);
                    break;
                case '<':
                    result = evaluate(left) < evaluate(right);
                    break;
                case '>':
                    result = evaluate(left) > evaluate(right);
                    break;
                case '%':
                    result = evaluate(left) % evaluate(right);
                    break;
                case '^':
                    result = pow(evaluate(left), evaluate(right));
                    break;
                case '&':
                    result = evaluate(left) && evaluate(right);
                    break;
                case '|':
                    result = evaluate(left) || evaluate(right);
                    break;
                case '!':
                    result = !evaluate(right);
                    break;
                case '~':
                    result = ~evaluate(right);
                    break;
                default:
                    throw std::runtime_error("Unsupported operator: " + std::string(1, op));
            }

            std::cout << "Result of " << op << " operation: " << result << std::endl;
            
        }

        void visit(IfNode* node) override {
            // First, get a string representation of the condition for debugging purposes
            std::string conditionStr = conditionToString(node->getCondition().get());
            std::cout << "Evaluating IfNode condition: " << conditionStr << std::endl;

            int conditionResult = evaluate(node->getCondition().get()); // Are you a 0 or a 1?
            std::cout << "Condition result: " << conditionResult << std::endl;

            // If the condition is true, execute the thenBranch
            if (conditionResult == 1) {

                BlockNode* thenBlock = node->getThenBranch().get();  

                if (thenBlock) {
                    std::cout << "Executing thenBranch of IfNode." << std::endl;
                    for (const auto& stmt : thenBlock->getStatements()) {  // Access the statements inside BlockNode
                        stmt->accept(this);  // Visit each statement in the block
                    }

                }

            } else if (conditionResult == 0) {

                BlockNode* elseBlock = node->getElseBranch().get(); // Check if there is an elseBranch and execute it

                if (elseBlock) {
                    std::cout << "Executing elseBranch of IfNode." << std::endl;
                    for (const auto& stmt : elseBlock->getStatements()) {
                        stmt->accept(this);  // Visit each statement in the else block
                    }
                } else {
                    std::cout << "No elseBranch to execute." << std::endl;
                }

            }

        }

        void visit(BlockNode* node) override {
            // Check if the BlockNode is not null and has statements
            if (node) {
                // Iterate over each statement in the block and accept the visitor
                for (const auto& stmt : node->getStatements()) {
                    stmt->accept(this);
                }
            }
        }
        
        void visit(FunctionNode* node) override {
            std::cout << "Executing function: " << node->getName() << std::endl;
            functions[node->getName()] = node;
        }

        void visit(FunctionCallNode* node) override {
            std::cout << "Function call: " << node->getName() << std::endl;

    // Retrieve the function definition from the stored functions
            FunctionNode* funcDef = functions[node->getName()];
            if (funcDef == nullptr) {
                throw std::runtime_error("Function not defined: " + node->getName());
    }

    // Check if argument sizes match
            if (funcDef->getParameters().size() != node->getArguments().size()) {
                throw std::runtime_error("Argument size mismatch");
    }

    // Create a new scope for the function call
            auto newScope = std::make_shared<Scope>(currentScope);

    // Evaluate each argument and set it in the new scope
            for (size_t i = 0; i < node->getArguments().size(); ++i) {
                int argValue = evaluate(node->getArguments()[i].get());
                newScope->setVariable(funcDef->getParameters()[i], argValue);
    }

    // Save the current scope to restore later
            auto previousScope = currentScope;
    // Switch to the new scope for the duration of the function call
            currentScope = newScope;

    // Execute the function body
            funcDef->getBody()->accept(this);

    // Retrieve the return value from the new scope
            int returnValue = currentScope->getReturnValue();

    // Restore the previous scope after the function call
            currentScope = previousScope;

    // Set the return value in the current scope
            currentScope->setReturnValue(returnValue);

    // No need to return a value here since the return type is void
    // The return value is stored in the current scope's returnValue variable
}

            
            
            
        
        // Implement other visit methods...
  
    private:
        int evaluate(ASTNode* node) {
            switch (node->getType()) {
                case ASTNodeType::Int:
                    return dynamic_cast<IntNode*>(node)->getValue();

                case ASTNodeType::Identifier:
                    return currentScope->getVariable(dynamic_cast<IdentifierNode*>(node)->getIdentifier());

                case ASTNodeType::BinaryOp: {
        
                    BinaryOpNode* binNode = dynamic_cast<BinaryOpNode*>(node);
                    
                    int left = evaluate(binNode->getLeft().get()); // Use .get() to retrieve raw pointers from unique_ptr for recursive calls
                    int right = evaluate(binNode->getRight().get());

                    return evaluateBinaryOperation(binNode->getOp(), left, right);
                    
                }
                case ASTNodeType::Return: {
                    ReturnNode* returnNode = dynamic_cast<ReturnNode*>(node);
                    return evaluate(returnNode->getValue());
                }
                case ASTNodeType::FunctionCall: {
                    FunctionCallNode* funcCallNode = dynamic_cast<FunctionCallNode*>(node);
                    FunctionNode* funcDef = functions[funcCallNode->getName()];
                    if (!funcDef) {
                        throw std::runtime_error("Function not defined: " + funcCallNode->getName());
    }
    
    // Create a new scope for the function call
                    auto newScope = std::make_shared<Scope>(currentScope);
    // ... Push currentScope on the stack or switch to newScope

    // Evaluate each argument and set it in the new scope
                    const auto& params = funcDef->getParameters();
                    const auto& args = funcCallNode->getArguments();
                    for (size_t i = 0; i < args.size(); ++i) {
                        int argValue = evaluate(args[i].get());
                        newScope->setVariable(params[i], argValue);
                    }

                    // Switch to the new scope and execute the function body
                    currentScope = newScope;
                    funcDef->getBody()->accept(this);

                    // Handle the return value from the function
                    int returnValue = currentScope->getReturnValue(); // This would come from your mechanism for return values

                    // Restore the old scope
                    currentScope = currentScope->getParent();

                    return returnValue;
}

                case ASTNodeType::Print: {
                    PrintNode* printNode = dynamic_cast<PrintNode*>(node);
                    for (const auto& expr : printNode->getExpressions()) {
                        if (expr->getType() == ASTNodeType::Identifier) {
                            IdentifierNode* idNode = dynamic_cast<IdentifierNode*>(expr.get());
                            std::cout << currentScope->getVariable(idNode->getIdentifier()) << " ";
                        } else if (expr->getType() == ASTNodeType::String) {
                            StringNode* strNode = dynamic_cast<StringNode*>(expr.get());
                            std::cout << strNode->getValue() << " ";
                        }
                    }
                    std::cout << std::endl;
                    return 0;
                }
                case ASTNodeType::If: {
                    IfNode* ifNode = dynamic_cast<IfNode*>(node);
                    int condition = evaluate(ifNode->getCondition().get());
                    if (condition) {
                        ifNode->getThenBranch()->accept(this);
                    } else {
                        if (ifNode->getElseBranch()) {
                            ifNode->getElseBranch()->accept(this);
                        }
                    }
                    return 0;
                }
                case ASTNodeType::Block: {
                    BlockNode* blockNode = dynamic_cast<BlockNode*>(node);
                    for (const auto& stmt : blockNode->getStatements()) {
                        stmt->accept(this);
                    }
                    return 0;
                }
                case ASTNodeType::Assign: {
                    AssignNode* assignNode = dynamic_cast<AssignNode*>(node);
                    int value = evaluate(assignNode->getValue());
                    currentScope->setVariable(assignNode->getIdentifier(), value);
                    return 0;
                }
                case ASTNodeType::Function: {
                    FunctionNode* funcNode = dynamic_cast<FunctionNode*>(node);
                    std::cout << "Function definition: " << funcNode->getName() << std::endl;
                    return 0;
                }
                case ASTNodeType::String:
                    return 0;
                

                default:
                    throw std::runtime_error("Evaluation error: Unknown node type or unsupported node type in evaluate.");
            }

            // This return should never be reached because all cases should be handled above.
            throw std::runtime_error("Unexpected error in evaluate function.");
        }

        int evaluateBinaryOperation(char op, int left, int right) {
            switch (op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/': 
                    if (right == 0) throw std::runtime_error("Division by zero.");
                    return left / right;
                case 'E':  // '=='
                    return left == right;
                case 'N':  // '!='
                    return left != right;
                case 'L':  // '<='
                    return left <= right;
                case 'G':  // '>='
                    return left >= right;
                case '<':
                    return left < right;
                case '>':
                    return left > right;
                case '%':
                    return left % right;
                case '^':
                    return std::pow(left, right);
                case '&':
                    return left && right;
                case '|':
                    return left || right;
                case '~':
                    return ~left;
                case '!':
                    return !left;
                case 'A':
                    return left & right;
                case 'O':
                    return left | right;
                default:
                    throw std::runtime_error("Unsupported operator for binary operation.");
            }     
        }
        
        // Debugging
        std::string conditionToString(ASTNode* node) {
            if (!node) return "null";
            switch (node->getType()) {
                case ASTNodeType::BinaryOp: {
                    BinaryOpNode* binNode = dynamic_cast<BinaryOpNode*>(node);
                    char op = binNode->getOp();  // Assuming getOp() returns a char
                    return conditionToString(binNode->getLeft().get()) + 
                        " " + std::string(1, op) + " " + 
                        conditionToString(binNode->getRight().get());
                }
                case ASTNodeType::Int: {
                    IntNode* intNode = dynamic_cast<IntNode*>(node);
                    return std::to_string(intNode->getValue());
                }
                case ASTNodeType::Identifier: {
                    IdentifierNode* idNode = dynamic_cast<IdentifierNode*>(node);
                    return idNode->getIdentifier();
                }
                default:
                    return "Unsupported node type";
            }

        }

};



// Utility Function: Read file, turn into string
std::string fileToString(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}


/* ----------- MAIN ----------- */
int main(int argc, char* argv[]) {
    
    try {

        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <script file>" << std::endl;
            return 1;
        }

        // Read the script from the file specified by the first command line argument
        std::string script = fileToString(argv[1]);

        Lexer lexer(script);
        auto tokens = lexer.getTokens();

        // Debugging: Prints TokenType & lexeme upon generation
        for (const auto& token : tokens) { 
            token.print(); 
        }
        
        std::cout << std::endl;

        Parser parser(tokens);
        auto astNodes = parser.parse();

        // Debugging: Prints ASTNode type upon generation
        for (const auto& node : astNodes) {
            std::cout << "Generated AST Node Type: " << ASTNode::nodeTypeToString(node->getType()) << std::endl;
        }
        
        std::cout << std::endl;
       
        Interpreter interpreter;
        for (auto& root : astNodes) {
            interpreter.interpret(root.get());  // Interpret each AST node
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;

}