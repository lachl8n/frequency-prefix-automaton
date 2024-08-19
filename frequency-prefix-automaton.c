/* Program to generate text based on the context provided by input prompts.

  Skeleton program written by Artem Polyvyanyy, http://polyvyanyy.com/,
  September 2023, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.
  All included code is (c) Copyright University of Melbourne, 2023.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Lachlan Chue 1643779
  Dated:     13/10/2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define SDELIM "==STAGE %d============================\n" // stage delimiter
#define MDELIM "-------------------------------------\n"  // delimiter of -'s
#define THEEND "==THE END============================\n"  // end message
#define NOSFMT "Number of statements: %d\n"               // no. of statements
#define NOCFMT "Number of characters: %d\n"               // no. of chars
#define NPSFMT "Number of states: %d\n"                   // no. of states
#define TFQFMT "Total frequency: %d\n"                    // total frequency
#define GREATER 1                                         // indicate greater
#define LESSTHAN 0                                        // indicate less than
#define MAXCHS 37                                         // max output chars

#define CRTRNC '\r' // carriage return character

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef struct state state_t; // a state in an automaton
typedef struct node node_t;   // a node in a linked list

struct node
{                          // a node in a linked list of transitions has
    char str;              // ... a transition string
    state_t *state;        // ... the state reached via the string,
    node_t *next;          // ... a link to the next node in the list, and
    char compressed[1000]; // ... a full string array for compression.
};

typedef struct
{                 // a linked list consists of
    node_t *head; // ... a pointer to the first node and
    node_t *tail; // ... a pointer to the last node in the list.
} list_t;

struct state
{                      // a state in an automaton is characterized by
    unsigned int id;   // ... an identifier,
    unsigned int freq; // ... frequency of traversal,
    int visited;       // ... visited status flag, and
    list_t *outputs;   // ... a list of output states.
};

typedef struct
{                     // an automaton consists of
    state_t *ini;     // ... the initial state, and
    unsigned int nid; // ... the identifier of the next new state.
} automaton_t;

/* USEFUL FUNCTIONS ----------------------------------------------------------*/
char mygetchar(void);                    // getchar() that skips carriage returns
automaton_t *make_empty_automaton(void); // makes a new automaton

/* inserts new node at head of child nodes list */
list_t *insert_at_head(list_t *list, char value, int state_id);

list_t *make_empty_list(void); // makes an empty list for child nodes

/* searches through the child nodes to find a node of matching character value */
node_t *search_children(list_t *list, node_t *child_node, char value);

/* searches through child nodes and finds node with highest frequency */
node_t *search_freq(list_t *list, node_t *child_node);

/* compares the ASCII value of two nodes */
node_t *compare_ascii(node_t *node1, node_t *node2, int sign);

/* searches through  child nodes to find the node of the smallest ASCII value */
node_t *find_smallest_label(list_t *list, node_t *child_node);

/* frees tree in recursive function */
static void recursive_free_tree(state_t *);

/* frees tree */
void free_tree(automaton_t *automaton);

