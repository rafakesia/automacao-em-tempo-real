#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stack>
#include <unistd.h>


using namespace std;

// Matriz representando o labirinto
char** maze;
char maze_in = 'i';
char maze_out = 'o';
char discard_char;

// Numero de linhas e colunas do labirinto
int num_rows;
int num_cols;

// Posição a ser explorada
struct pos_t
{
	int i;
	int j;
};

// Estrutura de dados contendo as próximas
// posicões a serem exploradas no labirinto
stack<pos_t> fork_points;
pos_t dead_end = {-1,-1};

void print_maze()
{
	for (int i = 0; i < num_rows; ++i)
	{
		for (int j = 0; j < num_cols; ++j)
		{
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
}

void clear_console() {
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}

pos_t load_maze(const char* file_name)
{
	pos_t initial_pos;
	// Abre o arquivo para leitura (fopen)
	FILE* maze_file = fopen(file_name, "r");
	if (maze_file == NULL) {
        std::cerr << "Error opening file\n";
        return initial_pos;
    }
	// Le o numero de linhas e colunas (fscanf)
	// e salva em num_rows e num_cols
	fscanf(maze_file, "%d %d", &num_rows, &num_cols);
	fscanf(maze_file, "%c", &discard_char);
	
	// Aloca a matriz maze (malloc)
	maze = (char**)malloc(num_rows * sizeof(char*));
	for (int i = 0; i < num_rows; i++) {
		// Aloca cada linha da matriz
		maze[i] = (char *)malloc(num_cols * sizeof(char));
		//Le a linha e transcreve o valor para a matriz alocada
		for (int j = 0; j < num_cols; j++)
		{
			fscanf(maze_file, "%c", &maze[i][j]);
			if (maze[i][j] == maze_in) {
				initial_pos.i = i;
				initial_pos.j = j;
			}
		}
		fscanf(maze_file, "%c", &discard_char);
	}
	fclose(maze_file);
	printf("Labirinto:\n");
	print_maze();
	//sleep(1);
	clear_console();
	return initial_pos;
}

stack<pos_t> get_valid_positions(pos_t actual_pos, char * valid_chars) {
	pos_t valid_pos;
	stack<pos_t> valid_positions;
	char possible_pos;
	for (int i = actual_pos.i - 1; i <= actual_pos.i + 1; i+=2) {
		possible_pos = maze[i][actual_pos.j];
		if(strchr(valid_chars, possible_pos)) {
			valid_pos = {i, actual_pos.j};
			valid_positions.push(valid_pos);
		}
	}
	for (int j = actual_pos.j - 1; j <= actual_pos.j + 1; j+=2) {
		possible_pos = maze[actual_pos.i][j];
		if(strchr(valid_chars, possible_pos)) {
			valid_pos = {actual_pos.i, j};
			valid_positions.push(valid_pos);
		}
	}
	return valid_positions;
}

pos_t explore_path(pos_t actual_pos) {
	stack<pos_t> valid_positions = get_valid_positions(actual_pos, "xo");
	if(valid_positions.empty()) {
		return dead_end;
	}
	if(valid_positions.size() > 1) {
		fork_points.push(actual_pos);
	}
	return valid_positions.top();
}

bool equal_points(pos_t p1, pos_t p2) {
	return p1.i == p2.i && p1.j == p2.j;
}

pos_t walk_back(pos_t actual_pos) {
	stack<pos_t> valid_positions = get_valid_positions(actual_pos, ".x");
	if(valid_positions.size() > 1) {
		fork_points.push(actual_pos);
	}
	return valid_positions.top();
}

void walk(pos_t pos)
{
	pos_t next_pos;
	while(1) {
		//sleep(1);
		clear_console();	
		next_pos = explore_path(pos);
		if (equal_points(next_pos, dead_end)) {
			if (fork_points.size() < 1) {
				printf("Nao foi possivel encontrar saida\n");
			}
			else {
				next_pos = walk_back(pos);
			}
		}
		if(maze[next_pos.i][next_pos.j] == 'o') {
			break;
		}
		maze[pos.i][pos.j] = '.';
		maze[next_pos.i][next_pos.j] = '-';
		printf("Caminhando\n");
		print_maze();
		pos = {next_pos.i, next_pos.j};
	}
	maze[pos.i][pos.j] = '.';
	maze[next_pos.i][next_pos.j] = '-';
	printf("Finished:\n");
	print_maze();
	return;
}

void print_start(pos_t initial_pos){
	maze[initial_pos.i][initial_pos.j] = '-';
	printf("Inicio:\n");
	print_maze();
	//sleep(1);
}

int main(int argc, char* argv[])
{
	// carregar o labirinto com o nome do arquivo recebido como argumento (argv[])
	clear_console();
	pos_t initial_pos = load_maze("maze.txt");
	print_start(initial_pos);
	walk(initial_pos);
}
