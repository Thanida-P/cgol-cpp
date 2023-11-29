#include "cgol.hpp"
#include "parser_utils.hpp"

#include <sstream>
#include <cctype>
#include <algorithm>

// https://conwaylife.com/wiki/Run_Length_Encoded
Grid RLE_Parser::read() {
    std::string line;
    // pass comments before header
    while (getline(ios, line)) {
        if (line.front() != COMMENT_TAG)
            break;
    }

    std::stringstream header(line);

    std::string token;
    int width = 0;
    int height = 0;
    std::string rule;

    // parse header
    while (std::getline(header, token, ',')) {
        std::stringstream tokenStream(token);
        std::string key;
        std::string value;

        if (std::getline(tokenStream, key, '=') && std::getline(tokenStream, value)) {
            if (key.find("x") != std::string::npos) {
                width = std::stoi(value);
            } else if (key.find("y") != std::string::npos) {
                height = std::stoi(value);
            } else if (key.find("rule") != std::string::npos) {
                rule = value;
            }
        }
    }

    std::vector<std::vector<bool>> temp_grid;
    std::vector<bool> current_line;

    // RLE
    char ch;
    int run_length = 0;
    while (ios.get(ch)) {
        // if digit add to run length
        if (ch >= '0' && ch <= '9') {
            // run_length * 10 for multi digit run length
            run_length = run_length * 10 + (ch - '0');
        }
        else {
            // when run length is omitted
            if (run_length == 0) {
                run_length = 1;
            }
            switch (ch) {
            case LIVE_TAG:
                for (int i = 0; i < run_length; ++i) {
                    current_line.push_back(LIVE);
                }
                break;
            case DEAD_TAG:
                for (int i = 0; i < run_length; ++i) {
                    current_line.push_back(DEAD);
                }
                break;
            case EOL_TAG:
                temp_grid.push_back(current_line);
                current_line.clear();
                break;
            case END_TAG:
                if (!current_line.empty()) {
                    temp_grid.push_back(current_line);
                }
                return Grid(width, height, temp_grid);
            default:
                throw std::runtime_error("Invalid token.");
            }
            run_length = 0;
        }
    }
    std::cout << "Warning: Ending '!' not found." << std::endl;
    if (!current_line.empty()) {
        temp_grid.push_back(current_line);
    }
    return Grid(width, height, temp_grid);
}

void RLE_Parser::save(const Grid& g) {
    
    // header
    ios << "x = " << g.get_width() << ", y = " << g.get_height() << std::endl;

    // RLE
    for (size_t y = 0; y < g.get_height(); ++y) {
        int run_length = 1;
        bool prev = g.get_cell(0,y);
        for (size_t x = 1; x < g.get_width(); ++x) {
            if (g.get_cell(x, y) != prev) {
                if (run_length != 1) {
                    ios << std::to_string(run_length);
                }
                if (prev == LIVE) {
                    ios << LIVE_TAG;
                }
                else {
                    ios << DEAD_TAG;
                }
                run_length = 1;
                prev = g.get_cell(x, y);
            }
            else {
                run_length++;
            }
        }
        if (run_length != 1) {
            ios << std::to_string(run_length);
        }
        if (prev == LIVE) {
            ios << LIVE_TAG;
        }
        else {
            ios << DEAD_TAG;
        }
        ios << EOL_TAG;
    }
    ios << END_TAG;
}

// https://conwaylife.com/wiki/Plaintext
Grid Plaintext_Parser::read() {
    std::string line;
    std::vector<std::string> lines;
    int w = 0;
    int h = 0;

    // creating the grid
    while (getline(ios, line)) {
        if (line.front() != COMMENT_TAG) {
            w = line.size();
            h++;
            lines.push_back(line);
        }
    }
    Grid result(w,h);

    //set the cells
    int y = 0;
    for (const auto& line : lines) {
        for (int x = 0; x < w; x++) {
            if (line[x] == LIVE_SYMBOL) {
                result.set_cell(x, y, LIVE);
            }
            else if (line[x] == DEAD_SYMBOL) {
                result.set_cell(x, y, DEAD);
            }
        }
        y++;
    }

    return result;
}

void Plaintext_Parser::save(const Grid& g) {
    for (size_t i = 0; i < g.get_height(); i++){
        for (size_t j = 0; j < g.get_width(); j++){
            if (g.get_cell(j,i) == LIVE){
                ios << LIVE_SYMBOL;
            }
            else if (g.get_cell(j,i) == DEAD){
                ios << DEAD_SYMBOL;
            }
        }
        ios << std::endl;
    }
}

// https://conwaylife.com/wiki/Life_1.06
Grid Life106_Parser::read() {
    std::string line;
    std::vector<int> x;
    std::vector<int> y;
    
    // skip header
    getline(ios, line);

    // creating the grid
    while (getline(ios, line)) {
        std::stringstream ss(line);
        int x_coord;
        int y_coord;
        ss >> x_coord >> y_coord;
        x.push_back(x_coord);
        y.push_back(y_coord);
    }
    int minX = *std::min_element(x.begin(), x.end());
    int minY = *std::min_element(y.begin(), y.end());
    int maxX = *std::max_element(x.begin(), x.end());
    int maxY = *std::max_element(y.begin(), y.end());
    int h = maxY - minY + 1;
    int w = maxX - minX + 1;
    Grid result(w,h);

    //set the cells
    for (size_t count = 0; count < x.size(); count++) {
        result.set_cell(x[count] - minX, y[count] - minY, LIVE);
    }
    return result;
}

void Life106_Parser::save(const Grid& g) {
    ios << "#Life 1.06" << std::endl;
    for (size_t i = 0; i < g.get_height(); i++){
        for (size_t j = 0; j < g.get_width(); j++){
            if (g.get_cell(j,i) == LIVE){
                ios << j << " " << i << std::endl;
            }
        }
    }
}

std::string FileHandler::get_extension(std::string filename) {
    std::string ext = filename.substr(filename.find_last_of(".") + 1);
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return ext;
}

Grid FileHandler::read(std::string filename) {

    std::fstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("File failed to open.");
    }
    
    std::string ext = get_extension(filename);
    std::unique_ptr<Parser> parser;

    if (ext == "rle") {
        parser = std::make_unique<RLE_Parser>(file);
    }
    else if (ext == "txt") {
        parser = std::make_unique<Plaintext_Parser>(file);
    }
    else if (ext == "life" || ext == "lif") {
        parser = std::make_unique<Life106_Parser>(file);
    }
    else {
        throw std::runtime_error("Unsupported file format.");
    }
    
    Grid result = parser->read();

    file.close();

    return result;
}

void FileHandler::save(const Grid& g, std::string filename) {
    
    std::fstream file(filename, std::ios::out);

    if (!file.is_open()) {
        throw std::runtime_error("File failed to open.");
    }
    
    std::string ext = get_extension(filename);
    std::unique_ptr<Parser> parser;

    if (ext == "rle") {
        parser = std::make_unique<RLE_Parser>(file);
    }
    else if (ext == "txt") {
        parser = std::make_unique<Plaintext_Parser>(file);
    }
    else if (ext == "life" || ext == "lif") {
        parser = std::make_unique<Life106_Parser>(file);
    }
    else {
        throw std::runtime_error("Unsupported file format.");
    }
    
    parser->save(g);

    file.close();
}