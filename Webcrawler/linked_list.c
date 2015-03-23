#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>




void copy_and_append(struct Linked_list * from, struct Linked_list * to)
{
    struct node * temp = from->root;

    do
    {
        add_node(to);
        ((to->root)->previous)->data = temp->data;
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
        tempb->data = tempa->data;
        tempa = tempa->next;
        tempb = tempb->next;
    }while(tempa != a->root);
}

void add_node(struct Linked_list * a)
{
    struct node * new_node;
    new_node = (struct node *)malloc(sizeof(struct node));
    new_node->previous = (a->root)->previous;
    new_node->next = a->root;
    ((a->root)->previous)->next = new_node;
    (a->root)->previous = new_node;
    a->size += 1;
}

void add_node_location_data(struct Linked_list* a, int location, int value)
{
    add_node(a);
    ((a->root)->previous)->data = value;

    move_from_to_index(a, (int)(a->size - 1), location);
}


void remove_node(struct Linked_list * a, struct node * to_remove)
{
    if(a->root == to_remove)
    {
        a->root = to_remove->next;
    }

    (to_remove->next)->previous = to_remove->previous;
    (to_remove->previous)->next = to_remove->next;

    free(to_remove->next);
    free(to_remove->previous);
    a->size -= 1;

}

int search_for_num(struct Linked_list * a, int value)
{
    struct node * test = a->root;

    int result = 0;

    do
    {
        if (test->data == value)
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

    a->size = 1;

    a->root = (struct node*)malloc(sizeof(struct node));

    a->root->next = a->root;
    a->root->previous = a->root;

    for(i = 1; i < size; i++)
    {   
        add_node(a);
    }

    return a;
}

void randomize_linked_list(struct Linked_list * a, int max)
{
    struct node * temp;

    temp = a->root;

    do
    {   
        temp->data = rand() % max;
        temp = temp->next;
    }while(temp != a->root);
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
            a->root = (a->root)->previous;
        }
        else
        {

            to_move = get_node_at_index(a,from_index);

            old = get_node_at_index(a,to_index);

            (to_move->previous)->next = to_move->next;
            (to_move->next)->previous = to_move->previous;

            if(to_index < from_index)
            {
                to_move->next = old;
                to_move->previous = old->previous;
                (old->previous)->next = to_move;
                old->previous = to_move;

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
                to_move->previous = old;
                (old->next)->previous = to_move;
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
            // printf("here\n");
            temp1 = temp2;
        }while(temp1 != a->root);
    }

    free(a->root);

    // if(temp == temp->next)
    // {
    //  printf("here1\n");
    //  free(a->root);
    // }
    // else
    // {
    //  printf("here2\n");
    //  a->root = temp->next;
    //  (temp->previous)->next = temp->next;
    //  (temp->next)->previous = temp->previous;
    //  free(temp);
    //  delete_linked_list(a);
    // }
}

void print_linked_list(struct Linked_list * a)
{
    int i = 0;

    struct node * temp;

    temp = a->root;

    do
    {
        printf("node number %d data = %d\n", i++,temp->data );
        temp = temp->next;
    }while(temp != a->root);
}

int get_array_max(struct Linked_list * a, int start)
{
    struct node * test;
    int max = 0;

    test = get_node_at_index(a,start);

    for(;start < a->size; test = test->next)
    {
        if (test->data > max)
        {
            max = test->data;
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


