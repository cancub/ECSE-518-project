#include <stdlib.h>
#include <stdio.h>
// #include <curl/curl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "linked_list.h"
// #include <cblas.h>

// --------------------------------------adding a new node into a linked list after another node-----------------------------------------------------------

#define SIZE        10

char * switch_order(char * url)
{

    char ** pieces = (char **)malloc(sizeof(char *)*3);
    char * stops = (char *)malloc(sizeof(char)*3);

    // http://(www.google.com) or http://(google.com)
    char * beginning = strstr(url, "//");
    char * middle, * end, *path, * newurl, * testurl;

    testurl = NULL;

    if(beginning == NULL)
    {
        // if there isn't http(s):// for some reason
        free(pieces);
        free(stops);
        if(testurl != NULL)
            free(testurl);
        return NULL;
    }
    else
    {
        beginning = &(beginning[2]);
    }

    // printf("%s %s %s\n",beginning,(strchr(strchr(beginning,'.')+1,'.')),strchr(beginning,'/'));

    if( (strchr(strchr(beginning,'.')+1,'.') == NULL) || 
        ((strchr(strchr(beginning,'.')+1,'.') > strchr(beginning,'/')) &&
        (strchr(strchr(beginning,'.')+1,'.') != NULL) && (strchr(beginning,'/') != NULL)))
    {
        // printf("here1\n");
        // we have a link with only one period
        // add a www to the beginning of beginning
        testurl = (char *)malloc(strlen("www.") + strlen(beginning) + 1);     
        testurl[4 + strlen(beginning)] = '\0';

        strcpy(testurl,"www.");
        strcpy(&(testurl[4]),beginning);
        testurl[4 /* "www." */ + strlen(beginning)] = '\0';
        beginning = testurl;
    }

    int final_length = (int)(strlen(beginning)) +1;

    // find the next location for the slash, as this seperates the part
    // of the link we want to switch from the rest we want to leave alone

    // www.google.com(/asndalkndasa) or google.com(NULL)
    char * seperator = strchr(beginning,'/');

    // this will be our first split in the code: either there is a / after the main link or 
    // we just have the basic link
    if(seperator == NULL)
    {
        // if the slash does not exist (e.g http://google.com or http://www.google.com )
        seperator = "\0";

        // www.(google.com) or google.(com)
        middle = strchr(beginning,'.') + 1;

        if (middle == NULL)
        {
            // there were no periods in the link, WTF?
            free(pieces);
            free(stops);
            if(testurl != NULL)
                free(testurl);
            return NULL;
        }

        // www.google.(com) or google.com(NULL)
        end  = strchr(middle,'.') + 1;

        if(end == NULL)
        {
            // there was no second period, so we have to re-evaluate what we think of
            // as beginning, middle and end
            end = middle;   //(com)
            middle = beginning; //(google.com)
            beginning = "www.";            
            final_length += 4;

        }
    }
    else
    {
        //we do have a character that separates the main link from the path

        // www.(google.com/sadasdsa) or www.(google.com/) or google.(com/asda.ssa) or google.(com/)
        middle = strchr(beginning,'.') + 1;

        if (middle == NULL)
        {
            // there were no periods in the link, WTF?
            free(pieces);
            free(stops);
            if(testurl != NULL)
                free(testurl);
            return NULL;
        }

        // www.google.(com/sadasdsa) or www.google.(com/) or google.com/(NULL) or google.com/adasds.(asdads)
        end  = strchr(middle,'.') + 1;

        if((end == NULL) || ((int)(end-seperator) > 0))
        {
            // there was no second period or it's a vestigal period in the path
            // so we have to re-evaluate what we think of as beginning, middle and end
            end = middle;   //(com/asdadasd) or (com/asdsa.asdasd) or (com/)
            middle = beginning; //(google.com/asdsadsa.asdsda)
            beginning = "www."; 
            final_length += 4;

        }


        seperator = "/";

    }

    // for strange links like www.alumni.mcgill.ca with a useless (www.)
    if( (strchr(end,'/') - strchr(end,'.') > 0) && (strchr(end,'.') != NULL) && 
        (strstr(beginning,"www") == beginning))
    {
        // bump everything up one
        beginning = middle;         //(www.alumni.mcgill.ca) --> www.(alumni.mcgill.ca)
        middle = end;               //www.(alumni.mcgill.ca) --> www.alumni.(mcgill.ca)
        end = strchr(end,'.') + 1;  //www.alumni.(mcgill.ca) --> www.alumni.mcgill.(ca)
    }


    newurl = (char *)malloc(final_length);

    // set up both the pieces of the url we will look at and the characters that we will look for
    // in order to shift to the next pointer
    pieces[0] = end;            //(com)
    pieces[1] = middle;         //(google.com)
    pieces[2] = beginning;      //(www.)

    // printf("\n%s\n",end );
    // printf("%s\n", middle);
    // printf("%s\n\n",beginning );
    //     sleep(1);
    // }



    stops[0] = seperator[0];
    stops[1] = '.';
    stops[2] = '.';

    int pieces_stops_count = 0;
    int url_count = 0;
    int i;

    do
    {
        //step through each of the pieces
        i = 0;

        // add each of the characters from this pieces until we find a character that matches
        // the stopping character for this piece
        while(pieces[pieces_stops_count][i] != stops[pieces_stops_count])
        {
            newurl[url_count++] = pieces[pieces_stops_count][i++];
            // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
        }

        // printf("\n");

        if(pieces_stops_count < 2)
        {
            newurl[url_count++] = '.';
            // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
        }

        // move to the next piece
        pieces_stops_count++;

    }while(pieces_stops_count < 3);


    // printf("\n");

    if(stops[0] == '/')
    {
        // http://(www.google.com(/jasdkadksa)) or http://(google.com/(asdsadas))
        path = strstr(url, "//") + 2;
        path = strchr(path,'/');


        i = 0;

        // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
        do
        {
            // copy the remainder of the path to the new url
            // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
            newurl[url_count++] = path[i++];
        }while(path[i] != '\0');
    } 

    // copy the terminator
    newurl[url_count] = '\0';

    //clean up;
    free(pieces);
    free(stops);
    if(testurl != NULL)
        free(testurl);

    // for(i = 0; i < strlen(newurl); i++)
    // {
    //     if( (newurl[i] > '~') || (newurl[i] < '!') )
    //     {
    //         // printf("Error with \"%s\"\nOffending character: newurl[%d] = %c, length = %d\n",newurl, i,newurl[i], (int)(strlen(newurl)) );
    //         return NULL;
    //     }
    // }

    return newurl;
}

