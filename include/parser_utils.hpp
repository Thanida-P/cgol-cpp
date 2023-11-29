#ifndef PARSER_HPP
#define PARSER_HPP

#include "cgol.hpp"

#include <fstream>
#include <sstream>
#include <memory>

class Parser {
public:
    Parser(std::iostream& stream): ios(stream) {}
    virtual ~Parser() {};

    virtual Grid read() = 0;
    virtual void save(const Grid& g) = 0;

protected:
    std::iostream& ios;
};

// https://conwaylife.com/wiki/Run_Length_Encoded
class RLE_Parser: public Parser {
public:
    RLE_Parser(std::iostream& stream): Parser(stream) {}
    Grid read() override;
    void save(const Grid& g) override;

    static const char COMMENT_TAG = '#';
    static const char DEAD_TAG = 'b';
    static const char LIVE_TAG = 'o';
    static const char EOL_TAG = '$';
    static const char END_TAG = '!';
};

// https://conwaylife.com/wiki/Plaintext
class Plaintext_Parser: public Parser {
public:
    Plaintext_Parser(std::iostream& stream): Parser(stream) {}
    Grid read() override;
    void save(const Grid& g) override;

    static const char DEAD_SYMBOL = '.';
    static const char LIVE_SYMBOL = 'O';
    static const char COMMENT_TAG = '!';
};

// https://conwaylife.com/wiki/Life_1.06
class Life106_Parser: public Parser {
public:
    Life106_Parser(std::iostream& stream): Parser(stream) {}
    Grid read() override;
    void save(const Grid& g) override;
};

class FileHandler {
public:
    FileHandler() {};

    Grid read(std::string filename);
    void save(const Grid& g, std::string filename);

    std::string get_extension(std::string filename);
    // std::unique_ptr<Parser> get_parser(std::string filename);
};

#endif /* PARSER_HPP */