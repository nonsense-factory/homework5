#ifndef H_26B_H_5B
#define H_26B_H_5B

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Adjust this for larger/more tokens
#define TOKENS 30
#define TOKEN_LENGTH 26

// ~~~~~~~~ Queues ~~~~~~~~ //
typedef struct queue_node QUEUE_NODE;
typedef struct queue QUEUE;
struct queue{
    QUEUE_NODE* front;
    QUEUE_NODE* rear;
    int count;
};
struct queue_node{
    QUEUE_NODE *prev;
    QUEUE_NODE *next;
    int line_num;
};

QUEUE* QUEUE_create(void);
int enqueue( QUEUE *queue, int line); 
int dequeue( QUEUE *queue, int *out_line); 


// ~~~~~~~~ Trees ~~~~~~~~~ //
typedef struct bst_node BST_NODE;
typedef struct bst_tree BST_TREE;
struct bst_node{
    BST_NODE *left;
    BST_NODE *right;
    char identifier[TOKEN_LENGTH];
    QUEUE *lines;
};
struct bst_tree{
    char* filename;
    int count;
    BST_NODE *root; 
};  

BST_TREE* BST_create(); 
void BST_read(FILE* in_fp, FILE* out_fp, BST_TREE* tree); 
void BST_write(BST_TREE* tree, FILE* out_fp); 
void BST_print(BST_NODE* root, FILE* fp);

#endif