int first_after_second(char * a, char * b)
{
    // returns 1 if "a" lexicographically comes after "b",
    //  0 if "b" comes after "a" and 3 if they are the same
    // string
    if(a[0] == '\0' && b[0] != '\0')
    {
        return 0;
    }
    else if(b[0] == '\0' && a[0] != '\0')
    {
        return 1;
    }
    else if(b[0] == '\0' && a[0] == '\0')
    {
        return 3;
    }
    else
    {
        if(a[0] > b[0])
        {
            return 1;
        }
        else if(a[0] < b[0])
        {
            return 0;
        }
        else
        {
            return first_after_second(&(a[1]),&(b[1]));
        }
    }
}

void add_link_string(struct node * n, char * link)
{
    n->hyperlink = (char*)malloc(strlen(link)+1);
    strcpy(n->hyperlink, link);
    n->filtered_hyperlink = switch_order(link);
}

void remove_links(struct Linked_list *a)
{
    struct node * temp = a->root;

    do
    {
        free(temp->hyperlink);
        free(temp->filtered_hyperlink);
        to_next(&temp);
    } while (temp != a->root);
}

void free_LL(struct Linked_list * a)
{
    remove_links(a);
    delete_linked_list(a);
    free(a);
}

struct node * add_sorted_link(struct Linked_list * a, char * str)
{
    // struct node * temp = a->root;   
    // int test;   // to test the order

