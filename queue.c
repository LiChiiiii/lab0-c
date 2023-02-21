#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(*head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *itm = NULL, *safe = NULL;
    list_for_each_entry_safe (itm, safe, l, list) {
        q_release_element(itm);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_itm = malloc(sizeof(*new_itm));
    if (!new_itm)
        return false;
    size_t size = strlen(s) + 1;
    new_itm->value = malloc(size * sizeof(char));
    if (!new_itm->value) {
        free(new_itm);
        return false;
    }
    memcpy(new_itm->value, s, size);
    list_add(&new_itm->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_itm = malloc(sizeof(*new_itm));
    if (!new_itm)
        return false;
    size_t size = strlen(s) + 1;
    new_itm->value = malloc(size * sizeof(char));
    if (!new_itm->value) {
        free(new_itm);
        return false;
    }
    memcpy(new_itm->value, s, size);
    list_add_tail(&new_itm->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    element_t *itm = list_first_entry(head, element_t, list);
    list_del(&itm->list);
    if (sp != NULL) {
        strncpy(sp, itm->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return itm;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    element_t *itm = list_last_entry(head, element_t, list);
    list_del(&itm->list);
    if (sp != NULL) {
        strncpy(sp, itm->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return itm;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int size = 0;
    if (!head)
        return size;
    struct list_head *itm;
    list_for_each (itm, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head)
        return false;
    struct list_head *front = head->next, *rear = head->prev;
    while (front != rear->prev && front != rear) {
        front = front->next;
        rear = rear->prev;
    }
    element_t *mid = list_entry(front, element_t, list);
    list_del(front);
    q_release_element(mid);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return false;

    int flag = 0;
    struct list_head *cur, *safe = NULL;
    list_for_each_safe (cur, safe, head) {
        if (cur->prev == head)
            continue;
        element_t *cur_el = list_entry(cur, element_t, list);
        element_t *cur_prev_el = list_entry(cur->prev, element_t, list);
        if (!strcmp(cur_el->value, cur_prev_el->value)) {
            list_del(cur->prev);
            q_release_element(cur_prev_el);
            flag = 1;
        } else if (flag == 1) {
            list_del(cur->prev);
            q_release_element(cur_prev_el);
            flag = 0;
        }
    }

    return true;
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return;
    int flag = 0;
    struct list_head *cur, *safe = NULL;

    list_for_each_safe (cur, safe, head) {
        if (cur->prev == head || flag == 0) {
            flag = 1;
            continue;
        } else if (flag == 1) {
            list_move(cur->prev, cur);
            flag = 0;
        }
    }
    return;
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return;
    struct list_head *cur, *safe = NULL;

    list_for_each_safe (cur, safe, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return;

    int top = 0;
    struct list_head *cur, **cur_prev_k = &(head->next), *safe = NULL,
                           *tmp = NULL;

    list_for_each_safe (cur, safe, head) {
        top++;
        if (top % k == 0) {
            tmp = cur;
            while (*cur_prev_k != tmp) {
                list_move(tmp->prev, cur);
                cur = cur->next;
            }
            cur_prev_k = &(cur->next);
        }
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
