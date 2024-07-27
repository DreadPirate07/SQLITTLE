#include <stdio.h>
#include <stdint.h>

#define COLUMN_USERNAME_SIZE 35
#define COLUMN_EMAIL_SIZE 255

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define TABLE_MAX_PAGES 100


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
  PREPARE_SYNTAX_ERROR,
  PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;


// Defines operation statements for easier handling of keywords
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

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


// table struct which holds the table data!
typedef struct {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;


typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;


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