    if(a->root == NULL)
    {
        //this is the first link that we are inputing into
        // the linked list so we don't need to care about anything really
        // just put it at the front and don't worry about sorting
        add_node(a);
        a->root->index = 1;
        add_link_string(a->root,str);
        return a->root;
    }
    else
    {
        struct node * new_node = (struct node*)malloc(sizeof(struct node));   // this will hold all the link's node info
        struct node * temp = a->root; // to loop through the list
        add_link_string(new_node,str);
        int test;
        // there are other nodes in the list, so we seek out this link's place in the list
        do
        {   
            test = first_after_second(temp->filtered_hyperlink,new_node->filtered_hyperlink);
            if(test == 1)
            {   
                //we've determined that the str, lexigraphically, comes before temp.
                // thus we insert it before temp
                new_node->index = temp->index;
                if(new_node->index == 1)
                {
                    // must move the root if this is the first link
                    a->root = new_node;
                }

                // push the node to the proper location (just before the last node we tested)
                insert_node_before(temp,new_node);
                a->size += 1;

                if (temp == a->root)
                {
                    a->root = new_node;
                }

                do
                {
                    // incriment all the indices of the links after this node
                    temp->index += 1;
                    to_next(&temp);
                } while (temp != a->root);

                //we're done
                return new_node;
            }
            else if (test == 3)
            {
                // the links appear to be the same, but it might just be the filtered hyperlinks,
                // implying that one is https and the other http
                if(strstr(temp->hyperlink,"https")!= NULL && strstr(new_node->hyperlink,"https") == NULL)
                {
                    //we've determined that the str, lexigraphically, comes before temp.
                    // thus we insert it before temp
                    new_node->index = temp->index;
                    if(new_node->index == 1)
                    {
                        // must move the root if this is the first link
                        a->root = new_node;
                    }

                    // push the node to the proper location (just before the last node we tested)
                    insert_node_before(temp,new_node);
                    a->size += 1;

                    if (temp == a->root)
                    {
                        a->root = new_node;
                    }

                    do
                    {
                        // incriment all the indices of the links after this node
                        temp->index += 1;
                        to_next(&temp);
                    } while (temp != a->root);

                    //we're done
                    return new_node;
                } 
                else if (strstr(temp->hyperlink,"https") == NULL && strstr(new_node->hyperlink,"https") != NULL)
                {
                    // it's the opposite, with the new link being the secure link and thus coming after the
                    // link already in the list
                    new_node->index = temp->index + 1;

                    // push the node to the proper location (just after the last node we tested)
                    insert_node_after(temp,new_node);
                    a->size += 1;

                    to_next(&temp);
                    while (temp != a->root)
                    {
                        // incriment all the indices of the links after this node
                        temp->index += 1;
                        to_next(&temp);
                    }

                    //we're done
                    return new_node;
                } 
                else
                {
                    // we should not have made it this far if the link already exists in the list
                    // therefor there has been an error in a previous function
                    printf("----------------------------\n");
                    printf("Error: repeated link %s from %s\n",new_node->filtered_hyperlink, new_node->hyperlink);
                    printf("is the same as %d: %s from %s\n",(int)(temp->index),temp->filtered_hyperlink, 
                        temp->hyperlink);
                    printf("input link is \"%s\"\n",str );
                    printf("---------------------------\n");
                    free(new_node->hyperlink);
                    free(new_node->filtered_hyperlink);
                    free(new_node);
                    return NULL;
                    // exit(0);
                }
            }
            else
            {
                // the link in temp comes before the link under test, str
                // move to the next link in the list
                to_next(&temp);
            }
        } while (temp != a->root);

        if(temp == a->root)
        {
            // we've made it to the point where we know that this new link is higher, lexigraphically, than all the other links
            // so we add it to the end
            new_node->index = a->root->prev->index + 1;
            append_node(a,new_node);
            return new_node;
        }
    }
}


int main()
{
    int LL_size = 4000;
    struct Linked_list * LL = initialize_linked_list(0);
    FILE * ifp = fopen("filtered_links.txt","r");
    char fgets_str[2500];
    char * test;
    if(ifp == NULL)
    {
        printf("could not open file\n");
        return(-1);
    }
    int i = 0;

    do
    {  
        test = fgets(fgets_str,2500,ifp);
        if(test != NULL)
        {
            fgets_str[strlen(fgets_str)-1] = '\0';
            // printf("\n\nadding %s\n",fgets_str );
            add_sorted_link(LL,fgets_str);
            LL_size--;
        }
        else
            break;
    } while (LL_size > 0);


    // struct node * temp = LL->root;
    // do
    // {   
    //     printf("index is %d, link is %s\n",temp->index, temp->filtered_hyperlink );
    //     to_next(&temp);
    // } while (temp!=LL->root);

    free_LL(LL);
    fclose(ifp);

    return 0;
}


