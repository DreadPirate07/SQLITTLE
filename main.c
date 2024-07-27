#include "include.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

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

// processess meta commands and returns upon exit or an unrecognized command
MetaCommandResult do_meta_command(InputBuffer *input_buffer,Table* table)
{
  if (strcmp(input_buffer->buffer, ".exit") == 0)
  {
    // clost_input_buffer(input_buffer);
    exit(EXIT_SUCCESS);
    return META_COMMAND_SUCCESS;
  }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

/*  PrepareResult is just for handling result (weather the command recognition
   was a success or not). The type of Statement that as passed will be stored in
   the Statement type. That would further get processed for execution.

*/
PrepareResult prepare_statement(InputBuffer *input_buffer,Statement *statement)
{
  if (strcmp(input_buffer->buffer, "insert") == 0)
  {
    statement->type = STATEMENT_INSERT;

    // copies data onto row_to_insert struct
    int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s");
    return PREPARE_SUCCESS;
  }

  if (strcmp(input_buffer->buffer, "select") == 0)
  {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_statement(Statement *statement, Table *table) {
  switch (statement->type)
  {
  case STATEMENT_INSERT:
    printf("Insert Operation ");
    return execute_insert(statement, table);

  case STATEMENT_SELECT:
    printf("Select command ");
    return execute_select(statement, table);
  }
}

// instantiates new input buffer object and returns it
InputBuffer *new_input_buffer() {
  InputBuffer *input_buffer = (InputBuffer *)(malloc(sizeof(InputBuffer)));

  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void serialise_row(Row *row, void *destination)
{
  memcpy(destination + ID_OFFSET, &(row->id), ID_SIZE);
  memcpy(destination + USERNAME_OFFSET, &(row->username), USERNAME_SIZE);
  memcpy(destination + EMAIL_OFFSET, &(row->email), EMAIL_SIZE);
}

void deserialise_row(Row *row, void *source) 
{
  mempcpy(&(row->id), source + ID_OFFSET, ID_SIZE);
  mempcpy(&(row->email), source + EMAIL_OFFSET, EMAIL_SIZE);
  mempcpy(&(row->username), source + USERNAME_OFFSET, USERNAME_SIZE);
}

void *row_slot(Table *table, uint32_t row_number)
{
  uint32_t page_no = row_number / ROWS_PER_PAGE;
  void *page = table->pages[page_no];

  // we will allocate memory only when the page is null!
  if (page == NULL) {
    page = malloc(PAGE_SIZE);
  }

  uint32_t row_offset = row_number % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;

  return page + byte_offset;
}

Table *new_table()
{
  Table *new = (Table *)malloc(sizeof(Table));
  new->num_rows = 0;

  for (uint32_t i = 0; i < TABLE_MAX_ROWS; i++) {
    new->pages[i] = NULL;
  }
  return new;
}

void free_table(Table *table)
{
  for (uint32_t i = 0; i < PAGE_SIZE; i++)
  {
    free(table->pages[i]);
  }
  free(table);
}

void print_row(Row *row)
{
  printf("(%d,%s,%s)", row->id, row->username, row->email);
}

void print_prompt() { printf("db > "); }

ExecuteResult execute_insert(Statement *statement, Table *table)
{
  if (table->num_rows >= TABLE_MAX_ROWS)
  {
    return EXECUTE_TABLE_FULL;
  }

  Row *num_row = &statement->row_to_insert;

  serialise_row(num_row, row_slot(table, table->num_rows));
  table->num_rows++;
  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table *table)
{
  Row row;

  for (uint32_t i = 0; i < TABLE_MAX_ROWS; i++)
  {
    deserialise_row(&row, row_slot(table, i));
    print_row(&row);
  }
  return EXECUTE_SUCCESS;
}

/*  future update : return type void can be transformed
to ssize_t or an int type.
*/
/* Here, getline reads a line from stdin, including the newline character \n,
into input_buffer->buffer. It automatically allocates or resizes the buffer
as needed, with the new buffer size placed in input_buffer->buffer_length.
The function returns the number of characters read, including the newline
character, if present, but not including the terminating null byte. */
//  adjusts the recorded length of the input to exclude the
//  trailing newline character.
/* This line replaces the trailing newline character
with a null terminator ('\0'), which marks the end
of a string in C. */
void read_input(InputBuffer *input_buffer)
{

  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0)
  {
    printf("Error reading the input, exitting \n");
    exit(EXIT_FAILURE);
  }

  input_buffer->input_length = bytes_read - 1;

  input_buffer->buffer[input_buffer->input_length] = 0;
}


void close_input_buffer(InputBuffer *input_buffer)
{
  free(input_buffer->buffer);
  free(input_buffer);
}


int main(int argc, char *argv[])
{
  InputBuffer *input_buffer = new_input_buffer();
  Table *table = new_table();

  while (true)
  {
    print_prompt();
    read_input(input_buffer);

    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
      close_input_buffer(input_buffer);
      exit(EXIT_SUCCESS);
    }
    else 
    {
      printf("Unrecognized command '%s'.\n", input_buffer->buffer);
      if (input_buffer->buffer[0] == '.')
      {
        switch (do_meta_command(input_buffer, table))
        {
        case (META_COMMAND_SUCCESS):
          continue;

        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'\n", input_buffer->buffer);
          continue;
        }
      }

      Statement statement;
      switch (prepare_statement(input_buffer, &statement))
      {
      case (PREPARE_SUCCESS):
        break;

      case (PREPARE_SYNTAX_ERROR):
        printf("Syntax error. Could not parse statement.\n");
        continue;

      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n",
               input_buffer->buffer);
        continue;
      }

      switch (execute_statement(&statement, table))
      {
      case (EXECUTE_SUCCESS):
        printf("Executed.\n");
        break;

      case (EXECUTE_TABLE_FULL):
        printf("Error: Table full.\n");
        break;
      }
    }
  }
  return 0;
}