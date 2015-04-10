#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "twodarrays.h"


struct node
{
	// for our purposes in webcrawler, we will use data for the index of the link
    int data;
    struct node *next;
    struct node *previous;
    char * hyperlink;
    char * filtered_hyperlink;
    struct Array edges;

};

struct Linked_list
{
    struct node * root;
    int size;
};




void print_linked_list(struct Linked_list * a);
void delete_linked_list(struct Linked_list * a);
void move_from_to_index(struct Linked_list * a,int from_index,int to_index);
struct node * get_node_at_index(struct Linked_list * a,int index);
void randomize_linked_list(struct Linked_list * a, int max);
struct Linked_list * initialize_linked_list(int size);
void add_node(struct Linked_list * a);
int get_array_max(struct Linked_list * a, int start);
int get_max(int a, int b);
int get_min(int a, int b);
void copy_linked_list(struct Linked_list * a, struct Linked_list * b);
void remove_node(struct Linked_list * a, struct node * to_remove);
int search_for_num(struct Linked_list * a, int value);
struct node * pop_node(struct Linked_list * a, int index);
void append_node(struct Linked_list * a, struct node * n);
void add_node_location_data(struct Linked_list* a, int location, int value);
void copy_and_append(struct Linked_list * from, struct Linked_list * to);
void to_next(struct node ** n);
void to_prev(struct node ** n);
void concatenate_lists(struct Linked_list * a, struct Linked_list * b);
void insert_node_before(struct node * existing, struct node * new_node);
void insert_node_after(struct node * existing, struct node * new_node);

#endif