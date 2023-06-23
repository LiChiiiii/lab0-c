#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    if (!head || list_empty(head))
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
    if (!head || list_empty(head))
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
    if (!head || list_empty(head))
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
    if (!head || list_empty(head))
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
            if (cur == head->prev) {
                list_del(cur);
                q_release_element(cur_el);
                return true;
            }
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

/* q_Sort */
/* Merge two list into one queue */
struct list_head *merge_two_list(struct list_head *l1, struct list_head *l2)
{
    struct list_head *temp = NULL;
    struct list_head **indirect = &temp;
    for (struct list_head **node = NULL; l1 && l2; *node = (*node)->next) {
        element_t *e1 = list_entry(l1, element_t, list);
        element_t *e2 = list_entry(l2, element_t, list);
        if (strcmp(e1->value, e2->value) < 0)
            node = &l1;
        else
            node = &l2;
        *indirect = *node;
        indirect = &(*indirect)->next;
    }
    *indirect = (struct list_head *) ((u_int64_t) l1 | (u_int64_t) l2);
    return temp;
}

/* Divide the queue and sort every element */
struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast = head, *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    fast = slow;
    slow->prev->next = NULL;
    struct list_head *l1 = mergesort(head);
    struct list_head *l2 = mergesort(fast);
    return merge_two_list(l1, l2);
}
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort(head->next);
    struct list_head *current = head, *next = head->next;
    while (next) {
        next->prev = current;
        current = next;
        next = next->next;
    }
    current->next = head;
    head->prev = current;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return 0;

    struct list_head *cur = head->prev;

    while (cur->prev != head) {
        element_t *cur_el = list_entry(cur, element_t, list);
        element_t *cur_prev_el = list_entry(cur->prev, element_t, list);
        int cmp = strcmp(cur_el->value, cur_prev_el->value);
        if (cmp > 0) {
            list_del(cur->prev);
            q_release_element(cur_prev_el);
        } else {
            cur = cur->prev;
        }
    }

    return q_size(head);
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return list_entry(head, queue_contex_t, chain)->size;

    queue_contex_t *que = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *cur_que = NULL;
    list_for_each_entry (cur_que, head, chain) {
        if (cur_que == que)
            continue;

        list_splice_init(cur_que->q, que->q);
        que->size = que->size + cur_que->size;
        cur_que->size = 0;
    }

    q_sort(que->q);
    return que->size;

    // https://leetcode.com/problems/merge-k-sorted-lists/
}



/**
 * include treesort.c (red black tree)
 *
 * node_t - Linked list RB node
 * @color: RB node's color
 * @left: pointer to left child node
 * @right: pointer to right child node
 * @list: node of a doubly-linked list
 * @value: pointer to array holding integer

 *
*/
struct rb_node {
    uintptr_t color;
    struct rb_node *left, *right;
    char *value;
} __attribute__((aligned(sizeof(long))));

typedef struct __node {
    struct rb_node RBnode;
    struct list_head *list;
} node_t;

struct cmap_internal {
    struct rb_node *head;

    /* Properties */
    size_t key_size, element_size, size;
    struct rb_node *it_end, *it_most, *it_least;
    int (*comparator)(void *, void *);
};
typedef enum { CMAP_RED = 0, CMAP_BLACK } color_t;

#define rb_parent(r) ((struct rb_node *) (((r)->color & ~7)))
#define rb_color(r) ((color_t) (r)->color & 1)

#define rb_set_parent(r, p)                         \
    do {                                            \
        (r)->color = rb_color(r) | (uintptr_t) (p); \
    } while (0)
#define rb_set_red(r)     \
    do {                  \
        (r)->color &= ~1; \
    } while (0)
#define rb_set_black(r)  \
    do {                 \
        (r)->color |= 1; \
    } while (0)

#define rb_is_red(r) (!rb_color(r))
#define rb_is_black(r) (rb_color(r))

#if defined(__GNUC__) || defined(__clang__)
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

enum { _CMP_LESS = -1, _CMP_EQUAL = 0, _CMP_GREATER = 1 };

