#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// small wrapper around the state we need to store to interact with getline() function
typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

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
    printf(" db > ");
}

/*  future update : return type void can be transformed
    to ssize_t or an int type.
*/
void read_input(InputBuffer *input_buffer)
{
    /*Here, getline reads a line from stdin, including the newline character,
    into input_buffer->buffer. It automatically allocates or resizes the buffer
    as needed, with the new buffer size placed in input_buffer->buffer_length.
    The function returns the number of characters read, including the newline character,
    if present, but not including the terminating null byte.*/

    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0)
    {
        printf("Error reading the input, exitting \n");
        exit(EXIT_FAILURE);
    }

    //  adjusts the recorded length of the input to exclude the
    //  trailing newline character.

    input_buffer->input_length = bytes_read - 1;

    /*This line replaces the trailing newline character
    with a null terminator ('\0'), which marks the end
    of a string in C.*/
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
        if (strcmp(input_buffer->buffer, ".exit") == 0)
        {
            clost_input_buffer(input_buffer);
            printf("Status: Exitting");
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Unrecognized Command \n");
        }
    }
    return 0;
}