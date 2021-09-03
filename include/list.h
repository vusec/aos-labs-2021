#pragma once

#include <types.h>

struct list {
	struct list *next;
	struct list *prev;
};

#define list_foreach(list, node) \
	for (node = (list)->next; node != list; node = node->next)
#define list_foreach_rev(list, node) \
	for (node = (list)->prev; node != list; node = node->prev)
#define list_foreach_safe(list, node, next) \
	for (node = (list)->next, next = node->next; node != list; node = next, \
		next = node->next)
#define list_foreach_safe_rev(list, node, prev) \
	for (node = (list)->prev, prev = node->prev; node != list; node = prev, \
		prev = node->prev)

#define LIST_INIT(name) { &(name), &(name) }

static inline int list_is_empty(struct list *head)
{
	return head->next == head;
}

static inline struct list *list_head(struct list *head)
{
	return (head->next == head) ? NULL : head->next;
}

static inline struct list *list_tail(struct list *head)
{
	return (head->prev == head) ? NULL : head->prev;
}

static inline struct list *list_next(struct list *head, struct list *node)
{
	return (node->next == head) ? NULL : node->next;
}

static inline struct list *list_prev(struct list *head, struct list *node)
{
	return (node->prev == head) ? NULL : node->prev;
}

void list_init(struct list *head);
void list_insert_after(struct list *node, struct list *new);
void list_insert_before(struct list *node, struct list *new);
void list_del(struct list *node);
void list_add(struct list *head, struct list *new);
void list_add_tail(struct list *head, struct list *new);
struct list *list_pop(struct list *head);
struct list *list_pop_tail(struct list *head);
