#include "26B_H_5B.h"
#include <time.h>

#define BUFFERSIZE 121
#define DELIMITERS "\040\n\t(){}[]<>;,./=+-!:?*"
#define VALID_IDENTIFIER_CHARS \
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_"

// Convert lines of code into tokens
static int   _tokenize(char* string, char (*list)[TOKENS][TOKEN_LENGTH], 
                       int* is_comment);
static int   _starts_valid(char* token);
static char* _preprocess(char* string, int* is_comment);

// Update the tree
static BST_NODE* _bst_create(int* count, char* input);
static BST_NODE* _bst_update(BST_TREE* tree, char *identifier_in);
static BST_NODE* _bst_update_helper(BST_NODE* root, int* count,
                                  char *identifier_in);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINITIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// BST_create: Create a BST tree node to build a tree from.
/// @return a pointer to the struct that holds the tree.
BST_TREE* BST_create(){
    BST_TREE *tree;

    tree = (BST_TREE*)malloc(sizeof(BST_TREE));
    if (!tree){  // if malloc succeeds
        printf("Error allocating Tree!\n");
        exit(1);
    } 
    tree->root = NULL;
    tree->count = 0;
    tree->filename = NULL;

    return tree;
}

/// BST_read: Reads and processes the contents of a file line by line.
///     Processing the file: Read a line from the file, then write the line 
///     number, followed by the full line into an output file. 
///     Then break the line into tokens for each c identifier using tokenize
///     Then build the tree using _bst_update() to insert or find the identifier 
///     node and enqueue to add the line to the node.
/// @param in_fp Holds file to read from 
/// @param tree the tree to build.
void BST_read(FILE* in_fp, FILE* out_fp, BST_TREE* tree){
    char rawbuffer[BUFFERSIZE] = "";
    char list[TOKENS][TOKEN_LENGTH];
    char* buffer = rawbuffer;
    int is_comment = 0, i, line = 0, num_tokens;
    BST_NODE* token_node;

    fprintf(out_fp, "\n      ~~~~~~~~ %s ~~~~~~~~\n\n", tree->filename);
    while (fgets(buffer, BUFFERSIZE, in_fp)){
        line++;
        fprintf(out_fp, "%4d| ", line);
        fprintf(out_fp, "%s", buffer);
        num_tokens = _tokenize(buffer, &list, &is_comment);

        for (i = 0; i < num_tokens; i++){
            token_node = _bst_update(tree, list[i]);
            if (!(enqueue(token_node->lines, line))){
                printf("Error enqueuing line number to ");
                printf("%s\n", token_node->identifier);
                exit(1); 
            }
        }
    }
    return;
}

/// BST_write: Writes the contents of the tree to a file name, provided by the 
///     caller. If the file exists, rename it to prevent overwriting. 
/// @param fp 
/// @param tree 
void BST_write(BST_TREE* tree, FILE* out_fp){
    time_t rawtime;
    char* out_time;
    time( &rawtime );
    out_time = ctime( &rawtime );
    // remove the /n so I can put the timezone in the same line.
    out_time = strtok(out_time, "\n");
    fprintf(out_fp, "\n\nList of Identifiers in %s. ", tree->filename);
    fprintf(out_fp,"Created on %s PDT\n\n", out_time);
    BST_print(tree->root, out_fp);
    fprintf(out_fp, "\n");

}

/// @BST_print: Print the contents of a tree to a file recursively
///       Uses read_queue to read the queue without removing items. 
///       (Could replace with dequeue if we wanted to remove the line nodes)
/// @param root Pass in the root node of the tree. 
/// @param fp An open file in write mode
void BST_print(BST_NODE* root, FILE* fp){
    // I used to allow NULL to be passed in to change this function to print to 
    // stdout
    // if (fp == NULL)
    //     fp = stdout;
    int queue_num;
    if (root){
        if(root->left)
            BST_print(root->left, fp);
        fprintf(fp, "  %-15s", root->identifier);
        // read_queue(root->lines, fp);
        while((dequeue(root->lines, &queue_num)))
            fprintf(fp, "%5d", queue_num);
        fprintf(fp, "\n");
        if (root->right)
            BST_print(root->right, fp);
    }
    else printf("Empty tree.\n");
}

