#include "26B_H_5B.h"

int main (int argc, char *argv[]){  
    void free_tree(BST_TREE*);
    FILE *in_fp, *out_fp;
    BST_TREE* tree;
    printf("\nCreating a Cross-referrenced list from a .c source file\n");
    if (argc == 1){
        printf("Error: No input file given as argument. Please try again.");
        printf("\n%s {File_Name.c}\n", argv[0]);
        exit(1);
    }
    if (argc == 2){
      printf("Error: No output filename given as argument. Please try again.");
      printf("\n%s {File_Name.c} {Outputfile.txt}\n", argv[0]);
      exit(1);
    }
    char* valid_file = strstr(argv[1], ".c");
    if (valid_file && (*(valid_file + 2) != '\0')){
      printf("Invalid filename - File must have .c extention. Skipping\n");
      exit(1);
    }
    printf("Opening file %s for reading\n", argv[1]);
    if (!(in_fp = fopen(argv[1], "r"))) {
      printf("Error opening file \'%s\'\n\n", argv[1]);
      exit(1);
    }
    if(*argv[2] != '\0'){
      
    }
    printf("Opening file %s for writing\n", argv[2]);
    if(!(out_fp = fopen(argv[2], "w"))){
        printf("Error opening file %s for writing\n", argv[2]);
        exit(1);
    }
    tree = BST_create();
    tree->filename = argv[1];
    BST_read(in_fp, out_fp, tree);
    fclose(in_fp);
  
    // If the tree is not empty, add it to the outfile.
    if (tree->root){
      printf("Writing cross reference list to %s\n", argv[2]);
      BST_write(tree, out_fp);
    }
    else
      printf("\n~~~~~~~~ Empty tree ~~~~~~~~\n\n");

    fclose(out_fp);
    free_tree(tree);
    printf("Process complete. Cross reference list created in %s\n", argv[2]);
}

/// free_tree: Frees all nodes, queues and queue nodes within a tree
/// @param tree Sets the tree pointer to NULL to prevent reading from free'd mem
void free_tree(BST_TREE* tree){
  void free_tree_node(BST_NODE*);
  free_tree_node(tree->root);
  free(tree);
  tree = NULL;
}

/// free_tree_node: Used by free_tree to recursively free all nodes within a tree.
/// @param root 
void free_tree_node(BST_NODE* root){
  if (root->left)
    free_tree_node(root->left);
  
  if(root->right)
    free_tree_node(root->right);
  int queue_clear;
  while (dequeue(root->lines, &queue_clear));
  free(root->lines);
  free(root);
}
// Output: 
/********* 
// [root@localhost 26B_H_5B]# ./cross_reference.out test.c test.txt

// Creating a Cross-referrenced list from a .c source file
// Opening file test.c for reading
// Opening file test.txt for writing
// Writing cross reference list to test.txt
// Process complete. Cross reference list created in test.txt
// [root@localhost 26B_H_5B]# cat test.txt

//       ~~~~~~~~ test.c ~~~~~~~~

//    1| /*
//    2|      CIS26B
//    3|      Complex Structures and Functions
//    4|         - define and initialize an array of FRIEND structures
//    5|         - use structures as reference parameters (pointers)
//    6| */
//    7| #include <stdio.h>
//    8| #include <string.h>
//    9|
//   10| typedef struct{
//   11|     char *name;
//   12|     char  phone[11];
//   13|     int   age;
//   14| } FRIEND;
//   15|
//   16| // Function Declarations
//   17| void exchange( FRIEND *one, FRIEND *two );
//   18| void print_a_friend( const FRIEND *a_friend );
//   19|
//   20| int main( void )
//   21| {
//   22|     int i;
//   23|
//   24|     FRIEND a_friend = { "John", "4081112345", 25 };
//   25|     FRIEND friend_2;
//   26|     FRIEND friends[10] =
//   27|     {
//   28|         { "John", "4081112345", 25 },
//   29|         { "Mary", "4089992222", 21 },
//   30|         { "Mira", "4086668888", 22 }
//   31|     };
//   32|
//   33|     friend_2.name = "Mary";
//   34|     strcpy(friend_2.phone, "4089992222");
//   35|     friend_2.age = 21;
//   36|
//   37|     printf( "%s %s %d\n", a_friend.name, a_friend.phone, a_friend.age );
//   38|     printf( "%s %s %d\n", friend_2.name, friend_2.phone, friend_2.age );
//   39|     printf( "\n" );
//   40|
//   41|     for( i = 0; i < 3; i++ )
//   42|         print_a_friend( &friends[i] );
//   43|     printf( "\n" );
//   44|
//   45|     exchange( &friends[0], &friends[2] );
//   46|
//   47|     for( i = 0; i < 3; i++ )
//   48|         print_a_friend( &friends[i] );
//   49|     printf( "\n" );
//   50|
//   51|   return 0;
//   52| }
//   53|
//   54| /*
//   55|  This function swaps
//   56|  two FRIEND structures
//   57|  */
//   58| void exchange( FRIEND *one, FRIEND *two )
//   59| {
//   60|     FRIEND _temp; // needed for swapping
//   61|
//   62|     _temp = *one;
//   63|     *one = *two;
//   64|     *two = _temp;
//   65| }
//   66|
//   67| /*
//   68|  This function receives a pointer to a FRIEND structure
//   69|  and displays its fields on one line.
//   70|  */
//   71| void print_a_friend( const FRIEND *a_friend )
//   72| {
//   73|     printf( "%s %s %d\n", a_friend->name, a_friend->phone, a_friend->age );
//   74| }
//   75|
//   76| /*
//   77|  ************ OUTPUT *****************
//   78|
//   79|  John 4081112345 25
//   80|  Mary 4089992222 21
//   81|
//   82|  John 4081112345 25
//   83|  Mary 4089992222 21
//   84|  Mira 4086668888 22
//   85|
//   86|  Mira 4086668888 22
//   87|  Mary 4089992222 21
//   88|  John 4081112345 25
//   89|
//   90| */


// List of Identifiers in test.c. Created on Wed Mar 22 23:13:29 2023 PDT

//   FRIEND            14   17   18   24   25   26   58   60   71
//   _temp             60   62   64
//   a_friend          18   24   37   71   73
//   age               13   35   37   38   73
//   char              11   12
//   const             18   71
//   exchange          17   45   58
//   for               41   47
//   friend_2          25   33   34   35   38
//   friends           26
//   i                 22   41   42   47   48
//   int               13   20   22
//   main              20
//   name              11   33   37   38   73
//   one               17   58   62   63
//   phone             12   34   37   38   73
//   print_a_friend    18   42   48   71
//   printf            37   38   39   43   49   73
//   return            51
//   strcpy            34
//   struct            10
//   two               17   58   63   64
//   typedef           10
//   void              17   18   20   58   71

// [root@localhost 26B_H_5B]# ./cross_reference.out song.c song.txt

// Creating a Cross-referrenced list from a .c source file
// Opening file song.c for reading
// Opening file song.txt for writing
// Writing cross reference list to song.txt
// Process complete. Cross reference list created in song.txt
// [root@localhost 26B_H_5B]# cat song.c

// Row, row, row your boat
// Gently down the stream
// Merrily merrily, merrily, merrily
// Life is but a dream.

// Row, row, row your boat
// Gently down the stream
// Merrily merrily, merrily, merrily:
// Life is but a dream!

// Row, row, row your boat
// Gently down the stream
// Merrily merrily, merrily, merrily
// Life is but a dream?

// Row, row, row your boat
// Gently down the stream
// Merrily merrily, merrily, merrily
// Life is but a dream.
//*/
