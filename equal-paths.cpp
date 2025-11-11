#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include<algorithm>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int getDepth(Node* root);

int getDepth(Node* root){
  if(root == nullptr){
    return 0;
  }
  return 1+ max(getDepth(root -> left),getDepth(root -> right)) ;
}



bool equalPaths(Node * root)
{
    // Add your code below

  if(root == nullptr){
    return true;
  }

  if(root -> left == nullptr && root -> right == nullptr){
    return true;
  }

  int leftDepth = getDepth(root -> left);
  int rightDepth = getDepth(root -> right);

  if(root -> left && root -> right && leftDepth!=rightDepth){
    return false;
  }

  return equalPaths(root ->left) && equalPaths(root ->right);

}