/* frees list */
void free_list(list_t *list);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int main(int argc, char *argv[])
{
    /*==========================================================================
    STAGE 0
    ==========================================================================*/
    /* initialise the automaton */
    automaton_t *automaton;
    automaton = make_empty_automaton();

    /* initialise the node currently selected in the automaton */
    node_t *selected_node;
    selected_node = (node_t *)malloc(sizeof(*selected_node));
    assert(selected_node != NULL);

    /* initialise the state currently selected in the automaton */
    state_t *selected_state;
    selected_state = (state_t *)malloc(sizeof(*selected_state));
    automaton->ini->id = 0;
    automaton->ini->freq = 1;
    automaton->ini->visited = 0;
    selected_state = automaton->ini;
    assert(selected_state != NULL);

    /* initialise the child nodes list */
    list_t *child_nodes;
    child_nodes = (list_t *)malloc(sizeof(*child_nodes));
    child_nodes = automaton->ini->outputs;
    child_nodes = make_empty_list();

    char c;
    int stage_count = 0, state_counter = 1, char_count = 0, statement_count = 0;
    int new_line = 0;

    while ((c = mygetchar()) != EOF)
    {
        if (c == '\n')
        {

            if (new_line == 1)
            {
                /* stop reading input if two consecutive newline characters found */
                break;
            }
            else
            {
                /* if at the end of statement, set frequency to 0 to indicate leaf node*/
                (selected_node->state->freq) = 0;
                statement_count++;
                new_line = 1;
                /* reset selected state and child nodes to beginning of automaton */
                selected_state = automaton->ini;
                child_nodes = automaton->ini->outputs;
                continue;
            }
        }
        else
        {
            new_line = 0;
            char_count++;
        }

        if (child_nodes->head == NULL)
        {
            /* create new child node at head of list if list is empty */
            list_t *new_children;
            new_children = (list_t *)malloc(sizeof(*new_children));
            new_children = make_empty_list();
            new_children = insert_at_head(new_children, c, state_counter);
            state_counter++;
            selected_state->outputs = new_children;
            selected_state = selected_state->outputs->head->state;
            selected_node = new_children->head;
            /* set selected child nodes as child nodes of new node */
            child_nodes = selected_node->state->outputs;
            child_nodes = make_empty_list();
        }
        else
        {
            /* if child nodes exist, search and test if character matches */
            selected_node = search_children(child_nodes, child_nodes->head, c);
            if (selected_node == NULL)
            {
                /* if character does not match, create new node at head */
                list_t *new_children;
                new_children = (list_t *)malloc(sizeof(*new_children));
                new_children = make_empty_list();
                new_children = child_nodes;
                new_children = insert_at_head(new_children, c, state_counter);
                state_counter++;
                selected_node = new_children->head;
                selected_state = selected_node->state;
                selected_state->outputs = new_children;
                selected_state = selected_state->outputs->head->state;
                child_nodes = selected_node->state->outputs;
                child_nodes = make_empty_list();
            }
            else
            {
                /* if character matches, increment frequency */
                (selected_node->state->freq)++;
                child_nodes = selected_node->state->outputs;
            }
        }
    }
    /* print delimiter and count information */
    printf(SDELIM, stage_count);
    printf(NOSFMT, statement_count);
    printf(NOCFMT, char_count);
    printf(NPSFMT, state_counter);
    stage_count++; // increment stage count to indicate end of stage 0
    /*==========================================================================
    STAGE 1
    ==========================================================================*/
    printf(SDELIM, stage_count);

    /* reset selected state and child nodes to beginning of automaton */
    selected_state = automaton->ini;
    child_nodes = automaton->ini->outputs;
    new_line = 0;          // indicator when a newline character is found
    int printed_chars = 0; // counter for number of characters printed
    int do_not_print = 0;  // indicator when the program should halt printing
    int node_found = 1;    // indicator for when a node is found

    while ((c = mygetchar()) != EOF)
    {
        printed_chars++;
        if (do_not_print == 1 && selected_node == NULL)
        {
            /* if node is NULL and program should not print,
            read the remaining characters in the input until newline occurs */
            while (c != '\n')
            {
                c = mygetchar();
            }
        }

        if (printed_chars > MAXCHS)
        {
            /* if max number of characters have been printed, do not print */
            do_not_print = 1;
        }

        if (c == '\n')
        {
            if (new_line == 1)
            {
                /* stop reading input if two consecutive newline
                characters found */
                break;
            }
            else
            {
                /* otherwise indicate newline character found */
                new_line = 1;
            }
            if (do_not_print == 0)
            {
                /* if program can still print, print ellipses
                and add 3 to counter to compensate */
                printf("...");
                printed_chars += 3;
            }
            else
            {
                /* as end of statement, reset flag for next statement*/
                do_not_print = 0;
            }

            if (node_found == 0)
            {
                /* if node is found, can print newline for next output line */
                printf("\n");
            }
            else
            {
                while ((selected_node != NULL) &&
                       (selected_node->state->freq != 0) && (do_not_print == 0))
                {
                    /* if node is not NULL, does not have a frequency of 0 and can
                    print, find node of highest frequency and print */
                    selected_node = search_freq(child_nodes, child_nodes->head);
                    printf("%c", selected_node->str);
                    printed_chars++;
                    child_nodes = selected_node->state->outputs;
                }
                /* print newline at end of loop */
                printf("\n");
                printed_chars = 0; // reset counter for statement characters
            }
            node_found = 1; // reset flag for if node is found
            /* reset selected state and child nodes to beginning of automaton
            for next statement */
            selected_state = automaton->ini;
            child_nodes = automaton->ini->outputs;
            continue;
        }
        new_line = 0;
        if (do_not_print == 0)
        {
            /* if program can print, print character read */
            printf("%c", c);
            if (child_nodes == NULL)
            {
                /* if the node list is NULL, increment '.' for avaliable space */
                for (int i = 0; i < 3; i++)
                {
                    printf(".");
                    printed_chars++;

                    if (printed_chars > 36)
                    {
                        break;
                    }
                }
                continue;
            }
        }
        if (child_nodes != NULL)
        {
            /* if node list is not NULL, search for node with matching
            child node  */
            selected_node = search_children(child_nodes, child_nodes->head, c);
        }
        if (selected_node == NULL)
        {
            /* if node not found, set flag */
            node_found = 0;
        }
        else
        {
            /* if node is found, set list as child nodes of found node */
            child_nodes = selected_node->state->outputs;
        }
    }

    stage_count++;
    /*==========================================================================
    STAGE 2
    ==========================================================================*/
    printf(SDELIM, stage_count);

    state_counter = 0;
    int frequency_count = 0;

    selected_state = automaton->ini;
    child_nodes = automaton->ini->outputs;
    selected_node = automaton->ini->outputs->head;

    child_nodes = selected_node->state->outputs;

    if (child_nodes->head->next == NULL)
    {
        selected_node->compressed[0] = selected_node->str;
        selected_node->compressed[1] = child_nodes->head->str;
        selected_node->compressed[2] = '\0';
    }

    printf(NPSFMT, state_counter);
    printf(TFQFMT, frequency_count);

    printf(MDELIM);

    free_tree(automaton);

    printf(THEEND);

    return EXIT_SUCCESS; // algorithms are fun!!!
}

