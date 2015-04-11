#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


void to_next(struct node ** n)
{
    *n = (*n)->next;
}

void to_prev(struct node ** n)
{
    *n = (*n)->prev;
}

void copy_and_append(struct Linked_list * from, struct Linked_list * to)
{
    struct node * temp = from->root;

    do
    {
        add_node(to);
        ((to->root)->prev)->index = temp->index;
        temp =temp->next;

    }while(temp != from->root);
}

void copy_linked_list(struct Linked_list * a, struct Linked_list * b)
{   
    struct node * tempa, * tempb;

    tempa = a->root;
    tempb = b->root;

    do 
    {
        tempb->index = tempa->index;
        tempa = tempa->next;
        tempb = tempb->next;
    }while(tempa != a->root);
}

void add_node(struct Linked_list * a)
{
    struct node * new_node = (struct node *)malloc(sizeof(struct node));
    if(a->root != NULL)
    {
        new_node->prev = (a->root)->prev;
        new_node->next = a->root;
        ((a->root)->prev)->next = new_node;
        (a->root)->prev = new_node;
        a->size += 1;
    }
    else
    {
        new_node->prev = new_node;
        new_node->next = new_node;
        a->root = new_node;
        a->size += 1;
    }
}

void append_node(struct Linked_list * a, struct node * n)
{
    // printf("a->size = %d\n",a->size );
    // printf("prev link = %s, next link = %s\n",a->root->prev->hyperlink, a->root->next->hyperlink );
    if(a->root == NULL)
    {
        a->root = n;
        n->prev = n;
        n->next = n;
    }
    else
    {        
        n->prev = (a->root)->prev;
        n->next = a->root;
        ((a->root)->prev)->next = n;
        (a->root)->prev = n;
    }

    a->size += 1;
    // printf("prev link = %s, next link = %s\n",a->root->prev->hyperlink, a->root->next->hyperlink );

}

void add_node_location_index(struct Linked_list* a, int location, int value)
{
    add_node(a);
    ((a->root)->prev)->index = value;

    move_from_to_index(a, (int)(a->size - 1), location);
}


void remove_node(struct Linked_list * a, struct node * to_remove)
{
    if(a->root == to_remove)
    {
        if(a->size == 1)
        {
            a->root = NULL;
        }
        else
        {
            a->root = to_remove->next;
        }
    }

    if(a->size > 1)
    {
        (to_remove->next)->prev = to_remove->prev;
        (to_remove->prev)->next = to_remove->next;

        free(to_remove->next);
        free(to_remove->prev);
    }
    a->size -= 1;

}

int search_for_num(struct Linked_list * a, int value)
{
    struct node * test = a->root;

    int result = 0;

    do
    {
        if (test->index == value)
        {
            result = 1;
            break;
        }
        else
        {
            test = test->next;
        }
    }while(test != a->root);

    return result;
}

struct Linked_list * initialize_linked_list(int size)
{

    struct Linked_list * a = (struct Linked_list *)malloc(sizeof(struct Linked_list));
    int i;

    a->size = 0;
    a->root = NULL;

    // ((a->root)->edges).size == 0;

    for(i = 0; i < size; i++)
    {   
        add_node(a);
    }

    return a;
}

void randomize_linked_list(struct Linked_list * a, int max)
{
    if(a->root != NULL)
    {
        struct node * temp;

        temp = a->root;

        do
        {   
            temp->index = rand() % max;
            temp = temp->next;
        }while(temp != a->root);
    }
    else
    {
        printf("Linked list must be initialized first\n");
    }
}

struct node * get_node_at_index(struct Linked_list * a,int index)
{
    struct node * temp;

    // printf("here\n");


    temp = a->root;

    int i;

    for (i = 0; i < index; i ++)
    {
        // printf("i = %d, index = %d\n",i, index );
        temp = temp->next;
    }

    return temp;
}

struct node * pop_node(struct Linked_list * a, int index)
{
    struct node * temp = get_node_at_index(a,index);

