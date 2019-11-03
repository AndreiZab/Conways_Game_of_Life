#include <iostream>
#include <string>
#include <vector>
#include <random>

#include <fstream>
#include <cstdlib>
#include <time.h> //clock()
#include <chrono> //sleep 2.0
#include <thread> // sleep 2.0
//#include <zconf.h>

using namespace std; // I can do it here?

/*
Дана доска размером M × N клеток.
Клетка может находиться в одном из двух состояний: 1 — живая, 0 — мёртвая. Каждая клетка взаимодействует с восемью соседями.

Правила таковы:
	Живая клетка, у которой меньше двух живых соседей, погибает.
	Живая клетка, у которой два или три живых соседа, выживает.
	Живая клетка, у которой больше трёх живых соседей, погибает.
	Мёртвая клетка, у которой три живых соседа, возрождается.

Напишите программу, которая будет:
— случайным образом генерить стартовое состояние;
— уметь получать его из файла (способ выбирается через параметры запуска в консоли);
— каждую секунду выводить в консоль новое состояние доски.
 */


void    print_usage()
{
    cout << "Usage: ./program_name (<-g> | <-f> file)\n"
            "\t-g - generate a cell board\n"
            "\t-f - open cell board from file\n";
    exit (0);
}

void    print_table (vector<string> &table)
{
    for (auto &row : table) {
        cout << row;
        cout << '\n';
    }
    cout << '\n';
}

void	generate_table(vector<string> &table)
{
    mt19937 mt(time(nullptr));
    uniform_int_distribution<int> map_dist(5, 20);
    int width = map_dist(mt);
    int height = map_dist(mt);
    uniform_int_distribution<char> dist('0', '1');
    for (int i = 0; i < height; ++i) {
        string line;
        for (int j = 0; j < width; ++j) {
            line.push_back(dist(mt));
        }
        table.push_back(line);
    }
    print_table(table);
}

string remove_spaces(string &line)
{
    int length = line.length();
    for (int i = 0; i < length; i++) {
        if(line[i] == ' ') {
            line.erase(i, 1);
            length--;
            i--;
        }
    }
    return (line);
}

void	read_table(const char *name, vector<string> &table)
{
	ifstream file(name);
	if (!file)
        throw "File could not be opened";
	//	write_error("File could not be opened");
	while (file) {
        string line;
        getline(file, line);
        remove_spaces(line);
        for (char c : line) {
            if (c != '0' && c != '1')
                throw "Invalid character in board(only 1 and 0 allowed)";
        }
        if (!line.empty())
            table.push_back(line);
    }
}

void    input_validation(vector<string> &table, int argc, char *argv[])
{
    if (argc == 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
        print_usage();
    else if (argc == 2 && !strcmp(argv[1], "-g"))
        generate_table(table);
    else if (argc == 3 && !strcmp(argv[1], "-f"))
        read_table(argv[2], table);
    else if (argc > 3)
        throw "Too many arguments";
        //write_error("Too many arguments");
    else
        print_usage();
}

int     cell_neighbors(const vector<string> &table, const int row, const int col)
{
    int neig (0);
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if (i == 0 && j == 0)
                continue ;
            if (row + i < 0 || col + j < 0)
                continue ;
            if (row + i >= table.size() || col + j >= table[row + i].size())
                continue ;
            neig += table[row + i][col + j] - '0';
        }
    }
    return (neig);
}

bool interaction (vector<string> &table)
{
    bool all_zero (false);
    bool no_changes (true);
    const vector<string> last_state (table);

    for (int row = 0; row < table.size(); ++row) {
        for (int col = 0; col < table[row].size(); ++col) {
            int neighbors = cell_neighbors(last_state, row, col);
           if (last_state[row][col] == '1' && (neighbors > 3 || neighbors < 2)) {
               table[row][col] -= 1;
               no_changes = false;
           }
           else if (last_state[row][col] == '0' && neighbors == 3) {
               table[row][col] += 1;
               no_changes = false;
           }
           if (!all_zero && table[row][col] == '1') {
               all_zero = true;
           }
        }
    }
    if (!all_zero)
        cout << "Stop: Further in the output only zeros" << '\n';
    if (no_changes)
        cout << "Stop: No further changes" << '\n';
    return (!no_changes & all_zero);
}

int		main(int argc, char *argv[])
{
 //   vector<vector<bool>> table;
    vector<string> table;
    try {
        input_validation(table, argc, argv);
        bool loop(true);
        while (loop) {
            auto time_0 = chrono::steady_clock::now();
            loop = interaction(table);
            auto wait = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - time_0);
            this_thread::sleep_for(chrono::seconds{1} - wait);
            if (loop)
                print_table(table);
        }
    }
    catch (const char* exception) {
        cerr << "Error: " << exception << '\n';
        exit(1);
    }
    catch (bad_alloc& exception) {
        cerr << "bad_alloc caught: " << exception.what() << '\n';
        exit(1);
    }
    catch (...) {
        cerr << "We caught an exception of an undetermined type!\n";
        exit(1);
    }
//	free_memory(table); dont need, lol
	return(0);

}