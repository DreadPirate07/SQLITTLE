#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

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
    PREPARE_UNRECOGNIZED_STATEMENT,
} PrepareResult;

// Defines operation statements for easier handling of keywords
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

// small wrapper around the state we need to store to interact with getline() function
typedef struct
{
    char *buffer;         // character pointer
    size_t buffer_length; // length of buffer originally returned
    ssize_t input_length; // actual buffer length excluding trailing newline character
} InputBuffer;

typedef struct
{
    StatementType type;
} Statement;

// processess meta commands and returns upon exit or an unrecognized command
MetaCommandResult do_meta_command(InputBuffer *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        // clost_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
        return META_COMMAND_SUCCESS;
    }
    else
        return META_COMMAND_UNRECOGNIZED_COMMAND;
}

/*  PrepareResult is just for handling result (weather the command recognition was
    a success or not). The type of Statement that as passed will be stored in the
    Statement type. That would further get processed for execution.

*/
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement)
{
    if (strcmp(input_buffer->buffer, "insert") == 0)
    {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement *statement)
{
    switch (statement->type)
    {
    case STATEMENT_INSERT:
        printf("Insert Operation ");
        break;
    case STATEMENT_SELECT:
        printf("Select command ");
        break;
    }
}
// instantiates new input buffer object and returns it
InputBuffer *new_input_buffer()
{
    InputBuffer *input_buffer = (InputBuffer *)(malloc(sizeof(InputBuffer)));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_promt()
{
    printf("db > ");
}

/*  future update : return type void can be transformed
    to ssize_t or an int type.
*/
void read_input(InputBuffer *input_buffer)
{
    /* Here, getline reads a line from stdin, including the newline character \n,
    into input_buffer->buffer. It automatically allocates or resizes the buffer
    as needed, with the new buffer size placed in input_buffer->buffer_length.
    The function returns the number of characters read, including the newline character,
    if present, but not including the terminating null byte. */

    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0)
    {
        printf("Error reading the input, exitting \n");
        exit(EXIT_FAILURE);
    }

    //  adjusts the recorded length of the input to exclude the
    //  trailing newline character.

    input_buffer->input_length = bytes_read - 1;

    /* This line replaces the trailing newline character
    with a null terminator ('\0'), which marks the end
    of a string in C. */
    input_buffer->buffer[input_buffer->input_length] = 0;
}

void clost_input_buffer(InputBuffer *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

int main(int argc, char *argv[])
{
    InputBuffer *input_buffer = new_input_buffer();
    while (1)
    {
        print_promt();
        read_input(input_buffer);
        // fprintf(stdout,input_buffer->buffer);
        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command %s \n", input_buffer->buffer);
                continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized statement %s \n", input_buffer->buffer);
            continue;
        }
        execute_statement(&statement);
        printf("Executed. \n");
    }
    return 0;
}