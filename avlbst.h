#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key,Value>* node);
    void rotateRight(AVLNode<Key,Value>* node);
    void removeFix(AVLNode<Key,Value>* node, int8_t diff);
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // if tree is blank
    if(this->root_ == nullptr){
      this->root_ = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
      return;
    }
    
    // if tree is not blank, 
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key,Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while(curr != nullptr){
      parent = curr;

      if(curr -> getKey() == new_item.first){
        curr -> getItem().second = new_item.second;
        return;
      }else if(curr -> getKey() > new_item.first){
        curr = curr->getLeft();

      }else if(curr -> getKey() < new_item.first){
        curr = curr->getRight();
      }
    }


    // insert new node
    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if(new_item.first < parent -> getKey()){
        parent -> setLeft(node);
    }else{
        parent -> setRight(node);
    }

    // fix balance
    // if(parent->getBalance() == 0){
    //   insertFix(parent, node);
    // }

    insertFix(parent, node);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO

    AVLNode<Key, Value>* aim = static_cast<AVLNode<Key,Value>*>(this-> internalFind(key));
    if(aim == nullptr){
      return;
    }

    // if aim has child, swap with predecessor
    if(aim->getLeft() != nullptr && aim->getRight() != nullptr){
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key,Value>*> (this -> predecessor(aim));
      nodeSwap(aim, pred);
    }

    // only possible for aim to have a left child
    AVLNode<Key, Value>* child = nullptr;
    AVLNode<Key, Value>* parent = aim ->getParent();
    if(aim->getLeft() != nullptr){
      child = aim -> getLeft();
    }
    if(aim->getRight() != nullptr){
      child = aim -> getRight();
    }
    
    if(child != nullptr){
      child -> setParent (parent);
    }

    int8_t diff = 0;

    if(parent == nullptr){
      this -> root_ = child;
    }else{
        if(parent -> getLeft() == aim){
            parent -> setLeft(child);
            diff = 1;
        }else{
            parent -> setRight(child);
            diff = -1;
        }
    }
    delete aim;

    if(parent != nullptr && diff != 0){
        removeFix(parent, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}



template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node){

    if(parent == nullptr){
      return;
    }

    AVLNode<Key, Value>* grandparent = parent -> getParent();

    if(node == parent -> getLeft()){
      parent -> updateBalance(-1);

      int8_t balance = parent -> getBalance();
      if(balance == 0){
        return;
      }else if (balance == -1){
        insertFix(grandparent,parent);
      }else if(balance == -2){
        if(node -> getBalance() <= 0){
          rotateRight(parent);
          parent -> setBalance(0);
          node -> setBalance(0);
        }
        else{
        AVLNode<Key, Value>* rest = node -> getRight();
        rotateLeft(node);
        rotateRight(parent);

        int8_t restBalance = rest -> getBalance();
        if(restBalance == -1){
          parent -> setBalance(1);
          node -> setBalance(0);
        }
        else if(restBalance == 0){
          parent -> setBalance(0);
          node -> setBalance(0);
        }
        else{
          parent -> setBalance(0);
          node -> setBalance(-1);
        }

        if (rest != nullptr){
          rest -> setBalance(0);
        }
      }
        
      }

    }
    else{
      parent -> updateBalance(1);

      int8_t balance = parent -> getBalance();
      if(balance == 0){
        return;
      }else if (balance == 1){
        insertFix(grandparent,parent);
      }else if(balance == 2){
        if(node -> getBalance() >= 0){
          rotateLeft(parent);
          parent -> setBalance(0);
          node -> setBalance(0);
        }
        else{
        AVLNode<Key, Value>* rest = node -> getLeft();
        rotateRight(node);
        rotateLeft(parent);

        int8_t restBalance = rest -> getBalance();
        if(restBalance == 1){
          parent -> setBalance(-1);
          node -> setBalance(0);
        }
        else if(restBalance == 0){
          parent -> setBalance(0);
          node -> setBalance(0);
        }
        else{
          parent -> setBalance(0);
          node -> setBalance(1);
        }

        if (rest != nullptr){
          rest -> setBalance(0);
        }
      }
      }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* node, int8_t diff){
    while(node != nullptr){

        AVLNode<Key, Value>* parent = node -> getParent();
        int8_t nextDiff = 0;

        if(parent != nullptr){
          if(node == parent -> getLeft()){
            nextDiff = 1;
          }
          else{
            nextDiff = -1;
          }
        }

        node -> updateBalance(diff);
        int8_t balance = node -> getBalance();

        if(balance == -1 || balance == 1){
            break;
        }

        if(balance == 2){
            AVLNode<Key,Value>* rightChild = node -> getRight();

            if(rightChild == nullptr){
              break;
            }

            int8_t rightBalance = rightChild -> getBalance();

            if (rightBalance == -1){
                AVLNode<Key,Value>* rightLeftChild = rightChild->getLeft();
                int8_t rlBalance;
                if(rightLeftChild != nullptr){
                    rlBalance = rightLeftChild->getBalance();
                }
                else{
                    rlBalance = 0;
                }
                
                rotateRight(rightChild);
                rotateLeft(node);

                if(rlBalance == 1){
                    node->setBalance(-1);
                    rightChild->setBalance(0);
                }
                else if(rlBalance == 0){
                    node->setBalance(0);
                    rightChild->setBalance(0);
                }
                else{ 
                    node->setBalance(0);
                    rightChild->setBalance(1);
                }
                if(rightLeftChild) rightLeftChild->setBalance(0);

                node = parent;
                diff = nextDiff;
            }
            else if (rightBalance == 1){
                rotateLeft(node);
                node->setBalance(0);
                rightChild->setBalance(0);
                
                node = parent;
                diff = nextDiff;
            }
            else{  // 0
                rotateLeft(node);
                node->setBalance(1);
                rightChild->setBalance(-1);
                break; 
            }
        }

        else if(balance == -2){
            AVLNode<Key,Value>* leftChild = node -> getLeft();

            if (leftChild == nullptr){
              break;
            }

            int8_t leftBalance = leftChild->getBalance(); 

            if (leftBalance == 1){
                AVLNode<Key,Value>* leftRightChild = leftChild->getRight();
                int8_t lrBalance;
                if(leftRightChild != nullptr){
                    lrBalance = leftRightChild->getBalance();
                }
                else{
                    lrBalance = 0;
                }
                
                rotateLeft(leftChild);
                rotateRight(node);

              
                if(lrBalance == -1){
                    node->setBalance(1);
                    leftChild->setBalance(0);
                }
                else if(lrBalance == 0){
                    node->setBalance(0);
                    leftChild->setBalance(0);
                }
                else{  // 1
                    node->setBalance(0);
                    leftChild->setBalance(-1);
                }
                if(leftRightChild) leftRightChild->setBalance(0);

                node = parent; 
                diff = nextDiff;
            }
            else if (leftBalance == -1){
                rotateRight(node);
                node->setBalance(0); 
                leftChild->setBalance(0);
                
                node = parent; 
                diff = nextDiff;
            }
            else{ 
                rotateRight(node);
                node->setBalance(-1); 
                leftChild->setBalance(1);
                break; 
            }
        
        }

        else{
            node = parent;
            diff = nextDiff;
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* node){
    if(node == nullptr || node -> getRight() == nullptr){
      return;
    }

    
    AVLNode<Key,Value>* newNode = node -> getRight();
    AVLNode<Key,Value>* parent = node -> getParent();

    if(newNode -> getLeft()!= nullptr){
        node -> setRight(newNode -> getLeft());
        newNode -> getLeft() -> setParent(node);
    }else{
        node -> setRight(nullptr);
    }
    newNode -> setLeft(node);
    node -> setParent(newNode);
    newNode -> setParent(parent);

    if(parent == nullptr){
        this -> root_ = newNode;
    }else if(parent -> getLeft() == node){
        parent -> setLeft(newNode);
    }else{
        parent -> setRight(newNode);
    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* node){

    if(node == nullptr || node -> getLeft() == nullptr){
      return;
    }
    
    AVLNode<Key,Value>* newNode = node -> getLeft();
    AVLNode<Key,Value>* parent = node -> getParent();


    if(newNode -> getRight()!= nullptr){
        node -> setLeft(newNode -> getRight());
        newNode -> getRight() -> setParent(node);
    }else{
        node -> setLeft(nullptr);
    }
    newNode -> setRight(node);
    node -> setParent(newNode);
    newNode -> setParent(parent);
    

    if(parent == nullptr){
        this -> root_ = newNode;
    }else if(parent -> getRight() == node){
        parent -> setRight(newNode);
    }else{
        parent -> setLeft(newNode);
    }
}

#endif
