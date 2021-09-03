#include <string.h>

#include <rbtree.h>

static void rotate_node(struct rb_tree *tree, struct rb_node *node,
	enum rb_dir dir)
{
	struct rb_node *parent = node->parent;
	struct rb_node *child = node->child[!dir];

	if ((node->child[!dir] = node->child[dir])) {
		node->child[dir]->parent = node;
	}

	child->child[dir] = node;
	child->parent = parent;

	if (parent) {
		parent->child[parent->child[RB_RIGHT] == node] = child;
	} else {
		tree->root = child;
	}

	node->parent = child;
}

static struct rb_node *get_outermost(struct rb_node *node,
	enum rb_dir dir)
{
	if (!node) {
		return NULL;
	}

	while (node->child[dir]) {
		node = node->child[dir];
	}
	
	return node;
}

static struct rb_node *get_closest(struct rb_node *node,
	enum rb_dir dir)
{
	struct rb_node *parent;

	if (!node)
		return NULL;

	if (node->child[dir])
		return get_outermost(node->child[dir], !dir);

	while ((parent = node->parent) && parent->child[dir] == node)
		node = parent;

	return parent;
}

struct rb_node *rb_first(struct rb_tree *tree)
{
	return get_outermost(tree->root, RB_LEFT);
}

struct rb_node *rb_last(struct rb_tree *tree)
{
	return get_outermost(tree->root, RB_RIGHT);
}

struct rb_node *rb_next(struct rb_node *node)
{
	return get_closest(node, RB_RIGHT);
}

struct rb_node *rb_prev(struct rb_node *node)
{
	return get_closest(node, RB_LEFT);
}

int rb_insert(struct rb_tree *tree, struct rb_node *node)
{
	struct rb_node *parent, *grandparent, *uncle;
	enum rb_dir dir;

	if (!tree || !node)
		return -1;

	node->color = RB_RED;

	while ((parent = node->parent) && parent->color == RB_RED) {
		grandparent = parent->parent;
		dir = grandparent->child[RB_LEFT] == parent;
		uncle = grandparent->child[dir];

		if (uncle && uncle->color == RB_RED) {
			parent->color = RB_BLACK;
			uncle->color = RB_BLACK;
			grandparent->color = RB_RED;
			node = grandparent;

			continue;
		}

		if (parent->child[dir] == node) {
			rotate_node(tree, parent, !dir);
			node = parent;
			parent = node->parent;
		}

		parent->color = RB_BLACK;
		grandparent->color = RB_RED;

		rotate_node(tree, grandparent, dir);
	}

	tree->root->color = RB_BLACK;

	return 0;
}

static int do_remove(struct rb_tree *tree,
	struct rb_node *node)
{
	struct rb_node *parent, *child;
	enum rb_dir dir, child_dir;

	if (!tree || !node)
		return -1;

	parent = node->parent;

	if (node->child[RB_LEFT] && node->child[RB_RIGHT]) {
		child = node->child[RB_LEFT];
		child = get_outermost(child, RB_RIGHT);
		do_remove(tree, child);

		child->child[RB_LEFT] = node->child[RB_LEFT];

		if (node->child[RB_LEFT])
			node->child[RB_LEFT]->parent = child;

		child->child[RB_RIGHT] = node->child[RB_RIGHT];

		if (node->child[RB_RIGHT])
			node->child[RB_RIGHT]->parent = child;
	} else {
		child_dir = !node->child[RB_LEFT];
		child = node->child[child_dir];
	}

	if (parent) {
		dir = parent->child[RB_RIGHT] == node;
		parent->child[dir] = child;
	} else {
		tree->root = child;
	}

	if (child)
		child->parent = parent;

	memset(node, 0, sizeof *node);

	return 0;
}

int rb_remove(struct rb_tree *tree, struct rb_node *node)
{
	struct rb_node *sibling, *child, *other;
	enum rb_dir dir, child_dir;

	if (!tree || !node)
		return -1;

	child_dir = !node->child[RB_LEFT];
	child = node->child[child_dir];
	other = node->child[!child_dir];

	do_remove(tree, node);

	if (!child && !other)
		return 0;

	if (node->color == RB_RED)
		return 0;

	node = child;

	if (node->color == RB_RED) {
		node->color = RB_BLACK;
		return 0;
	}

	while (node->parent) {
		dir = node->parent->child[RB_LEFT] == node;
		sibling = node->parent->child[dir];

		if (!sibling) {
			break;
		}

		if (sibling->color == RB_RED) {
			node->parent->color = RB_RED;
			sibling->color = RB_BLACK;
			rotate_node(tree, node->parent, !dir);
			sibling = node->parent->child[dir];
		};

		if ((!sibling->child[!dir] || sibling->child[!dir]->color ==
		    RB_BLACK) &&
		    (!sibling->child[dir] || sibling->child[dir]->color ==
		    RB_BLACK)) {
			sibling->color = RB_RED;
			node = node->parent;
			continue;
		}

		if (!sibling->child[dir] || sibling->child[dir]->color ==
		    RB_BLACK) {
			sibling->color = RB_RED;
			sibling->child[!dir]->color = RB_BLACK;
			rotate_node(tree, node->parent, dir);
			sibling = node->parent->child[dir];
		}

		sibling->color = node->parent->color;
		node->parent->color = RB_BLACK;
		sibling->child[dir]->color = RB_BLACK;
		rotate_node(tree, node->parent, !dir);

		node = tree->root;
	}

	if (node)
		node->color = RB_BLACK;

	return 0;
}

int rb_replace(struct rb_tree *tree, struct rb_node *node,
	struct rb_node *new_node)
{
	if (!node || !new_node)
		return -1;

	if (node->parent)
		node->parent->child[node->parent->child[RB_RIGHT] == node] =
			new_node;
	else
		tree->root = new_node;

	if (node->child[RB_LEFT])
		node->child[RB_LEFT]->parent = new_node;

	if (node->child[RB_RIGHT])
		node->child[RB_RIGHT]->parent = new_node;

	*new_node = *node;

	return 0;
}

