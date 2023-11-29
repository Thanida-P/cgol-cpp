#include "cgol.hpp"

#include <algorithm>

// returns positive remainder
size_t mod(int a, int b) {
    return (a % b + b) % b;
}

bool random_bool() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    return dis(gen);
}

Grid::Grid(size_t w, size_t h): width(w), height(h), grid(w, std::vector<bool>(h)) {
    // initialize cells
    for (size_t x = 0; x < w; ++x) {
        for (size_t y = 0; y < h; ++y) {
            grid[x][y] = DEAD;
        }
    }
}

Grid::Grid(size_t w, size_t h, const std::vector<std::vector<bool>>& other): width(w), height(h), grid(w, std::vector<bool>(h)) {
    for (size_t x = 0; x < w; ++x) {
        for (size_t y = 0; y < h; ++y) {
            if (x < other[y].size())
                grid[x][y] = other[y][x];
        }
    }
}

void Grid::random() {
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            grid[x][y] = random_bool();
        }
    }
}

void Grid::place(const Grid& other, size_t x, size_t y) {
    for (size_t j = 0; j < other.get_height(); ++j) {
        for (size_t i = 0; i < other.get_width(); ++i) {
            if (x + i < width && y + j < height) {
                set_cell(x+i, y+j, other.get_cell(i, j));
            }
        }
    }
}

void Grid::place_center(const Grid& other) {
    const int center_x = width/2 - other.get_width()/2;
    const int center_y = height/2 - other.get_height()/2;
    place(other, center_x, center_y);
}

int Grid::get_neighbors(size_t x, size_t y, bool wrap) const {
    int neighbors = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (!wrap) {
                // skip if at boundary
                if (x == 0 || x == width-1 || y == 0 || y == height-1 || (i == 0 && j == 0)) { 
                    continue; 
                }
                neighbors += grid[x+i][y+j];
            }
            else {
                // wrap around to other side
                if (i == 0 && j == 0) continue;
                neighbors += grid[mod(x+i, width)][mod(y+j, height)];
            }
        }
    }

    return neighbors;
}

Grid Grid::get_next_state() const {
    /*
    Any live cell with fewer than two live neighbours dies, as if by underpopulation.
    Any live cell with two or three live neighbours lives on to the next generation.
    Any live cell with more than three live neighbours dies, as if by overpopulation.
    Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
    */

    Grid result(width, height);

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            int neighbors = get_neighbors(x, y);
            if (neighbors < 2 || neighbors > 3) {
                result.set_cell(x, y, DEAD);
            }
            else if (neighbors == 3) {
                result.set_cell(x, y, LIVE);
            }
            else {
                result.set_cell(x, y, grid[x][y]);
            }
        }
    }

    return result;
}

Grid Grid::get_minimal() const {
    // get bounding rectangle by getting min and max coords
    size_t min_x = width;
    size_t min_y = height;
    size_t max_x = 0;
    size_t max_y = 0;
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            if (get_cell(x, y)) {
                if (x < min_x) { min_x = x; }
                if (x > max_x) { max_x = x; }
                if (y < min_y) { min_y = y; }
                if (y > max_y) { max_y = y; }
            }
        }
    }
    const size_t new_width = max_x - min_x + 1;
    const size_t new_height = max_y - min_y + 1;

    // copy area in bounding rectangle to new grid
    Grid minimal(new_width, new_height);
    for (size_t x = 0; x < new_width; ++x) {
        for (size_t y = 0; y < new_height; ++y) {
            minimal.set_cell(x, y, get_cell(min_x+x, min_y+y));
        }
    }
    return minimal;
}

void Grid::display() const {
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            if (grid[x][y] == LIVE) {
                std::cout << "X";
            } 
            else if (grid[x][y] == DEAD) {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

Grid Simulation::reset() {
    tick = 0;
    return states[tick];
}

Grid Simulation::prev() {
    tick--;
    return states[tick];
}

Grid Simulation::next() {
    if (tick == states.size() - 1) {
        states.push_back(cur().get_next_state());
    }
    else {
        states[tick+1] = cur().get_next_state();
    }
    tick++;

    return states[tick];
}

Grid Simulation::cur() {
    return states[tick];
}