/* string comparison */
static inline int cmap_cmp_str(void *arg0, void *arg1)
{
    char *a = (char *) arg0, *b = (char *) arg1;
    int result = strcmp(a, b);
    return result < 0 ? _CMP_LESS : result > 0 ? _CMP_GREATER : _CMP_EQUAL;
}

/* Store access to the head node, as well as the first and last nodes.
 * Keep track of all aspects of the tree. All cmap functions require a pointer
 * to this struct.
 */
typedef struct cmap_internal *cmap_t;

#define cmap_init(key_type, element_type, __func) \
    cmap_new(sizeof(key_type), sizeof(element_type), __func)

/*
 * RB tree implement function
 */
node_t *list_make_node(struct list_head *head);
node_t *cmap_create_node(node_t *node);
void tree_free(struct rb_node *node);
struct rb_node *cmap_rotate_left(cmap_t obj, struct rb_node *node);
struct rb_node *cmap_rotate_right(cmap_t obj, struct rb_node *node);
void cmap_l_l(cmap_t obj,
              struct rb_node *node UNUSED,
              struct rb_node *parent UNUSED,
              struct rb_node *grandparent,
              struct rb_node *uncle UNUSED);
void cmap_l_r(cmap_t obj,
              struct rb_node *node,
              struct rb_node *parent,
              struct rb_node *grandparent,
              struct rb_node *uncle);
void cmap_r_r(cmap_t obj,
              struct rb_node *node UNUSED,
              struct rb_node *parent UNUSED,
              struct rb_node *grandparent,
              struct rb_node *uncle UNUSED);
void cmap_r_l(cmap_t obj,
              struct rb_node *node,
              struct rb_node *parent,
              struct rb_node *grandparent,
              struct rb_node *uncle);
void cmap_fix_colors(cmap_t obj, struct rb_node *node);
void cmap_calibrate(cmap_t obj);
cmap_t cmap_new(size_t s1, size_t s2, int (*cmp)(void *, void *));
bool cmap_insert(cmap_t obj, node_t *node, void *value);
node_t *cmap_first(cmap_t obj);
node_t *cmap_next(node_t *node);
void tree_sort(struct list_head *head);
bool list_is_ordered(node_t *list);
void print_level_order(struct rb_node *root);


/*
 * RB tree implement function
 */
node_t *list_make_node(struct list_head *list)
{
    node_t *node = malloc(sizeof(node_t));
    node->list = list;
    node->RBnode.value =
        ((element_t *) list_entry(list, element_t, list))->value;
    cmap_create_node(node);
    return node;
}

node_t *cmap_create_node(node_t *node)
{
    /* Setup the pointers */
    node->RBnode.left = node->RBnode.right = NULL;
    rb_set_parent(&(node->RBnode), NULL);

    /* Set the color to black by default */
    rb_set_red(&(node->RBnode));

    return node;
}

void tree_free(struct rb_node *node)
{
    if (!node)
        return;

    tree_free(node->left);
    tree_free(node->right);
    free(container_of(node, node_t, RBnode));
}
struct rb_node *cmap_rotate_left(cmap_t obj, struct rb_node *node)
{
    struct rb_node *r = node->right, *rl = r->left, *up = rb_parent(node);

    /* Adjust */
    rb_set_parent(r, up);
    r->left = node;

    node->right = rl;
    rb_set_parent(node, r);

    if (node->right)
        rb_set_parent(node->right, node);

    if (up) {
        if (up->right == node)
            up->right = r;
        else
            up->left = r;
    }

    if (node == obj->head)
        obj->head = r;

    return r;
}
struct rb_node *cmap_rotate_right(cmap_t obj, struct rb_node *node)
{
    struct rb_node *l = node->left, *lr = l->right, *up = rb_parent(node);

    rb_set_parent(l, up);
    l->right = node;

    node->left = lr;
    rb_set_parent(node, l);

    if (node->left)
        rb_set_parent(node->left, node);

    if (up) {
        if (up->right == node)
            up->right = l;
        else
            up->left = l;
    }

    if (node == obj->head)
        obj->head = l;