/// _tokenize: Take a line of c code and parse it into a list of tokens. Include
///     only valid c identifiers, plus I did macros, which I think count
/// @param string The string to parse
/// @param list   A list of tokens to populate. 
/// @param is_comment  A flag to set when a multi-line comment starts
/// @return The number of tokens generated.
int _tokenize(char* string, 
              char (*list)[TOKENS][TOKEN_LENGTH], 
              int* is_comment){
    int num_tokens = 0;
    char* copy = **(list);
    
    if (!(string = _preprocess(string, is_comment)))
        return 0;

    // create the first token from the string pointer
    char* token = strtok(string, DELIMITERS); 
    do{
        if (!(_starts_valid(token)))
            continue;
        // if token too long - maybe not the best solution. 
        if ((strlen(token) > TOKEN_LENGTH - 1))           
          continue;
        strcpy(copy, token);
        copy += TOKEN_LENGTH;
        num_tokens++;
    } while ((token = strtok(NULL, DELIMITERS) ));// As long as strtok continues
                                                  // to create tokens
    return num_tokens;
}


/// @brief Check various high level cases that create exceptions to the main 
///       token creating loop. For example, if the string contains a 
///       pre-processor directive, we want to get the macro names from "defines"
///       but not the file names from includes. Also take the opportunity to 
///       generally clean up the string a bit before creating tokens.
/// @param string The string to process
/// @return the updated string pointer, or NULL if no tokens are detected.
char* _preprocess(char* string, int* is_comment) {

  // Check for multiline comments - One note, could break for people who 
  // use more than one multi-line comment in a single line, ie: /*a*/ b /*c*/
  // fix if there's time. 
  char* end = strstr(string, "/*");
  if (end){
    *end = '\0';
    *is_comment = 1;
  }
  // If we are currently inside a multi-line comment, 
  if (*is_comment == 1){
      string = strstr(string, "*/");
      if (string == NULL)
          return 0;
      string += 2;
      *is_comment = 0;
  }

  // If a single line comment is detected, insert a '\0' into the first '/'
  if ((end = strstr(string, "//")))
    *end = '\0';

  // // Eliminate starting whitespace - strtok will do this, but 
  // while (*string == '\040' || *string == '\t')
  //   string++;

  // // if the first char is /n, no valid tokens in line
  // if (*string == '\n')
  //   return NULL;

  // If the first char is #, 
  if (*string == '#') {
      // Sure hope no relevant preprocessor directives start with 'd'
      string++;
      if (*string == 'd') {
          strtok(string, "\040");
          string = strtok(NULL, "\040(");
          return string;
      }
      // If it's a different preprocessor directive, like includes or ifdef, 
      // then we will discard the line
      return NULL;
  }

  // Replace everything between single or double quotes with spaces
  if ((end = strpbrk(string, "\""))) {
    end++;
    while (end - string < BUFFERSIZE && *end != '\"') {
      *end = '\040';
      end++;
    }
  }
  if ((end = strpbrk(string, "\'"))) {
    end++;
    while (end - string < BUFFERSIZE && *end != '\'') {
      *end = '\040';
      end++;
    }
  }
  
  return string;
}


/// _starts_valid: Checks if a token begins with a valid character
///           Starting valid characters are all letters or '_'
/// @param token 
/// @return 1 for true, 0 for false
int _starts_valid(char* token){
  // Don't read from null.
  if (!(token)) {
    return 0;
  }
  if  ((*token >= 'a' && *token <= 'z') ||
      (*token >= 'A' && *token <= 'Z') ||
      (*token == '_'))
        return 1;
  return 0;
}

