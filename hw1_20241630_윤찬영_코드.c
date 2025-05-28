#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct element {
    short int row;
    short int col;
    short int dir;
    struct element* next;
} element;

typedef struct {
    short int vert;
    short int horiz;
} offsets;

offsets move[8] = {
    {-1, 0},  // N
    {-1, 1},  // NE
    {0, 1},   // E
    {1, 1},   // SE
    {1, 0},   // S
    {1, -1},  // SW
    {0, -1},  // W
    {-1, -1}  // NW
};

void push(element* top, element new_em) {
    element* new_node = (element*)malloc(sizeof(element));
    *new_node = new_em;
    new_node->next = NULL;

    while (top->next != NULL)
        top = top->next;

    top->next = new_node;
}

element pop(element* top) {
    if (top->next == NULL) {
        printf("스택이 비어 있습니다 (pop 실패)\n");
        exit(1); // 안전하게 종료
    }

    element* prev = top;
    element* curr = top->next;

    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }

    element ret = *curr;
    prev->next = NULL;
    free(curr);
    return ret;
}

void path(int** maze, int** mark, int EXIT_ROW, int EXIT_COL) {
    int row, col, nextRow, nextCol, dir;
    bool found = false;

    element* stack = (element*)malloc(sizeof(element));
    stack->row = -1; stack->col = -1; stack->dir = -1;
    stack->next = NULL;
    element start = { 1, 1, 0, NULL };
    mark[1][1] = 1;
    push(stack, start);

    element position;

    while (stack->next != NULL && !found) {
        position = pop(stack);

        row = position.row; col = position.col; dir = position.dir;

        while (dir < 8 && !found) {
            nextRow = row + move[dir].vert;
            nextCol = col + move[dir].horiz;

            if (nextRow == EXIT_ROW && nextCol == EXIT_COL) {
                found = true;
            }
            else if (!maze[nextRow][nextCol] && !mark[nextRow][nextCol]) {
                mark[nextRow][nextCol] = 1;
                position.row = row; position.col = col;
                position.dir = ++dir;
                push(stack, position);

                row = nextRow; col = nextCol; dir = 0;
            }
            else {
                dir++;
            }
        }
    }

    if (found) {
        printf("The path is : \n");
        printf("row col \n");

        for (element* p = stack->next; p != NULL; p = p->next)
            printf("%2d %5d\n", p->row, p->col);

        printf("%2d %5d\n", row, col);
        printf("%2d %5d\n", EXIT_ROW, EXIT_COL);
    }
    else {
        printf("-1\n");
    }

    while (stack != NULL) {
        element* temp = stack;
        stack = stack->next;
        free(temp);
    }
}

int main() {
    printf("현재 디렉토리: %s\n", _getcwd(NULL, 0));

    FILE* file;
    fopen_s(&file, "maze.txt", "r");
    if (file == NULL) {
        printf("파일열기 실패\n");
        return 1;
    }
    else {
        printf("파일열기 성공\n");
    }

    int maze_row, maze_col;
    fscanf_s(file, "%d %d", &maze_row, &maze_col);

    int** maze = (int**)malloc(sizeof(int*) * (maze_row + 2));
    int** mark = (int**)malloc(sizeof(int*) * (maze_row + 2));
    for (int i = 0; i <= maze_row + 1; i++) {
        maze[i] = (int*)malloc(sizeof(int) * (maze_col + 2));
        mark[i] = (int*)malloc(sizeof(int) * (maze_col + 2));
        for (int j = 0; j <= maze_col + 1; j++) {
            if (i == 0 || i == maze_row + 1 || j == 0 || j == maze_col + 1)
                maze[i][j] = 1;
            else {
                int n;
                fscanf_s(file, "%d", &n);
                maze[i][j] = n;
            }
            mark[i][j] = 0;
        }
    }

    fclose(file);

    path(maze, mark, maze_row, maze_col);

    for (int i = 0; i <= maze_row + 1; i++) {
        free(maze[i]);
        free(mark[i]);
    }
    free(maze);
    free(mark);

    return 0;
}
