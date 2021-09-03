#include <list.h>

void list_init(struct list *head)
{
	head->next = head;
	head->prev = head;
}

void list_insert_after(struct list *after, struct list *new)
{
	new->prev = after;
	new->next = after->next;

	after->next->prev = new;
	after->next = new;
}

void list_insert_before(struct list *before, struct list *new)
{
	new->next = before;
	new->prev = before->prev;

	before->prev->next = new;
	before->prev = new;
}

void list_del(struct list *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;

	node->next = node;
	node->prev = node;
}

void list_add(struct list *head, struct list *new)
{
	list_insert_after(head, new);
}

void list_add_tail(struct list *head, struct list *new)
{
	list_insert_before(head, new);
}

struct list *list_pop(struct list *head)
{
	struct list *node;

	if (head->prev == head)
		return NULL;

	node = head->prev;
	list_del(node);

	return node;
}

struct list *list_pop_tail(struct list *head)
{
	struct list *node;

	if (head->next == head)
		return NULL;

	node = head->next;
	list_del(node);

	return node;
}