    return l;
}
void cmap_l_l(cmap_t obj,
              struct rb_node *node UNUSED,
              struct rb_node *parent UNUSED,
              struct rb_node *grandparent,
              struct rb_node *uncle UNUSED)
{
    /* Rotate to the right according to grandparent */
    grandparent = cmap_rotate_right(obj, grandparent);

    /* Swap grandparent and uncle's colors */
    color_t c1 = rb_color(grandparent), c2 = rb_color(grandparent->right);

    if (c1 == CMAP_RED)
        rb_set_red(grandparent->right);
    else
        rb_set_black(grandparent->right);

    if (c2 == CMAP_RED)
        rb_set_red(grandparent);
    else
        rb_set_black(grandparent);
}
void cmap_l_r(cmap_t obj,
              struct rb_node *node,
              struct rb_node *parent,
              struct rb_node *grandparent,
              struct rb_node *uncle)
{
    /* Rotate to the left according to parent */
    parent = cmap_rotate_left(obj, parent);

    /* Refigure out the identity */
    node = parent->left;
    grandparent = rb_parent(parent);
    uncle =
        (grandparent->left == parent) ? grandparent->right : grandparent->left;

    /* Apply left-left case */
    cmap_l_l(obj, node, parent, grandparent, uncle);
}
void cmap_r_r(cmap_t obj,
              struct rb_node *node UNUSED,
              struct rb_node *parent UNUSED,
              struct rb_node *grandparent,
              struct rb_node *uncle UNUSED)
{
    /* Rotate to the left according to grandparent */
    grandparent = cmap_rotate_left(obj, grandparent);

    /* Swap grandparent and uncle's colors */
    color_t c1 = rb_color(grandparent), c2 = rb_color(grandparent->left);

    if (c1 == CMAP_RED)
        rb_set_red(grandparent->left);
    else
        rb_set_black(grandparent->left);

    if (c2 == CMAP_RED)
        rb_set_red(grandparent);
    else
        rb_set_black(grandparent);
}
void cmap_r_l(cmap_t obj,
              struct rb_node *node,
              struct rb_node *parent,
              struct rb_node *grandparent,
              struct rb_node *uncle)
{
    /* Rotate to the right according to parent */
    parent = cmap_rotate_right(obj, parent);

    /* Refigure out the identity */
    node = parent->right;
    grandparent = rb_parent(parent);
    uncle =
        (grandparent->left == parent) ? grandparent->right : grandparent->left;

    /* Apply right-right case */
    cmap_r_r(obj, node, parent, grandparent, uncle);
}
void cmap_fix_colors(cmap_t obj, struct rb_node *node)
{
    /* If root, set the color to black */
    if (node == obj->head) {
        rb_set_black(node);
        return;
    }

    /* If node's parent is black or node is root, back out. */
    if (rb_is_black(rb_parent(node)) && rb_parent(node) != obj->head)
        return;

    /* Find out the identity */
    struct rb_node *parent = rb_parent(node), *grandparent = rb_parent(parent),
                   *uncle;

    if (!rb_parent(parent))
        return;

    /* Find out the uncle */
    if (grandparent->left == parent)
        uncle = grandparent->right;
    else
        uncle = grandparent->left;

    if (uncle && rb_is_red(uncle)) {
        /* If the uncle is red, change color of parent and uncle to black */
        rb_set_black(uncle);
        rb_set_black(parent);

        /* Change color of grandparent to red. */
        rb_set_red(grandparent);

        /* Call this on the grandparent */
        cmap_fix_colors(obj, grandparent);
    } else if (!uncle || rb_is_black(uncle)) {
        /* If the uncle is black. */
        if (parent == grandparent->left && node == parent->left)
            cmap_l_l(obj, node, parent, grandparent, uncle);
        else if (parent == grandparent->left && node == parent->right)
            cmap_l_r(obj, node, parent, grandparent, uncle);
        else if (parent == grandparent->right && node == parent->left)
            cmap_r_l(obj, node, parent, grandparent, uncle);
        else if (parent == grandparent->right && node == parent->right)
            cmap_r_r(obj, node, parent, grandparent, uncle);
    }
}
void cmap_calibrate(cmap_t obj)
{
    if (!obj->head) {
        obj->it_least = obj->it_most = NULL;
        return;
    }

    /* Recompute it_least and it_most */
    obj->it_least = obj->it_most = obj->head;

    while (obj->it_least->left)
        obj->it_least = obj->it_least->left;

    while (obj->it_most->right)
        obj->it_most = obj->it_most->right;
}
cmap_t cmap_new(size_t s1, size_t s2, int (*cmp)(void *, void *))
{
    cmap_t obj = malloc(sizeof(struct cmap_internal));

    obj->head = NULL;

    obj->key_size = s1;
    obj->element_size = s2;
    obj->size = 0;

    obj->comparator = cmp;

    obj->it_end = malloc(sizeof(struct rb_node));
    obj->it_least = malloc(sizeof(struct rb_node));
    obj->it_most = malloc(sizeof(struct rb_node));

    obj->it_end->left = obj->it_end->right = NULL;
    obj->it_least->left = obj->it_least->right = NULL;
    obj->it_most->left = obj->it_most->right = NULL;

    return obj;
}

