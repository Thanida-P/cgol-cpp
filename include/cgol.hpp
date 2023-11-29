#ifndef CGOL_HPP
#define CGOL_HPP

#include <iostream>
#include <vector>
#include <random>

#define LIVE true
#define DEAD false

class Grid {
public:
    friend class Simulation;

    Grid(size_t w, size_t h);
    Grid(size_t w, size_t h, const std::vector<std::vector<bool>>& other);
    bool get_cell(size_t x, size_t y) const { return grid[x][y]; };
    void set_cell(size_t x, size_t y, bool state) { grid[x][y] = state; }

    void random();

    void place(const Grid& other, size_t x, size_t y);
    void place_center(const Grid& other);

    int get_neighbors(size_t x, size_t y, bool wrap = true) const;
    Grid get_next_state() const;

    Grid get_minimal() const;

    size_t get_width() const { return width; }
    size_t get_height() const { return height; }
    void display() const;
private:
    size_t width;
    size_t height;
    std::vector<std::vector<bool>> grid;
};

class Simulation {
public:
    Simulation(): tick(0), delay(300) { states.push_back(Grid(20, 20)); }
    Simulation(int w, int h): tick(0), delay(300) { states.push_back(Grid(w, h)); }
    Simulation(Grid g): tick(0), delay(300) { states.push_back(g); }

    void random() { states[tick].random(); }

    size_t get_tick() const { return tick; }
    int get_delay() const { return delay; }
    size_t get_width() const { return states[tick].get_width(); }
    size_t get_height() const { return states[tick].get_height(); }
    bool get_cell(size_t x, size_t y) { return states[tick].get_cell(x, y); }
    
    void set_delay(int val) { delay = val; }
    void set_cell(size_t x, size_t y, bool state) {
        states[tick].set_cell(x, y, state);
        states.erase(states.begin()+tick+1, states.end());
    }

    void display() const { states[tick].display(); };

    Grid reset();
    Grid prev();
    Grid next();
    Grid cur();
private:
    size_t tick;
    int delay; // ms
    std::vector<Grid> states;
};

#endif /* CGOL_HPP */