/// _bst_update: Updates the tree with the given identifier. If the tree is 
///         empty, set the root node. Uses _bst_update_helper() to recursively
///         find or create the identifier's node in the tree.
/// @param tree 
/// @param identifier_in 
/// @return a BST_NODE* for that identifier.
BST_NODE* _bst_update(BST_TREE* tree, char* identifier_in){
  BST_NODE* result = _bst_update_helper(tree->root, 
                                      &(tree->count), 
                                        identifier_in);
  if (!result){
      printf("Some kinda error occurred when inserting.");
      return NULL;
  }
  if (tree->root == NULL)
    tree->root = result;
  return result;
}


/// @brief Recursive insert traverses to the correct location in 
///         the tree based on alphabetical order using strcmp
///         If the node already exists in the tree, then no node is created.
/// @param tree The given tree, used to access the function pointer and the 
///         tree's count
/// @param root This is the part that changes for recursive calls
/// @param identifier_in Tokenized identifier generated elsewhere.
/// @return A node* for the given identifier.
BST_NODE* _bst_update_helper(BST_NODE* root, int* count, char* identifier_in) {
    int cmp_result;
    BST_NODE* curr = NULL;
    
    if (!root)
      return _bst_create(count, identifier_in);
    
    if (!(cmp_result = (strcmp(identifier_in, root->identifier))))
        curr = root;
    else if (cmp_result > 0){ 
        curr = _bst_update_helper(root->right, count, identifier_in);
        if (!root->right) 
            root->right = curr;
    } 
    else if (cmp_result < 0){  
        curr = _bst_update_helper(root->left, count, identifier_in);
        if (!root->left) 
            root->left = curr;
    }
    return curr;
}


/// @brief 
/// @param count 
/// @param input 
/// @return 
BST_NODE *_bst_create(int* count, char* input){
    BST_NODE* add = (BST_NODE*)malloc(sizeof(BST_NODE));
    if (!add){
        printf("Error creating BST_NODE!\n");
        exit(1);
    }

    char* copy = add->identifier;
    add->lines = QUEUE_create();
    add->left = NULL;
    add->right = NULL;
    strcpy(copy, input);
    *count += 1;
    return add;
}
/******************************
 * Queue STUFF
*/
/// QUEUE_create: Create a QUEUE struct to hold a new queue
/// @return A pointer to the QUEUE - NULL if failed. 
QUEUE* QUEUE_create(void){
    QUEUE* result = (QUEUE*)malloc(sizeof(QUEUE));
    if(!result){
        printf("Error creating Queue!\n");
        exit(1);
    }
    result->count = 0;
    result->front = NULL;
    result->rear = NULL;
    return result;
}

/// enqueue: adds a line number to the back of the queue
/// @param queue The given queue
/// @param line_num_in The data to queue
/// @return Success = 1 / Failure = 0
int enqueue(QUEUE* queue, int line_num_in){
    // Read is sequential by line, so line_num will always be >= last inserted 
    // line_num
    if (queue->rear) {
        if (line_num_in == queue->rear->line_num)
        return 1;
    }

    QUEUE_NODE* insert = (QUEUE_NODE*)malloc(sizeof(QUEUE_NODE));
    if(!insert) {
        printf("Error creating Queue Node!\n");
        exit(1);   
    }

    insert->prev = NULL;
    insert->line_num = line_num_in;
    insert->next = queue->rear;
    if (insert->next)
        insert->next->prev = insert;
    queue->rear = insert;

    if(queue->count == 0){
        queue->front = queue->rear;
    }

    (queue->count)++;
    return 1;
}


/// dequeue: operation removes and returns a line number from the front of the 
/// queue
/// @param queue The given queue
/// @param line_out Where to place the queue'd data
/// @return success = 1 / failure = 0;
int dequeue(QUEUE* queue, int *line_out){
    if (queue->count == 0)
        return 0;
    
    QUEUE_NODE* new_front = NULL;
    if (queue->count > 1){
      new_front = queue->front->prev;    
      new_front->next = NULL;
    }
    *line_out = queue->front->line_num;    
    free(queue->front);
    queue->front = new_front;
    (queue->count)--;

    return 1;
}

