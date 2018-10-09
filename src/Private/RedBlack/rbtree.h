/*
 * Copyright (c) 2017 FlyLab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
/*
 Code base from:
 https://github.com/fgoncalves/Generic-Red-Black-Tree
 */

#ifndef __RBTREE_H__
#define __RBTREE_H__

#define RED 1
#define BLACK 2

#include <stdint.h>


typedef struct stree_node{
  void* node;

  uint8_t color;
  
  struct stree_node* parent;
  struct stree_node* left;
  struct stree_node* right;
}tree_node;

struct sroot;
typedef void* (*RBTreeKeyPointer)(struct sroot* self, struct stree_node* node);
typedef int64_t (*RBTreeCompare)( struct sroot* self, void* keyA, void* keyB);

typedef struct sroot
{
    struct stree_node* root;
    RBTreeKeyPointer key;
    RBTreeCompare compare;
    
    void* user;
    
}tree_root;

typedef struct siterator{
  struct stree_node* current;
}tree_iterator;

tree_root* new_simple_rbtree(void);

tree_root* new_rbtree(RBTreeKeyPointer key_function_pointer, RBTreeCompare compare_function_pointer );

void* rb_tree_insert(tree_root* root, void* node);
void* rb_tree_delete(tree_root* root, void* key);
void* search_rbtree(tree_root* root, void* key);

uint8_t clear_rbtree(tree_root* root);
void destroy_rbtree(tree_root* root);

tree_iterator* new_tree_iterator(tree_root* root);
uint8_t tree_iterator_has_next(tree_iterator* it);
void* tree_iterator_next(tree_iterator* it);
void destroy_iterator(tree_iterator* it);
#endif
