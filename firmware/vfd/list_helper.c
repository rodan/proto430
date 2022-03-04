#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "list_helper.h"

struct list_struct *list_init(struct list_struct **head, struct list_struct **curr, const int val)
{
    struct list_struct *ptr =
        (struct list_struct *)malloc(sizeof(struct list_struct));
    if (NULL == ptr) {
        fprintf(stderr, "list_init malloc() error\n");
        return NULL;
    }
    ptr->val = val;
    ptr->next = NULL;

    *head = *curr = ptr;
    return ptr;
}

struct list_struct *list_add_node(struct list_struct **head, struct list_struct **curr, const int val)
{
    struct list_struct *t1, *t2, *tptr;

    if (NULL == *head) {
        tptr = list_init(&t1, &t2, val);
        *head = tptr;
        *curr = tptr;
        return (tptr);
    }

    struct list_struct *ptr =
        (struct list_struct *)malloc(sizeof(struct list_struct));
    if (NULL == ptr) {
        fprintf(stderr, "list_add_node malloc() error\n");
        return NULL;
    }
    ptr->val = val;
    ptr->next = NULL;

    (*curr)->next = ptr;
    *curr = ptr;

    return ptr;
}

struct list_struct *list_search(struct list_struct *head, int val, struct list_struct **prev)
{
    struct list_struct *ptr = head;
    struct list_struct *tmp = NULL;
    uint8_t found = 0;

    while (ptr != NULL) {
        if (ptr->val == val) {
            found = 1;
            break;
        } else {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if (found == 1) {
        if (prev) {
            *prev = tmp;
        }
        return ptr;
    } else {
        return NULL;
    }
}

int list_get_head(struct list_struct **head, int *val)
{
    struct list_struct *del = NULL;

    if (NULL == *head) {
        return EXIT_FAILURE;
    }

    del = *head;

    *val = (*head)->val;
    *head = del->next;
    free(del);

    return EXIT_SUCCESS;
}

int list_non_empty(struct list_struct *head)
{
    if (head == NULL) {
        return 0;
    }
    
    return 1;
}

int list_free(struct list_struct **head)
{
    struct list_struct *del = NULL;

    while ((del = *head) != NULL) {
        *head = del->next;
        free(del);
    }

    return EXIT_SUCCESS;
}

int list_rm_node(struct list_struct **head, struct list_struct **curr, const int val)
{
    struct list_struct *prev = NULL;
    struct list_struct *del = NULL;

    del = list_search(*head, val, &prev);
    if (del == NULL) {
        return EXIT_FAILURE;
    } else {
        if (prev != NULL)
            prev->next = del->next;

        if (del == *curr) {
            *curr = prev;
        } else if (del == *head) {
            *head = del->next;
        }
    }

    free(del);

    return EXIT_SUCCESS;
}

void list_print(struct list_struct *head)
{
    struct list_struct *ptr = head;

    while (ptr != NULL) {
        printf("%d ", ptr->val);
        ptr = ptr->next;
    }
    printf("\n");

    return;
}

int list_len(struct list_struct *head)
{
    int ret = 0;
    struct list_struct *ptr = head;

    while (ptr != NULL) {
        ret ++;
        ptr = ptr->next;
    }

    return ret;
}

