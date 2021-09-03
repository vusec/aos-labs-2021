#pragma once

#include <types.h>
#include <string.h>

enum rb_dir {
	RB_LEFT,
	RB_RIGHT,
};

enum rb_color {
	RB_BLACK,
	RB_RED,
};

struct rb_node {
	struct rb_node *parent;
	struct rb_node *child[2];
	enum rb_color color;
};

struct rb_tree {
	struct rb_node *root;
};

struct rb_node *rb_first(struct rb_tree *tree);
struct rb_node *rb_last(struct rb_tree *tree);
struct rb_node *rb_next(struct rb_node *node);
struct rb_node *rb_prev(struct rb_node *node);

static inline void rb_init(struct rb_tree *tree)
{
	tree->root = NULL;
}

static inline void rb_node_init(struct rb_node *node)
{
	memset(node, 0, sizeof *node);
}

int rb_insert(struct rb_tree *tree, struct rb_node *node);
int rb_remove(struct rb_tree *tree, struct rb_node *node);
int rb_replace(struct rb_tree *tree, struct rb_node *node,
	struct rb_node *new_node);