//----------------------------------setting indices based on pointers-----------------------------------

// struct node
// {
//  int index;
//  struct node * prev;
//  struct node * next;
//  char * link;
//  int ** edges_to;
// };

// struct nodeArray
// {
//  struct node * root;
//  size_t size;
// };


// int main()
// {
//     int i,j, size = 5;   // number of links
//     FILE * ifp = fopen("filtered_links.txt","r");        // file from which we obtain the links
//     struct node * firstnode = (struct node*)malloc(sizeof(struct node));    
//     firstnode->next = NULL;
//     firstnode->prev = NULL;
//     char what[] = "test";
//     // fgets(firstnode->link,2000,ifp);
//     // printf("here\n");
//     firstnode->index = 1;
//     firstnode->edges_to = (int **)malloc(sizeof(int*));
//     firstnode->edges_to[0] = &(firstnode->index);

//     struct node * secondnode = (struct node*)malloc(sizeof(struct node));
//     secondnode->prev = firstnode;

//     secondnode->next = NULL;
//     secondnode->index = 2;
//     secondnode->edges_to = (int **)malloc(sizeof(int*) *2);
//     secondnode->edges_to[0] = &(firstnode->index);
//     secondnode->edges_to[1] = &(secondnode->index);
    

//  printf("Link %d points to links: ", firstnode->index);
//  for(j = 0; j < firstnode->index; j++)
//  {
//      printf("%d ", *(firstnode->edges_to[j]));
//  }
//  printf("\n");
//  printf("Link %d points to links: ", secondnode->index);
//  for(j = 0; j < secondnode->index; j++)
//  {
//      printf("%d ", *(secondnode->edges_to[j]));
//  }

//     firstnode->index = 25;
//     secondnode->index = 10;
//     printf("\n");

//     printf("Link %d points to links: ", firstnode->index);
//  for(j = 0; j < 1; j++)
//  {
//      printf("%d ", *(firstnode->edges_to[j]));
//  }
//  // printf("%d ", *(firstnode->edges_to[0]));
//  printf("\n");

//  printf("Link %d points to links: ", secondnode->index);
//  for(j = 0; j < 2; j++)
//  {
//      printf("%d ", *(secondnode->edges_to[j]));
//  }
//  printf("\n");

//     // printf("here\n");
//     free(secondnode->edges_to);
//     free(secondnode);


//     // i = 1;
//     // do
//     // {

//     //   firstnode->next = (struct node*)malloc(sizeof(struct node*));
//     //   firstnode->next->next = NULL;
//     //   firstnode->next->prev = firstnode->next;
//     //     indices[i] = i+1;
//     //     i++;
//     // }while(i < size);

    

//     // for (i = 0; i< size; i++)
//     // {
//     //     strcpy(string_array[i],"testx");
//     //     string_array[i][4] = '0' + i;
//     //     indices[i] = i;

//     //     edges[i] = (int **)malloc(sizeof(int*) *(i+1));
//     //     for(j = 0; j<(i+1);j++ )
//     //     {
//     //         edges[i][j] = &(indices[i]);
//     //     }
//     //     // usleep(100000);
//     //     // srand(time(NULL));
//     // }

//     // // for(i = 0; i < size; i++)
//     // // {
//     // //     // string_array[i] = test;
//     // //     (string_array[i])[4] = '1';
//     // //     // printf("%s\n\n",string_array[i] );
//     // // }

//     // for(i = 0; i < size; i++)
//     // {

//     //     printf("%s\n",string_array[i] );
//     // }

//     free(firstnode->edges_to);
//     free(firstnode);
//     // struct node * temp1 = link_list->root->next;
//     // struct node * temp2;
//     // do
//     // {   

//     //     temp1 = temp1->next;
//     //     free(temp1->previous.edges_to)
//     //     free(firstnode->previous1);
//         // printf("here\n");
//     //     temp1 = temp2;
//     // }while(temp1->next != NULL);

//     // free(a->root);

    
//     fclose(ifp);
// }


//----------------------------------------------switching url order----------------------------------------------------------


// char * switch_order(char * url)
// {