    if(a->size > 1)
    {
        if (index == 0)
        {
            a->root = (a->root)->next;
        }

        (temp->prev)->next = temp->next;
        (temp->next)->prev = temp->prev;
        temp->next = NULL;
        temp->prev = NULL;
    }
    else
    {
        a->root = NULL;
    }

    return temp;
}

void move_from_to_index(struct Linked_list * a,int from_index,int to_index)
{
    struct node * to_move;
    struct node * old;
    // struct node *temp;

    if ((from_index >=0) && (from_index < a->size) && (to_index >=0) && (to_index < a->size))
    {
        if((from_index == 0) && (to_index == a->size - 1))
        {
            a->root = (a->root)->next;
        } 
        else if((from_index == a->size-1) && (to_index == 0))
        {
            a->root = (a->root)->prev;
        }
        else
        {

            to_move = get_node_at_index(a,from_index);

            old = get_node_at_index(a,to_index);

            (to_move->prev)->next = to_move->next;
            (to_move->next)->prev = to_move->prev;

            if(to_index < from_index)
            {
                to_move->next = old;
                to_move->prev = old->prev;
                (old->prev)->next = to_move;
                old->prev = to_move;

                if(to_index == 0)
                {
                    a->root = to_move;
                }
            }
            else
            {               

                if(to_move == a->root)
                {
                    a->root = to_move->next;
                }

                to_move->next = old->next;
                to_move->prev = old;
                (old->next)->prev = to_move;
                old->next = to_move;
                
            }
        }
    }
    else
    {
        printf("from_index = %d, to_index = %d\n",from_index, to_index );
        printf("problem\n\n\n\n\n\n\n");
        sleep(3);
    }

}

void delete_linked_list(struct Linked_list * a)
{
    // struct node * temp = a->root;

    struct node * temp1 = a->root->next;
    struct node * temp2;

    if(a->size > 1)
    {
        do
        {   
            temp2 = temp1->next;
            free(temp1);
            temp1 = temp2;
        }while(temp1 != a->root);
    }

    free(a->root);
}

void print_linked_list(struct Linked_list * a)
{

    struct node * temp;

    temp = a->root;

    do
    {

        printf("link[%d] = \t%s",temp->index, (temp->hyperlink));

        if ((int)((temp->edges).size) > 0)
        {
            printf(", \tedges = ");
            print_Array(temp->edges);
        }
        printf("\n");

        to_next(&temp);
    }while(temp != a->root);
}

int get_array_max(struct Linked_list * a, int start)
{
    struct node * test;
    int max = 0;

    test = get_node_at_index(a,start);

    for(;start < a->size; test = test->next)
    {
        if (test->index > max)
        {
            max = test->index;
        }
        start++;
    }

    return max;

}



int get_max(int a, int b) {
    int c = a - b;
    int k = (c >> 31) & 0x1;
    int max = a - k * c;
    return max;
}

int get_min(int a, int b) {
    int c = b - a;
    int k = (c >> 31) & 0x1;
    int min = a + k * c;
    return min;
}


void concatenate_lists(struct Linked_list * a, struct Linked_list * b)
{

    int new_size = a->size + b->size;
    struct node * a_last = a->root->prev;
    struct node * b_last = b->root->prev;

    a->root->prev = b_last;
    a_last->next = b->root;
    b->root->prev = a_last;
    b_last->next = a->root;

    a->size = new_size;

    free(b->root);
    free(b);

    // print_linked_list(a);
}

void insert_node_before(struct node * in_list, struct node * new_node)
{
    new_node->next = in_list;
    new_node->prev = in_list->prev;
    in_list->prev->next = new_node;
    in_list->prev = new_node;
}

void insert_node_after(struct node * in_list, struct node * new_node)
{
    new_node->prev = in_list;
    new_node->next = in_list->next;
    in_list->next->prev = new_node;
    in_list->next = new_node;
}