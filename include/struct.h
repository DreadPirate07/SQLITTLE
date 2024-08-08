#ifndef STRUCT_H
#define STRUCT_H

#include "include.h"

// indicate success or failure.
typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;


// Informs more about the command status
typedef enum 
{
  PREPARE_SUCCESS,
  PREPARE_NEGATIVE_ID,
  PREPARE_STRING_TOO_LONG,
  PREPARE_SYNTAX_ERROR,
  PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;


// Defines operation statements for easier handling of keywords
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;


typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY
} ExecuteResult;

typedef enum
{
    NODE_INTERNAL,
    NODE_LEAF
} NodeType;


// for storing of parsed arguments into a struct
typedef struct
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;


typedef struct
{
    StatementType type;
    Row row_to_insert;
} Statement;


// small wrapper around the state we need to store to interact with getline() function
typedef struct
{
    char *buffer;         // character pointer
    size_t buffer_length; // length of buffer originally returned
    ssize_t input_length; // actual buffer length excluding trailing newline character
} InputBuffer;


typedef struct
{
  int file_descriptor;
  uint32_t file_length;
  uint32_t num_pages;
  void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct
{
  Pager* pager;
  uint32_t root_page_num;
} Table;

typedef struct
{
  Table* table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table;  // Indicates a position one past the last element
} Cursor;


const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;                                  // 0 offset
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;          // 4 offset
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE; // 36 offset
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096; // ideal page size
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;


const static uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const static uint32_t NODE_TYPE_OFFSET = 0;
const static uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const static uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const static uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const static uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const static uint8_t COMMON_NODE_HEADER_SIZE =
    NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;


const static uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
const static uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const static uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
const static uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET =
    INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const static uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                           INTERNAL_NODE_NUM_KEYS_SIZE +
                                           INTERNAL_NODE_RIGHT_CHILD_SIZE;


const static uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
const static uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
const static uint32_t INTERNAL_NODE_CELL_SIZE =
    INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;
/* Keep this small for testing */
const static uint32_t INTERNAL_NODE_MAX_CELLS = 3;


const static uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const static uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const static uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof(uint32_t);
const static uint32_t LEAF_NODE_NEXT_LEAF_OFFSET =
    LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
const static uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                       LEAF_NODE_NUM_CELLS_SIZE +
                                       LEAF_NODE_NEXT_LEAF_SIZE;


const static uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const static uint32_t LEAF_NODE_KEY_OFFSET = 0;
const static uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const static uint32_t LEAF_NODE_VALUE_OFFSET =
    LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const static uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const static uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const static uint32_t LEAF_NODE_MAX_CELLS =
    LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;
const static uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const static uint32_t LEAF_NODE_LEFT_SPLIT_COUNT =
    (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;


// methods 
Table* new_table();
InputBuffer* new_input_buffer();
ExecuteResult execute_insert(Statement* statement,Table* table);
ExecuteResult execute_select(Statement* statement,Table* table);
ExecuteResult execute_statement(Statement *statement,Table* table);
MetaCommandResult do_meta_command(InputBuffer *input_buffer,Table* table);
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);

void print_prompt();
void print_row(Row* row);
void free_table(Table* table);
void read_input(InputBuffer *input_buffer);
void deserialise_row(Row* row,void* source);
void serialize_row(Row* row,void* destination);
void* row_slot(Table* table,uint32_t row_number);
void close_input_buffer(InputBuffer *input_buffer);



#endif