/* USEFUL FUNCTIONS ----------------------------------------------------------*/

// An improved version of getchar(); skips carriage return characters.
// NB: Adapted version of the mygetchar() function by Alistair Moffat
char mygetchar()
{
    int c;
    while ((c = getchar()) == CRTRNC)
        ;
    return c;
}

// Making a new empty automaton; adapted version of the make_new_tree function by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/treeops.c
automaton_t *make_empty_automaton(void)
{
    automaton_t *automaton;
    automaton = (automaton_t *)malloc(sizeof(*automaton));
    assert(automaton != NULL);
    automaton->ini = (state_t *)malloc(sizeof(*automaton->ini));
    automaton->nid = 1;
    return automaton;
}

// Inserting new node at head of list and initialising its state;
// adapted version of the insert_at_head function by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
list_t *insert_at_head(list_t *list, char value, int state_id)
{
    node_t *new;
    new = (node_t *)malloc(sizeof(*new));
    assert(list != NULL && new != NULL);
    new->str = value;
    new->next = list->head;
    list->head = new;

    state_t *new_state;
    new_state = (state_t *)malloc(sizeof(*new_state));
    new->state = new_state;
    new->state->id = state_id;
    new->state->freq = 1;
    new->state->visited = 0;

    if (list->tail == NULL)
    {
        /* this is the first insertion into the list */
        list->tail = new;
    }
    return list;
}

// Make an empty list when a child nodes list is created; by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
list_t *make_empty_list(void)
{
    list_t *list;
    list = (list_t *)malloc(sizeof(*list));
    assert(list != NULL);
    list->head = list->tail = NULL;
    return list;
}

// Search through child nodes to determine if node of matching value exists
node_t *search_children(list_t *list, node_t *child_node, char value)
{
    int found = 0;
    while (found == 0)
    {
        if (child_node == NULL)
        {
            return NULL;
        }

        if ((child_node->str) == value)
        {
            found = 1;
            break;
        }
        else
        {
            found = 0;
            child_node = child_node->next;
        }
    }

    if (found == 1)
    {
        return child_node;
    }
    else
    {
        return NULL;
    }
}

// Search through child nodes to find node with the highest frequency
node_t *search_freq(list_t *list, node_t *child_node)
{
    unsigned int highest_freq = 0;
    node_t *mode_node;
    while (child_node != NULL)
    {
        if (child_node->state->freq == 0)
        {
            return child_node;
        }
        if (child_node->state->freq > highest_freq)
        {
            highest_freq = child_node->state->freq;
            mode_node = child_node;
        }
        else if (child_node->state->freq == highest_freq)
        {
            mode_node = compare_ascii(child_node, mode_node, GREATER);
        }
        child_node = child_node->next;
    }
    return mode_node;
}

// Compare ASCII values of nodes to determine which is greater or less
node_t *compare_ascii(node_t *node1, node_t *node2, int sign)
{
    if (sign == GREATER)
    {
        if (node1->str > node2->str)
        {
            return node1;
        }
        else
        {
            return node2;
        }
    }
    else
    {
        if (node1->str < node2->str)
        {
            return node1;
        }
        else
        {
            return node2;
        }
    }
}

// Search for the node with the smallest ASCII value label
node_t *find_smallest_label(list_t *list, node_t *child_node)
{
    node_t *smallest_label;
    smallest_label = child_node;
    while (child_node != NULL)
    {
        if (child_node->str < smallest_label->str)
        {
            smallest_label = child_node;
        }
        else
        {
            smallest_label = compare_ascii(child_node, smallest_label, LESSTHAN);
        }
        child_node = child_node->next;
    }
    return smallest_label;
}

// Freeing the automaton as a tree;
// adapted version of the recursive_free_tree function by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/treeops.c
static void
recursive_free_tree(state_t *ini)
{
    if (ini)
    {
        recursive_free_tree(ini->outputs->head->state);
        free_list(ini->outputs);
        free(ini);
    }
}

// Freeing the automaton as a tree;
// adapted version of the recursive_free_tree function by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/treeops.c
/* Release all memory space associated with the tree
   structure. */
void free_tree(automaton_t *automaton)
{
    assert(automaton != NULL);
    recursive_free_tree(automaton->ini);
    free(automaton);
}

// Frees list of child nodes; by
// Alistair Moffat: https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
void free_list(list_t *list)
{
    node_t *curr, *prev;
    assert(list != NULL);
    curr = list->head;
    while (curr)
    {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(list);
}

/* THE END -------------------------------------------------------------------*/