bool cmap_insert(cmap_t obj, node_t *node, void *value)
{
    cmap_create_node(node);

    obj->size++;

    if (!obj->head) {
        /* Just insert the node in as the new head. */
        obj->head = &node->RBnode;
        rb_set_black(obj->head);

        /* Calibrate the tree to properly assign pointers. */
        cmap_calibrate(obj);
        return true;
    }

    /* Traverse the tree until we hit the end or find a side that is NULL */
    for (struct rb_node *cur = obj->head;;) {
        int res = obj->comparator(node->RBnode.value, cur->value);
        if (!res) /* If the key matches something else, don't insert */
            assert(0 && "not support repetitive value");

        if (res < 0) {
            if (!cur->left) {
                cur->left = &(node->RBnode);
                rb_set_parent(&(node->RBnode), cur);
                cmap_fix_colors(obj, &(node->RBnode));
                break;
            }
            cur = cur->left;
        } else {
            if (!cur->right) {
                cur->right = &(node->RBnode);
                rb_set_parent(&(node->RBnode), cur);
                cmap_fix_colors(obj, &(node->RBnode));
                break;
            }
            cur = cur->right;
        }
    }

    cmap_calibrate(obj);
    return true;
}


// void print_level_order(struct rb_node* root)
// {
//     int h = height(root);
//     for (int i = 1; i <= h; i++)
//         print_current_level(root, i);
//     printf("\n");
// }

node_t *cmap_first(cmap_t obj)
{
    struct rb_node *n = obj->head;
    if (!n)
        return NULL;

    while (n->left)
        n = n->left;
    return (node_t *) container_of(n, node_t, RBnode);
}
node_t *cmap_next(node_t *node)
{
    if (!node)
        return NULL;

    if (node->RBnode.right) {
        node = (node_t *) container_of(node->RBnode.right, node_t, RBnode);
        while (node->RBnode.left)
            node = (node_t *) container_of(node->RBnode.left, node_t, RBnode);
        return node;
    }

    struct rb_node *parent;
    while ((parent = rb_parent(&(node->RBnode))) &&
           &(node->RBnode) == parent->right)
        node = (node_t *) container_of(parent, node_t, RBnode);

    return parent ? (node_t *) container_of(parent, node_t, RBnode) : NULL;
}


void tree_sort(struct list_head *head)
{
    cmap_t map = cmap_new(sizeof(long), sizeof(NULL), cmap_cmp_str);
    struct list_head *list;
    list_for_each (list, head) {
        node_t *node = list_make_node(list);
        cmap_insert(map, node, NULL);
    }

    node_t *node = cmap_first(map);
    if (!node) {
        free(map);
        return;
    }

    for (node_t *next = cmap_next(node); next; next = cmap_next(next)) {
        list_del(next->list);
        list_add(next->list, node->list);
        node = next;
    }

    tree_free(map->head);
    free(map);
}