//     char ** pieces = (char **)malloc(sizeof(char *)*3);
//     char * stops = (char *)malloc(sizeof(char)*3);

//     // http://(www.google.com) or http://(google.com)
//     char * beginning = strstr(url, "//");
//     char * middle, * end, *path, * newurl, * testurl;

//     testurl = NULL;

//     if(beginning == NULL)
//     {
//         // if there isn't http(s):// for some reason
//         free(pieces);
//         free(stops);
//         if(testurl != NULL)
//             free(testurl);
//         return NULL;
//     }
//     else
//     {
//         beginning = &(beginning[2]);
//     }

//     // printf("%s %s %s\n",beginning,(strchr(strchr(beginning,'.')+1,'.')),strchr(beginning,'/'));

//     if( (strchr(strchr(beginning,'.')+1,'.') == NULL) || 
//         ((strchr(strchr(beginning,'.')+1,'.') > strchr(beginning,'/')) &&
//         (strchr(strchr(beginning,'.')+1,'.') != NULL) && (strchr(beginning,'/') != NULL)))
//     {
//         // printf("here1\n");
//         // we have a link with only one period
//         // add a www to the beginning of beginning
//         testurl = (char *)malloc(strlen("www.") + strlen(beginning) + 1);     
//         testurl[4 + strlen(beginning)] = '\0';

//         strcpy(testurl,"www.");
//         strcpy(&(testurl[4]),beginning);
//         testurl[4 /* "www." */ + strlen(beginning)] = '\0';
//         beginning = testurl;
//     }

//     int final_length = (int)(strlen(beginning)) +1;

//     // find the next location for the slash, as this seperates the part
//     // of the link we want to switch from the rest we want to leave alone

//     // www.google.com(/asndalkndasa) or google.com(NULL)
//     char * seperator = strchr(beginning,'/');

//     // this will be our first split in the code: either there is a / after the main link or 
//     // we just have the basic link
//     if(seperator == NULL)
//     {
//         // if the slash does not exist (e.g http://google.com or http://www.google.com )
//         seperator = "\0";

//         // www.(google.com) or google.(com)
//         middle = strchr(beginning,'.') + 1;

//         if (middle == NULL)
//         {
//             // there were no periods in the link, WTF?
//             free(pieces);
//             free(stops);
//             if(testurl != NULL)
//                 free(testurl);
//             return NULL;
//         }

//         // www.google.(com) or google.com(NULL)
//         end  = strchr(middle,'.') + 1;

//         if(end == NULL)
//         {
//             // there was no second period, so we have to re-evaluate what we think of
//             // as beginning, middle and end
//             end = middle;   //(com)
//             middle = beginning; //(google.com)
//             beginning = "www.";            
//             final_length += 4;

//         }
//     }
//     else
//     {
//         //we do have a character that separates the main link from the path

//         // www.(google.com/sadasdsa) or www.(google.com/) or google.(com/asda.ssa) or google.(com/)
//         middle = strchr(beginning,'.') + 1;

//         if (middle == NULL)
//         {
//             // there were no periods in the link, WTF?
//             free(pieces);
//             free(stops);
//             if(testurl != NULL)
//                 free(testurl);
//             return NULL;
//         }

//         // www.google.(com/sadasdsa) or www.google.(com/) or google.com/(NULL) or google.com/adasds.(asdads)
//         end  = strchr(middle,'.') + 1;

//         if((end == NULL) || ((int)(end-seperator) > 0))
//         {
//             // there was no second period or it's a vestigal period in the path
//             // so we have to re-evaluate what we think of as beginning, middle and end
//             end = middle;   //(com/asdadasd) or (com/asdsa.asdasd) or (com/)
//             middle = beginning; //(google.com/asdsadsa.asdsda)
//             beginning = "www."; 
//             final_length += 4;

//         }


//         seperator = "/";

//     }

//     // for strange links like www.alumni.mcgill.ca with a useless (www.)
//     if( (strchr(end,'/') - strchr(end,'.') > 0) && (strchr(end,'.') != NULL))
//     {
//         // bump everything up one
//         beginning = middle;         //(www.alumni.mcgill.ca) --> www.(alumni.mcgill.ca)
//         middle = end;               //www.(alumni.mcgill.ca) --> www.alumni.(mcgill.ca)
//         end = strchr(end,'.') + 1;  //www.alumni.(mcgill.ca) --> www.alumni.mcgill.(ca)
//     }


