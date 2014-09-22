/* 
 * File:   main.c
 * Author: des
 *
 * Created on September 15, 2014, 12:41 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

/*
 * 
 */
struct node {
    int a; //structure
    struct node *next;
};

void generate(struct node **, int);
void search(struct node *, int, int);
void delete(struct node **);

int main() {
    struct node *head;
    int key, num;

    printf("Enter the number of nodes: ");
    // read input from stdin, it shall be a decimal integer, assign it to the address of num
    scanf("%d", &num);
    // go to void/function generate
    generate(&head, num);
    printf("\nEnter key to search: ");
    // read input from stdin assign it to the address of key
    scanf("%d", &key);
    // go to void/function search
    search(head, key, num);
    // go to void/function delete
    delete(&head);
}

void generate(struct node **head, int num) {
    int i;
    struct node *temp;
 
    for (i = 0; i < num; i++) {
        //calculates size of new struct node, casts the needed space and assigns the object to temp
        temp = (struct node *)malloc(sizeof(struct node));
        //temp points to structure a, assigns random number smaller then num 
        temp->a = rand() % num;
        printf("%d    ", temp->a);
        //if the object is empty assign temp to it
        if (*head == NULL) {
            *head = temp;
            //make object head part of structure next, and make it NULL 
            (*head)->next = NULL;
        }

        else {
            // if object head is not empty
            //make temp part of the next structure and assign the object head to temp
            temp->next = *head;
            // assign value of temp to object head
            *head = temp;
        }
    }
}

void search(struct node *head, int key, int index) {
    // if head which is part of structure a is equal key print the index
    if (head->a == key) {
        printf("Key found at Position: %d\n", index);
    }
    // if head which is part of structure next is not matching, return it
    if (head->next == NULL) {
        return;
    }
    // recursive search with the next smaller index value
    search(head->next, key, index - 1);

}

void delete(struct node **head) {
    struct node *temp;
    // as long as object head is not found/existing
    while (*head != NULL) {
        // assign object head to temp
        temp = *head;
        // object head, which is part of structure next is assigned to object head
        *head = (*head)->next;
        //deallocate the memory of temp
        free(temp);
    }
}

