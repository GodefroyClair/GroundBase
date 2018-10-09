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
#include <stdio.h>
//#include <stdlib.h>
#include <sys/types.h>

#include "../../GBAllocator.h"
#include "rbtree.h"

tree_node RBNIL = {
  .node = NULL,
  .color = BLACK,
  .parent = &RBNIL,
  .left = &RBNIL,
  .right = &RBNIL
};

static tree_node* new_rbtree_node(void* node){
  tree_node *z = GBMalloc( sizeof(tree_node) );
  
  z->node = node;
  z->parent = &RBNIL;
  z->left = &RBNIL;
  z->right = &RBNIL;
  z->color = RED;
  return z;
}

static void* __pointer(struct sroot* self,tree_node* node)
{
    (void) self;
    
    return node->node;
}

static int64_t __compare_by_pointer(struct sroot* self,void* keyA, void* keyB)
{
    (void) self;
    return (char*) keyA - (char*)keyB;
}

tree_root* new_simple_rbtree(){
  return new_rbtree(NULL, NULL);  
}




tree_root* new_rbtree( RBTreeKeyPointer key_function_pointer, RBTreeCompare compare_function_pointer )
{
    tree_root* r = GBMalloc(sizeof( tree_root ));
  r->root = &RBNIL;
    
  if(key_function_pointer == NULL && compare_function_pointer == NULL){
    r->key = &__pointer;
    r->compare = &__compare_by_pointer;
  }else{
    r->key = key_function_pointer;
    r->compare = compare_function_pointer;
  }
    
    r->user = NULL;
  return r;
}

/*WARNNING left_rbrotate assumes that rbrotate_on->right is NOT &RBNIL and that root->parent IS &RBNIL*/
static void left_rbrotate(tree_root* root, tree_node* rbrotate_on){
  tree_node* y = rbrotate_on->right;
  rbrotate_on->right = y->left;

  if(y->left != &RBNIL)
    y->left->parent = rbrotate_on;
  
  y->parent = rbrotate_on->parent;
 
  if(rbrotate_on->parent == &RBNIL)
    root->root = y;
  else
    if(rbrotate_on == rbrotate_on->parent->left)
      rbrotate_on->parent->left = y;
    else
      rbrotate_on->parent->right = y;
 
  y->left = rbrotate_on;
  rbrotate_on->parent = y;
  return;
}

/*WARNNING right_rbrotate assumes that rbrotate_on->left is NOT &RBNIL and that root->parent IS &RBNIL*/
static void right_rbrotate(tree_root* root, tree_node* rbrotate_on){
  tree_node* y = rbrotate_on->left;
  rbrotate_on->left = y->right;

  if(y->right != &RBNIL)
    y->right->parent = rbrotate_on;
  
  y->parent = rbrotate_on->parent;
 
  if(rbrotate_on->parent == &RBNIL)
    root->root = y;
  else
    if(rbrotate_on == rbrotate_on->parent->right)
      rbrotate_on->parent->right = y;
    else
      rbrotate_on->parent->left = y;
 
  y->right = rbrotate_on;
  rbrotate_on->parent = y;
  return;
}

static void rb_tree_insert_fixup(tree_root* root, tree_node* z){
  tree_node* y;

  while(z->parent->color == RED){
    
    if(z->parent == z->parent->parent->left){
      y = z->parent->parent->right;
      
      if(y->color == RED){
	z->parent->color = BLACK;
	y->color = BLACK;
	z->parent->parent->color = RED;
	z = z->parent->parent;
      }else{
	if(z == z->parent->right){
	  z = z->parent;
	  left_rbrotate(root, z);
	}
	z->parent->color = BLACK;
	z->parent->parent->color = RED;
	right_rbrotate(root, z->parent->parent);
      }
      
    }else{
      y = z->parent->parent->left;
      
      if(y->color == RED){
	z->parent->color = BLACK;
	y->color = BLACK;
	z->parent->parent->color = RED;
	z = z->parent->parent;
      }else{
	if(z == z->parent->left){
	  z = z->parent;
	  right_rbrotate(root, z);
	}
	z->parent->color = BLACK;
	z->parent->parent->color = RED;
	left_rbrotate(root, z->parent->parent);
      }
    }
  }

  root->root->color = BLACK;
}

