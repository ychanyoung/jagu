#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#define MAX_SIZE 50




typedef enum { head, entry } tagfield;
typedef struct matrix_node* matrix_pointer;

typedef struct entry_node {
	int row;
	int col;
	int value;
}entry_node;


typedef struct matrix_node {
	matrix_pointer down;
	matrix_pointer right;
	tagfield tag;
	union {
		matrix_pointer next;
		entry_node entry;
	} u;
}matrix_node;

matrix_pointer hdnode[MAX_SIZE];


int compare(int x, int y) {
	if (x > y) return 1;
	else if (x == y) return 0;
	else return -1;
}

matrix_pointer mread(FILE* file, int** arr, int r, int c) {
	// file에서 matrix 자료를 읽어 리스트에 저장한다.
	int row, col;
	if (file != NULL) fscanf(file, "%d %d", &row, &col);
	else {
		row = r;
		col = c;
	}
	matrix_pointer ret = (matrix_pointer)malloc(sizeof(matrix_node));
	ret->u.entry.row = row; ret->u.entry.col = col; ret->tag = entry;

	matrix_pointer rowh = (matrix_pointer)malloc(sizeof(matrix_node));
	rowh->tag = head; rowh->right = rowh; ret->down = rowh; 
	rowh->down = ret;
	for (int i = 0; i < row-1; i++) {
		matrix_pointer temp = (matrix_pointer)malloc(sizeof(matrix_node));
		temp->tag = head;
		rowh->u.next = temp;
		temp->right = temp;
		rowh = temp;
	}
	rowh->u.next = ret;
	rowh = ret->down;


	matrix_pointer colh = (matrix_pointer)malloc(sizeof(matrix_node));
	colh->tag = head; colh->down = colh; ret->right = colh;
	colh->right = ret;
	for (int i = 0; i < col - 1; i++) {
		matrix_pointer temp = (matrix_pointer)malloc(sizeof(matrix_node));
		temp->tag = head;
		colh->u.next = temp;
		temp->down = temp;
		colh = temp;
	}
	colh->u.next = ret;
	colh = ret->right;


	int a;
	matrix_pointer temp;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (file != NULL) fscanf(file, "%d", &a);
			else a = arr[i][j];
			if (a != 0) {
				matrix_pointer sin = (matrix_pointer)malloc(sizeof(matrix_node));
				sin->tag = entry; sin->u.entry.row = i; sin->u.entry.col = j; sin->u.entry.value = a;

				//row
				for (int k = 0; k < i; k++) rowh = rowh->u.next;
				temp = rowh;
				for (; temp->right->tag != head; temp = temp->right);
				sin->right = temp->right;
				temp->right = sin;
				rowh = ret->down;


				//col
				for (int k = 0; k < j; k++) colh = colh->u.next;
				temp = colh;
				for (; temp->down->tag != head; temp = temp->down);
				sin->down = temp->down;
				temp->down = sin;
				colh = ret->right;
			}
		}
	}


	return ret;
}


void mwrite(matrix_pointer node)
{
	if (node == NULL) return;


	int i;
	matrix_pointer temp, head = node->down;
	for (i = 0; i < node->u.entry.row; i++) {
		int num = 0;
		for (temp = head->right; temp != head; temp = temp->right) {
			for (; temp->u.entry.col != num; ) {
				printf("0 ");
				num++;
			}
			printf("%d ", temp->u.entry.value);
			num++;
		}
		for (; node->u.entry.col != num; ) {
			printf("0 ");
			num++;
		}
		printf("\n");
		head = head->u.next;
	}

	printf("\n");
}

void merase(matrix_pointer* node)
{


	int i, num_heads;
	matrix_pointer x, y, head = (*node)->down;
	for (i = 0; i < (*node)->u.entry.row; i++) {
		y = head->right;
		while (y != head) {
			x = y; y = y->right; free(x);
		}
		x = head; head = head->u.next; free(x);
	}
	// free remaining head nodes
	y = (*node)->right;
	while (y != *node) {
		x = y; y = y->u.next; free(x);
	}
	free(*node) ; *node = NULL;
}

matrix_pointer msub(matrix_pointer a, matrix_pointer b) {
	// matrix subtraction
	matrix_pointer ret = (matrix_pointer)malloc(sizeof(matrix_node));

	matrix_pointer ta = a->down;
	matrix_pointer tb = b->down;

	int** arr = (int**)malloc(a->u.entry.row * sizeof(int*));
	for (int i = 0; i < a->u.entry.row; i++) {
		arr[i] = (int*)calloc(a->u.entry.col, sizeof(int));
	}

	while (ta->tag != entry && tb->tag != entry) {
		ta = ta->right;
		tb = tb->right;
		while (ta->tag != head && tb->tag != head) {
			switch (compare(ta->u.entry.col, tb->u.entry.col)) {
			case 0:
				if (ta->u.entry.value - tb->u.entry.value) {
					//A+B 추가
					arr[ta->u.entry.row][ta->u.entry.col] = ta->u.entry.value - tb->u.entry.value;
				}
				ta = ta->right;
				tb = tb->right;
				break;
			case 1:
				//B 추가
				arr[tb->u.entry.row][tb->u.entry.col] = -1 * tb->u.entry.value;
				tb = tb->right;
				break;
			case -1:
				//A 추가
				arr[ta->u.entry.row][ta->u.entry.col] = ta->u.entry.value;
				ta = ta->right;
			}
		}

		for (; ta->tag != head; ta = ta->right); //A추가
		for (; tb->tag != head; tb = tb->right); //B추가
		
		ta = ta->u.next;
		tb = tb->u.next;

	}
	ret = mread(NULL, arr, a->u.entry.row, a->u.entry.col);

	return ret;
}



int main() {
	matrix_pointer a, b, d;

	FILE* file1 = fopen("A.txt", "r");
	FILE* file2 = fopen("B.txt", "r");

	if (file1 == NULL) {
		return 1;
	}

	a = mread(file1, NULL, 0, 0);

	if (file2 == NULL) {
		return 1;
	}

	b = mread(file2, NULL, 0, 0);



	mwrite(a);
	mwrite(b);


	d = msub(a, b);
	mwrite(d);

	merase(&d);
	merase(&a);
	merase(&b);

	mwrite(a);
	mwrite(b);
	mwrite(d);






	return 0;
}