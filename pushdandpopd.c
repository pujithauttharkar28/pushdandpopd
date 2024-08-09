#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


#define INITIAL_STACK_SIZE 10
#define STACK_FILE "directory_stack.txt"

char **stack;
int top = -1;
int stack_size = INITIAL_STACK_SIZE;


void initialize_stack() {
    stack = malloc(stack_size * sizeof(char *));
    if (stack == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
}

void print_stack() {
    for (int i = top; i >= 0; i--) {
        printf("~ %s ", stack[i]);
    }
    printf("\n");
}

void resize_stack() {
    stack_size *= 2;
    stack = realloc(stack, stack_size * sizeof(char *));
    if (stack == NULL) {
        perror("Failed to reallocate memory");
        exit(EXIT_FAILURE);
    }
}

void save_stack() {
    FILE *file = fopen(STACK_FILE, "w");
    if (file == NULL) {
        perror("Failed to open file for writing");
        return;
    }

    fprintf(file, "%d\n", top);

    for (int i = 0; i <= top; i++) {
        fprintf(file, "%s\n", stack[i]);
    }
    fclose(file);
}

void load_stack() {
    FILE *file = fopen(STACK_FILE, "r");
    if (file == NULL) {
       
        return;
    }


    if (fscanf(file, "%d\n", &top) != 1) {
        //perror("Failed to read top value");
        fclose(file);
        return;
    }


    while (stack_size <= top) {
        resize_stack();
    }

    char buffer[1024];
    int index = 0;
    while (fgets(buffer, sizeof(buffer), file) && index <= top) {
        buffer[strcspn(buffer, "\n")] = '\0'; 
        stack[index++] = strdup(buffer);
        if (stack[index - 1] == NULL) {
            perror("Failed to duplicate string");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}

void pushd(const char *dir) {
    if (top == stack_size - 1) {
        resize_stack();
    }
    top = top + 1;
    stack[top] = strdup(dir);
    //printf("changed the dir: %s (top: %d)\n", stack[top], top);
    print_stack();
    save_stack();
}

void popd() 
{
    if (top == 0)
    {
        printf("popd: stack empty\n");
        return;
    } 
    else 
    {
       
        //printf("Pop the dir: %s (top: %d)\n", stack[top], top);
        
        free(stack[top]);
        stack[top] = NULL; 
        top = top - 1;
        print_stack();
        save_stack();
    }
}

void free_stack() 
{
    while (top >= 0) {
        free(stack[top--]);
    }
    free(stack);
}



int main(int argc, char *argv[]) 
{
    
    initialize_stack();
    load_stack(); 
   
   
    if (argc < 2) {
        printf("Usage: %s pushd|popd [directory]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "pushd") == 0) {
        if (argc < 3) {
            printf("Usage: %s pushd [directory]\n", argv[0]);
            return 1;
        }
        pushd(argv[2]);
    } else if (strcmp(argv[1], "popd") == 0) {
        popd();
    } else {
        printf("Invalid command\n");
    }

    return 0;
}