//     newurl = (char *)malloc(final_length);

//     // set up both the pieces of the url we will look at and the characters that we will look for
//     // in order to shift to the next pointer
//     pieces[0] = end;            //(com)
//     pieces[1] = middle;         //(google.com)
//     pieces[2] = beginning;      //(www.)

//     // printf("\n%s\n",end );
//     // printf("%s\n", middle);
//     // printf("%s\n\n",beginning );
//     //     sleep(1);
//     // }



//     stops[0] = seperator[0];
//     stops[1] = '.';
//     stops[2] = '.';

//     int pieces_stops_count = 0;
//     int url_count = 0;
//     int i;

//     do
//     {
//         //step through each of the pieces
//         i = 0;

//         // add each of the characters from this pieces until we find a character that matches
//         // the stopping character for this piece
//         while(pieces[pieces_stops_count][i] != stops[pieces_stops_count])
//         {
//             newurl[url_count++] = pieces[pieces_stops_count][i++];
//             // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
//         }

//         // printf("\n");

//         if(pieces_stops_count < 2)
//         {
//             newurl[url_count++] = '.';
//             // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
//         }

//         // move to the next piece
//         pieces_stops_count++;

//     }while(pieces_stops_count < 3);


//     // printf("\n");

//     if(stops[0] == '/')
//     {
//         // http://(www.google.com(/jasdkadksa)) or http://(google.com/(asdsadas))
//         path = strstr(url, "//") + 2;
//         path = strchr(path,'/');


//         i = 0;

//         // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
//         do
//         {
//             // copy the remainder of the path to the new url
//             // printf("newurl = %s, sizeof(newurl) = %d\n",newurl, (int)(strlen(newurl)) );
//             newurl[url_count++] = path[i++];
//         }while(path[i] != '\0');
//     } 

//     // copy the terminator
//     newurl[url_count] = '\0';

//     //clean up;
//     free(pieces);
//     free(stops);
//     if(testurl != NULL)
//         free(testurl);

//     // for(i = 0; i < strlen(newurl); i++)
//     // {
//     //     if( (newurl[i] > '~') || (newurl[i] < '!') )
//     //     {
//     //         // printf("Error with \"%s\"\nOffending character: newurl[%d] = %c, length = %d\n",newurl, i,newurl[i], (int)(strlen(newurl)) );
//     //         return NULL;
//     //     }
//     // }

//     return newurl;
// }


// int main()
// {
//  char url[2500];
//     char * newurl;

//     // FILE * ifp = fopen("filtered_links.txt", "r");
//     // if(ifp == NULL)
//     // {
//     //     printf("error opening file\n");
//     //     return(-1);
//     // }

//     // while(fgets(url,2500,ifp) != NULL)
//     // {
//     //     if (url[(int)(strlen(url))-1] == '\n')
//     //         url[(int)(strlen(url))-1] = '\0';

//     //     // printf("before = \t\"%s\"\n", url);
//     //     newurl = switch_order(url);
//     //     if(newurl == NULL)
//     //     {
//     //         printf("Invalid url = %s\n", url);
//     //     }
//     //     else
//     //     {
//     //         free(newurl);
//     //         // printf("result = \t\"%s\"\n", newurl);
//     //     }
//     //  //    usleep(100000);
//     // }
//     // fclose(ifp);

//     newurl = switch_order("http://humanitarianstudiesinitiative.org");
//     printf("newurl = %s\n", newurl);
//     free(newurl);
// }


// ------------------------------------------------------------------------------------------
// struct DubArray
// {
//     double * array;
//     size_t size;
// };

// void mtranspose(double ** A, int n)
// {
//     // A is the n by n matrix we will transpose

//     double temp;    // hold the intermediate value as we switch the values in the elements

//     int i,j;

//     for(i = 0; i < n-1; i++)
//     {
//         for(j = i+1; j < n; j++)
//         {
//             temp = (*A)[i*n+j];
//             (*A)[i*n+j] = (*A)[j*n+i];
//             (*A)[j*n+i] = temp;
//         }
//     }

// }

