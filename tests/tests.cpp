#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "../include/cgol.hpp"
#include "../include/parser_utils.hpp"

bool compare_grid(const Grid& g1, const Grid& g2) {
    if ((g1.get_width() != g2.get_width()) || (g1.get_height() != g2.get_height())){
        return false;
    }
    for (int i = 0; i < g1.get_width(); i++) {
        for (int j = 0; j < g1.get_height(); j++){
            if (g1.get_cell(i, j) != g2.get_cell(i, j)){
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("Initialize grid correctly") {

    SUBCASE("initialize grid with width and height") {
        Grid grid(10, 10);
        CHECK(grid.get_width() == 10);
        CHECK(grid.get_height() == 10);
    }

    SUBCASE("initialize grid with width and height and a vector of cells") {
        std::vector<std::vector<bool>> cells = {{LIVE, DEAD, DEAD}, {LIVE, LIVE, DEAD}, {DEAD, DEAD, LIVE}};
        Grid grid(3, 3, cells);
        CHECK(grid.get_width() == 3);
        CHECK(grid.get_height() == 3);
        CHECK(grid.get_cell(0, 0) == LIVE);
        CHECK(grid.get_cell(0, 1) == LIVE);
        CHECK(grid.get_cell(1, 1) == LIVE);
    }
}

TEST_CASE("Test Grid member functions") {
    std::vector<std::vector<bool>> cells = {{LIVE, DEAD, DEAD}, {LIVE, LIVE, DEAD}, {DEAD, DEAD, LIVE}};
    Grid grid(10, 10);
    Grid grid2(3, 3, cells);

    SUBCASE("test set_cell() and get_cell() functions") {
        CHECK(grid.get_cell(0, 0) == DEAD);
        grid.set_cell(0, 0, LIVE);
        CHECK(grid.get_cell(0, 0) == LIVE);
    }

    SUBCASE ("test place() and place_center() functions") {
        grid.place(grid2, 1, 1);
        CHECK(grid.get_cell(1, 1) == LIVE);
        CHECK(grid.get_cell(2, 2) == LIVE);
        CHECK(grid.get_cell(3, 3) == LIVE);

        grid.place_center(grid2);
        CHECK(grid.get_cell(4, 4) == LIVE);
        CHECK(grid.get_cell(5, 5) == LIVE);
        CHECK(grid.get_cell(6, 6) == LIVE);
    }

    SUBCASE("test get_neighbors() function") {
        Grid grid3(5,5);
        grid3.place_center(grid2);

        CHECK(grid3.get_neighbors(1, 1) == 2);
        CHECK(grid3.get_neighbors(2, 2) == 3);
        CHECK(grid3.get_neighbors(3, 3) == 1);
    }

    SUBCASE("test get_next_state() function") {
        Grid grid3(5,5);
        grid3.place_center(grid2);
        Grid grid4 = grid3.get_next_state();
        
        CHECK(grid4.get_cell(1, 1) == LIVE);
        CHECK(grid4.get_cell(2, 2) == LIVE);
        CHECK(grid4.get_cell(3, 3) == DEAD);
    }

    SUBCASE("test get_minimal() function") {
        Grid grid3(5,5);
        grid3.place_center(grid2);
        
        CHECK(compare_grid(grid3.get_minimal(), grid2));
    }
}

TEST_CASE("Initialize simulation correctly") {
    
    SUBCASE("Default initialization") {
        Simulation sim;
        CHECK(sim.get_width() == 20);
        CHECK(sim.get_height() == 20);
        CHECK(sim.get_delay() == 300);
    }

    SUBCASE("Initialization with width and height") {
        Simulation sim(5, 5);
        CHECK(sim.get_width() == 5);
        CHECK(sim.get_height() == 5);
        CHECK(sim.get_delay() == 300);
    }

    SUBCASE("Initialization with a grid") {
        Simulation sim(Grid(6, 6));
        CHECK(sim.get_width() == 6);
        CHECK(sim.get_height() == 6);
        CHECK(sim.get_delay() == 300);
    }
}

TEST_CASE("Test Simulation member functions") {
    Simulation sim1(5, 5);

    SUBCASE("test get_tick() function") {
        CHECK(sim1.get_tick() == 0);
    }

    SUBCASE("test get_cell() and set_cell() functions") {
        CHECK(sim1.get_cell(0, 0) == DEAD);
        sim1.set_cell(0, 0, LIVE);
        CHECK(sim1.get_cell(0, 0) == LIVE);
    }

    SUBCASE("test get_delay() and set_delay() functions") {
        CHECK(sim1.get_delay() == 300);
        sim1.set_delay(1000);
        CHECK(sim1.get_delay() == 1000);
    }

    SUBCASE("test reset(), prev(), next(), and cur() function") {
        sim1.set_cell(0, 0, LIVE);
        sim1.set_cell(1, 1, LIVE);
        sim1.set_cell(1, 2, LIVE);
        sim1.set_cell(1, 3, LIVE);

        Grid currentGrid = sim1.cur();
        CHECK(currentGrid.get_cell(0, 0) == LIVE);
        CHECK(currentGrid.get_cell(1, 1) == LIVE);
        CHECK(currentGrid.get_cell(1, 2) == LIVE);
        CHECK(currentGrid.get_cell(1, 3) == LIVE);

        Grid nextGrid = sim1.next();
        CHECK(nextGrid.get_cell(0, 0) == DEAD);
        CHECK(nextGrid.get_cell(0, 1) == LIVE);
        CHECK(nextGrid.get_cell(2, 2) == LIVE);

        Grid prevGrid = sim1.prev();
        CHECK(currentGrid.get_cell(0, 0) == LIVE);
        CHECK(currentGrid.get_cell(1, 1) == LIVE);
        CHECK(currentGrid.get_cell(1, 2) == LIVE);
        CHECK(currentGrid.get_cell(1, 3) == LIVE);

        Grid resetGrid = sim1.reset();
        CHECK(resetGrid.get_cell(0, 0) == LIVE);
        CHECK(resetGrid.get_cell(1, 1) == LIVE);
        CHECK(resetGrid.get_cell(1, 2) == LIVE);
        CHECK(resetGrid.get_cell(1, 3) == LIVE);
    }
}

TEST_CASE("test file handling and parsing") {
    FileHandler f1;

    std::vector <std::vector<bool>> cells = {{DEAD, LIVE, DEAD}, {DEAD, DEAD, LIVE}, {LIVE, LIVE, LIVE}};
    Grid test_grid(3, 3, cells);

    SUBCASE("test read and save for rle files") {
        auto g1 = f1.read("data/ex1.rle");
        CHECK(g1.get_width() == 3);
        CHECK(g1.get_height() == 3);
        CHECK(compare_grid(test_grid, g1) == true);

        Grid g2(5,5);
        g2.place_center(g1);
        Simulation s(g2);
        s.next();

        f1.save(s.cur(), "test.rle");
        auto g3 = f1.read("test.rle");
        CHECK(compare_grid(s.cur(), g3) == true);
    }

    SUBCASE("test read and save for txt files") {
        auto g1 = f1.read("data/ex2.txt");
        CHECK(g1.get_width() == 3);
        CHECK(g1.get_height() == 3);
        CHECK(compare_grid(test_grid, g1) == true);

        Grid g2(5,5);
        g2.place_center(g1);
        Simulation s(g2);
        s.next();

        f1.save(s.cur(), "test.txt");
        auto g3 = f1.read("test.txt");
        CHECK(compare_grid(s.cur(), g3) == true);
    }

    SUBCASE("test read and save for life files") {
        auto g1 = f1.read("data/ex3.life");
        CHECK(g1.get_width() == 3);
        CHECK(g1.get_height() == 3);
        CHECK(compare_grid(test_grid, g1) == true);

        Grid g2(5,5);
        g2.place_center(g1);
        Simulation s(g2);
        s.next();

        f1.save(s.cur(), "test.life");
        auto g3 = f1.read("test.life");
        CHECK(compare_grid(s.cur().get_minimal(), g3) == true);
    }
}
