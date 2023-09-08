#include <string>
#include <ncurses.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <array>

#include "ascii_arts.hpp"

typedef std::vector<std::vector<char>> Grid;
typedef std::vector<std::vector<int>> ColorGrid;

void init (Grid& grid, int height, int width) {
    grid.resize(height);
    for (auto& row : grid) {
        row.resize(width);
    }
}

class AsciiArt {
public:
    
    AsciiArt(std::string a, std::string colors_raw) : art(a), height(0), width(0), has_colors(true) {
        init_from_string();
        
        colors.resize(height);
        for (auto& row : colors) {
            row = std::vector<int>(width, 1);
        }

        int height_counter = 0;
        int width_counter = 0;

        for (int i = 0; i < art.size(); i++) {
            char c = colors_raw.at(i);
            if (c=='\n') {
                height_counter++;
                width_counter = 0;
            } else if (c >= '0' && c <= '9') {
                colors[height_counter][width_counter] = (int) c - '0';
                width_counter++;
            } else {
                colors[height_counter][width_counter] = 1;
                width_counter++;
            }
        }
    }

    AsciiArt(std::string a) : art(a), height(0), width(0), has_colors(false) {
        init_from_string();
    }

    void init_from_string() {
        int max_width = 0;
        int width_counter = 0;

        // push on the first row
        grid.push_back(std::vector<char>());
        height = 1;

        for (int i = 0; i < art.size(); i++) {
            char c = art.at(i);
            if (c=='\n') {
                height++;
                // add another row
                grid.push_back(std::vector<char>());
                // go back to first col
                width_counter = 0;
            } else {
                grid[height-1].push_back(c);
                width_counter++;
                max_width = std::max(width_counter, max_width);
            }
        }

        // fill in ragged rows with blanks
        width = max_width;
        for (int r = 0; r < height; r++) {
            for (int c = grid[r].size(); c < width; c++) {
                grid[r].push_back(' ');
            }
        }
    }

    AsciiArt(Grid g, int w, int h) : grid(g), width(w), height(h), has_colors(false) {}

    void print () {
        for (int r = 0; r < height; r++) {
            for (int c = 0; c < width; c++) {
                std::cout << grid[r][c];
            }
            std::cout << std::endl;
        }
    }

    void printColor () {
        if (!has_colors) return;

        for (int r = 0; r < height; r++) {
            for (int c = 0; c < width; c++) {
                std::cout << colors[r][c];
            }
            std::cout << std::endl;
        }
    }

    char getCharAt(int x, int y) {
        return grid[x][y];
    }

    int getColorAt(int x, int y) {
        if (!has_colors) return 1;
        return colors[x][y];
    }

    std::string art;
    Grid grid;
    ColorGrid colors;
    bool has_colors;
    int width;
    int height;
};

class AnimatedPlume {
public:
    static AsciiArt generate(size_t plume_size) {
        const std::string plume_chars  = "*~#$%&     /8b`'";

        int width = plume_size;
        int height = plume_size/2+1;

        Grid grid;
        init(grid, height, width);
        for (auto& row : grid) {
            for (auto& c  : row) {
                c = ' ';
            }
        }

        for (int r = 0; r < height-1; r++) {
            grid[r][r] = '(';
            for (int c = r+1; c < width-r-1; c++) {
                grid[r][c] = plume_chars.at(rand() % plume_chars.size());
    
            }
            grid[r][width-1-r] = ')';
        }
        grid[height-1][width/2] = '*';

        return AsciiArt(grid, width, height);
    }
};

void drawArt (AsciiArt art, int x, int y) {
    for (int r = 0; r < art.height; r++) {
        for (int c = 0; c < art.width; c++) {
            if (art.getCharAt(r, c) != ' ') {
                mvaddch(x+r,y+c,art.getCharAt(r, c) | COLOR_PAIR(art.getColorAt(r, c)) );
            }
        }
    }
}