// int main()
// {
//     int row_size = 30;
//     int matrix_elements = row_size*row_size;
//     double * a = (double *)calloc(matrix_elements,sizeof(double));
//     double *zeros = (double *)calloc(row_size,sizeof(double));
//     int * bitmap = (int *)calloc(row_size,sizeof(int));
//     int i, r = 0;
//     srand(time(NULL));

//     for(i = 0; i < rand()%row_size; i++)
//     {
        
//         do
//         {
//             r += rand()%50;
//             r = r % row_size;
//             // printf("bitmap[%d] = %d\n",r, bitmap[r]);
//             srand(time(NULL));
//         }while(bitmap[r] != 0);

//         bitmap[r] = 1;
//         // printf("zeroing %dth row\n",r );
//     }

//     for (i = 0;i < matrix_elements; i++)
//     {
//         a[i] = i;
//         printf("%4.0f ",a[i]);
//         if(i%row_size == row_size-1)
//         {
//             printf("\n");
//         }
//     }
//     printf("\n\n");

//     int j;

//     double * to_zero;
//     for(i = 0; i < row_size; i++)
//     {
//         if (bitmap[i])
//         {
//             cblas_dcopy(row_size,zeros,1,&(a[i*row_size]),1);
//         }
//     }

//     for(i = 0; i < matrix_elements; i++)
//     {
//         printf("%4.0f ",a[i]);
//         if(i%row_size == row_size-1)
//         {
//             printf("\n");
//         }
//     }

//     printf("\n\n");

//     printf("starting transpose\n");
//     mtranspose(&a,row_size);

//     for(i = 0; i < matrix_elements; i++)
//     {
//         printf("%4.0f ",a[i]);
//         if(i%row_size == row_size-1)
//         {
//             printf("\n");
//         }
//     }

//     printf("\n");


//     free(a);
//     free(bitmap);
//     free(zeros);
// }

/*------------------------------------------------------------------------------------------------------------------*/

// struct DubArray
// {
//     double * array;
//     size_t size;
// };

// int * taken;

// void print_dub(struct DubArray * a)
// {
//     int i;
//     for(i = 0; i < a->size; i++)
//     {
//         printf("%6.2f ", a->array[i]);
//     }
//     printf("\n");
// }

// void add_random_at_random(struct DubArray * a)
// {
//     int index;
//     double r = (double)(rand()%50)/1.2;
//     do
//     {
//         index = rand() % 20;
//     }while(taken[index]);

//     a->array[index] = r;
//     taken[index] = 1;
// }

// int main()
// {
//     srand(time(NULL));
//     struct DubArray test;
//     test.array = (double *)calloc(20,sizeof(double));
//     taken = (int *)calloc(20,sizeof(int));
//     test.size = 20;
//     int i = 0;
//     for(; i< 20; i++)
//     {
//         add_random_at_random(&test);
//         print_dub(&test);
//         printf("\n");
//     }

//     free(taken);
//     free(test.array);


//     return 0;
// }

/*------------------------------------------------------------------------------------------------------------------*/
// int main()
// {
//  CURL *curl;
//     CURLcode curl_res;

//     curl_global_init(CURL_GLOBAL_ALL);

//     curl = curl_easy_init();

//     if (curl)
//     {
//         curl_easy_setopt(curl, CURLOPT_URL, "http://bit.ly/1zKaphR");
//         curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
//      curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
//         // curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)"); 

//         /* Perform the request, curl_res will get the return code */ 
//         curl_res = curl_easy_perform(curl);
//         // printf("here\n");

//         /* Check for errors */ 
//         if(curl_res != CURLE_OK)
//           fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                   curl_easy_strerror(curl_res));

//         if(CURLE_OK == curl_res) 
//         {
//              char *url;
//              curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
//              long code;

//              curl_res = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&code);
//              if((CURLE_OK == curl_res))
//                  printf("CURLINFO_RESPONSE_CODE: .%d.\n", (int)code);

//              if((CURLE_OK == curl_res) && url)
//                  printf("CURLINFO_EFFECTIVE_URL: .%s.\n", url);


//         }

//         /* always cleanup */ 
//         curl_easy_cleanup(curl);

//         /* we're done with libcurl, so clean it up */ 
//         curl_global_cleanup();

//     }
//     else
//     {
//         printf("cURL error.\n");
//     }

//     return 0;

// }