void* rb_tree_insert(tree_root* root, void* node)
{
    tree_node *y = &RBNIL, *x = root->root;

    tree_node *z = new_rbtree_node(node);

    while(x != &RBNIL)
    {
        y = x;

        const int64_t comp = root->compare(root , root->key(root ,z), root->key(root ,x)) ;
      
        if(comp== 0)
        {
            void* holder = x->node;
            GBFree(z);
            x->node = node;
            return holder;
        }

        if( comp < 0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
  
    z->parent = y;

    if(y == &RBNIL)
    {
        root->root = z;
    }
    else
    {
        if(root->compare(root , root->key(root ,z),root->key(root ,y)) < 0)
        {
            y->left = z;
        }
        else
        {
            y->right = z;
        }
    }

    rb_tree_insert_fixup(root, z);
    return NULL;
}

static tree_node* __search_rbtree_node(tree_root* root, void* key)
{
  tree_node *z = root->root;

  while(z != &RBNIL)
  {
      
      const int64_t comp = root->compare(root , root->key(root ,z), key);
    if( comp == 0)
      return z;

    if(comp  < 0)
      z = z->right;
    else
      z = z->left;
  }

  return NULL;
}

void* search_rbtree(tree_root* root, void* key)
{
  tree_node *z = __search_rbtree_node(root, key);
  if(z)
    return z->node;
  return NULL;
}

static uint8_t _clear_rbtree(tree_node* node)
{

    if(node == &RBNIL || node == NULL)
        return 0;

    tree_node *l = node->left;
    tree_node *r = node->right;

    GBFree(node);
    node = NULL;

    _clear_rbtree(l);
    _clear_rbtree(r);
    
    
    
    return 1;
}
uint8_t clear_rbtree(tree_root* root)
{
    if( _clear_rbtree(root->root))
    {
        root->root = NULL;
    }
    return root->root == NULL;
}

void destroy_rbtree(tree_root* root)
{
    if( root->root)
    _clear_rbtree(root->root);
    GBFree(root);
    root = NULL;
}

static void __rb_transplant(tree_root* root, tree_node* u, tree_node* v){
  if(u->parent == &RBNIL)
    root->root = v;
  else
    if(u == u->parent->left)
      u->parent->left = v;
    else
      u->parent->right = v;
  v->parent = u->parent;
}

static tree_node* __rb_tree_minimum(tree_node* z){
  for(; z->left != &RBNIL; z = z->left);
  return z;
}

static void __rb_tree_delete_fixup(tree_root* root, tree_node* x){
  tree_node* w;
  while( x != root->root && x->color == BLACK){
    if(x == x->parent->left){
      w = x->parent->right;
      if(w->color == RED){
	w->color = BLACK;
	x->parent->color = RED;
	left_rbrotate(root, x->parent);
	w = x->parent->right;
      }
      if(w->left->color == BLACK && w->right->color == BLACK){
	w->color = RED;
	x = x->parent;
      }
      else{
	if(w->right->color == BLACK){
	  w->left->color = BLACK;
	  w->color = RED;
	  right_rbrotate(root, w);
	  w = w->parent->right;
	}
	w->color = x->parent->color;
	x->parent->color = BLACK;
	w->right->color = BLACK;
	left_rbrotate(root, x->parent);
	x = root->root;
      }
    }
    else{
      w = x->parent->left;
      if(w->color == RED){
	w->color = BLACK;
	x->parent->color = RED;
	right_rbrotate(root, x->parent);
	w = x->parent->left;
      }
      if(w->right->color == BLACK && w->left->color == BLACK){
	w->color = RED;
	x = x->parent;
      }
      else{
	if(w->left->color == BLACK){
	  w->right->color = BLACK;
	  w->color = RED;
	  left_rbrotate(root, w);
	  w = w->parent->left;
	}
	w->color = x->parent->color;
	x->parent->color = BLACK;
	w->left->color = BLACK;
	right_rbrotate(root, x->parent);
	x = root->root;
      }
    }
  }
  x->color = BLACK;
  return;
}

void* rb_tree_delete(tree_root* root, void* key)
{
  tree_node *y, *z, *x, *hold_node_to_delete;
  uint8_t y_original_color;
  void* node_to_return;

  hold_node_to_delete = y = z = __search_rbtree_node( root, key);

  if(y == NULL)
  {

    return NULL;
  }

  node_to_return = y->node;

  y_original_color = y->color;
  if(z->left == &RBNIL){
    x = z->right;
    __rb_transplant(root, z, z->right);
  }
  else
    if(z->right == &RBNIL){
      x = z->left;
      __rb_transplant(root, z, z->left);
    }
    else{
      y = __rb_tree_minimum(z->right);
      y_original_color = y->color;
      x = y->right;
      if(y->parent == z)
	x->parent = y;
      else{
	__rb_transplant(root, y, y->right);
	y->right = z->right;
	y->right->parent = y;
      }
      __rb_transplant(root, z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
    }
  if(y_original_color == BLACK)
    __rb_tree_delete_fixup(root, x);

  GBFree(hold_node_to_delete);
  return node_to_return;
}

tree_iterator* new_tree_iterator(tree_root* root)
{
  tree_node* aux = root->root;
  tree_iterator* it = GBMalloc( sizeof( tree_iterator ) );

  while(aux->left != &RBNIL || aux->right != &RBNIL){ 
    while(aux->left != &RBNIL)
      aux = aux->left;
    
    if(aux->right != &RBNIL)
      aux = aux->right;
  }
  it->current = aux;
  return it;
}

uint8_t tree_iterator_has_next(tree_iterator* it){
  if(it->current != &RBNIL)
    return 1;
  return 0;
}

void* tree_iterator_next(tree_iterator* it){
  tree_node* aux;
  tree_node* tn = it->current;

  if(tn->parent != &RBNIL && 
     tn->parent->right != &RBNIL && 
     tn->parent->right != tn){
    aux = tn->parent->right;
    while(aux->left != &RBNIL || aux->right != &RBNIL){ 
      while(aux->left != &RBNIL)
	aux = aux->left;
    
      if(aux->right != &RBNIL)
	aux = aux->right;
    }   
    it->current = aux;
  }
  else
    it->current = it->current->parent;
  return tn->node;
}

void destroy_iterator(tree_iterator* it)
{
  GBFree(it);
}