int getRandomAttr(int freq) {
    int attr;
    switch (rand() % freq) {
        case 0:
            attr = A_NORMAL;
            break;
        case 1:
            attr = A_NORMAL;
            break;
        case 2:
            attr = A_NORMAL;
            break;
        case 3:
            attr = A_BOLD;
            break;
        case 4:
            attr = A_DIM;
            break;
        default: 
            attr = A_NORMAL;
    }

    return attr;
}

void drawArtSparkly (AsciiArt art, int x, int y) {
    for (int r = 0; r < art.height; r++) {
        for (int c = 0; c < art.width; c++) {
            int pixel = art.getCharAt(r, c);
            if (pixel != ' ') {
                pixel |= getRandomAttr(5);
                pixel |= COLOR_PAIR(art.getColorAt(r, c));
                mvaddch(x+r,y+c,pixel);
            }
        }
    }
}

int main () {

    AsciiArt tower (tower_raw);
    AsciiArt sls (sls_raw, sls_colors);

    // sls.print();
    // sls.printColor();

    std::vector<AsciiArt> arm;
    for (int i = 0; i < num_arm_stages; i++) {
        arm.emplace_back(arm_raw[i]);
    }

    std::vector<AsciiArt> countdown = {
        AsciiArt(liftoff),
        AsciiArt(one),
        AsciiArt(two),
        AsciiArt(three),
        AsciiArt(four),
        AsciiArt(five),
        AsciiArt(six),
        AsciiArt(seven),
        AsciiArt(eight),
        AsciiArt(nine),
        AsciiArt(ten)
    };

    // init
	initscr();		
    curs_set(0);

    int num_rows, num_cols;
    getmaxyx(stdscr,num_rows,num_cols);

    int x_frame = num_cols / 2 - 15;
    int y_frame = num_rows - 33;

    int final_arm_index = num_arm_stages-1;

    int sls_x_base = 0;
    int sls_y_base = 0;
    int arm_x_base = 13;
    int arm_y_base = 4;
    int tower_x_base = 25;
    int tower_y_base = 2;

    int number_x_base = -25;
    int number_y_base = 5;
    int liftoff_x_base = -45;

	start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK);

    // Change magenta to be that orangey sls color
    init_color(COLOR_MAGENTA, 756, 325, 137);
    init_color(COLOR_RED, 756, 0, 0);


    // just white
	init_pair(4, COLOR_WHITE, COLOR_BLACK);

    // sls orange
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

    // blue for the nasa meatball
	init_pair(6,  COLOR_BLUE, COLOR_BLACK);

    // Red for nasa logo
	init_pair(7,  COLOR_RED, COLOR_BLACK);


    int countdown_stage = 10;
    bool incr_countdown = false;

    // Contdown and Retract the arm
    for (int i = 0; i < num_arm_stages; i++) {
        erase();
        drawArt(sls, y_frame + sls_y_base, x_frame + sls_x_base);
        drawArt(arm[i], y_frame + arm_y_base, x_frame + arm_x_base + i);
        drawArt(tower, y_frame + tower_y_base, x_frame + tower_x_base);
        drawArt(countdown[countdown_stage], y_frame + number_y_base, x_frame + number_x_base);

        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (incr_countdown) {
            countdown_stage--;
        }
        incr_countdown = !incr_countdown;
    }

    // Countdown to 3
    for (; countdown_stage > 3; countdown_stage--) {
        erase();
        drawArt(sls, y_frame + sls_y_base, x_frame + sls_x_base);
        drawArt(arm[final_arm_index], y_frame + arm_y_base, x_frame + arm_x_base + final_arm_index);
        drawArt(tower, y_frame + tower_y_base, x_frame + tower_x_base);
        drawArt(countdown[countdown_stage], y_frame + number_y_base, x_frame + number_x_base);

        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Plumes start
    int plume_start_y = sls_y_base + sls.height;
    int r_booster_x = -1;
    int l_booster_x = 15;
    int r_engine_x = 6;
    int l_engine_x = 10; 

    int plume_referesh_rate = 10;
    for (int i = countdown_stage; i >=1 ; i--) {
        for (int j = 0; j < plume_referesh_rate; j++) {
            erase();
            drawArt(sls, y_frame + sls_y_base, x_frame + sls_x_base);
            drawArt(arm[final_arm_index], y_frame + arm_y_base, x_frame + arm_x_base + final_arm_index);
            drawArt(tower, y_frame + tower_y_base, x_frame + tower_x_base);
            drawArt(countdown[i], y_frame + number_y_base, x_frame + number_x_base);

            // draw plumes
            drawArtSparkly(AnimatedPlume::generate(5), y_frame + plume_start_y, x_frame + r_booster_x);
            drawArtSparkly(AnimatedPlume::generate(5), y_frame + plume_start_y, x_frame + l_booster_x);
            drawArtSparkly(AnimatedPlume::generate(3), y_frame + plume_start_y, x_frame + r_engine_x);
            drawArtSparkly(AnimatedPlume::generate(3), y_frame + plume_start_y, x_frame + l_engine_x);

            refresh();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000/plume_referesh_rate));
        }
    }

    // sparkle everything, just for fun
    int jiggle_frames = 10;
    for (int i = 0; i < jiggle_frames; i++) {
        erase();
        drawArt(sls, y_frame + sls_y_base, x_frame + sls_x_base);
        drawArt(arm[final_arm_index], y_frame + arm_y_base, x_frame + arm_x_base + final_arm_index);
        drawArt(tower, y_frame + tower_y_base, x_frame + tower_x_base);
        drawArtSparkly(countdown[0], y_frame + number_y_base, x_frame + liftoff_x_base);

        // draw plumes
        drawArtSparkly(AnimatedPlume::generate(5), y_frame + plume_start_y, x_frame + r_booster_x);
        drawArtSparkly(AnimatedPlume::generate(5), y_frame + plume_start_y, x_frame + l_booster_x);
        drawArtSparkly(AnimatedPlume::generate(3), y_frame + plume_start_y, x_frame + r_engine_x);
        drawArtSparkly(AnimatedPlume::generate(3), y_frame + plume_start_y, x_frame + l_engine_x);

        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/plume_referesh_rate));
    }


    // Launch!
    int launch_frames = num_rows + 300;
    int frame_rate = 150;
    for (int i = 0; i < launch_frames; i++) {
        erase();
        drawArt(sls, y_frame + sls_y_base - i, x_frame + sls_x_base);
        drawArt(arm[final_arm_index], y_frame + arm_y_base, x_frame + arm_x_base + final_arm_index);
        drawArt(tower, y_frame + tower_y_base, x_frame + tower_x_base);
        drawArtSparkly(countdown[0], y_frame + number_y_base, x_frame + liftoff_x_base);

        drawArtSparkly(AnimatedPlume::generate(5), y_frame + plume_start_y-i, x_frame + r_booster_x);
        drawArtSparkly(AnimatedPlume::generate(5), y_frame + plume_start_y-i, x_frame + l_booster_x);
        drawArtSparkly(AnimatedPlume::generate(3), y_frame + plume_start_y-i, x_frame + r_engine_x);
        drawArtSparkly(AnimatedPlume::generate(3), y_frame + plume_start_y-i, x_frame + l_engine_x);

        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(frame_rate));

        // speed up
        frame_rate = (double) frame_rate * 0.95;
    }

    // ending
    int finale_frames = 100;
    frame_rate = 20;
    for (int i = 0; i < finale_frames; i++) {
        erase();
        drawArtSparkly(countdown[0], y_frame + number_y_base, x_frame + liftoff_x_base);
        drawArt(arm[final_arm_index], y_frame + arm_y_base, x_frame + arm_x_base + final_arm_index);
        drawArt(tower, y_frame + tower_y_base, x_frame + tower_x_base);

        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(frame_rate));
    }

    // Exit
	endwin();

	return 0;
}

