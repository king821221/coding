#include <float.h>
#include <map>
#include <vector>
#include <set>
using namespace std;

#pragma once

#define INVALID_NODE_VAL (FLT_MIN)

class sample
{
public:
        double y;
        vector<double> x;
};

class tree_node_split
{
public:
        size_t fid; // split feature
        double val; //split value of the split feature
};

class tree_node
{
private:
       tree_node* left;
       tree_node* right;
       tree_node_split best_split;
       double node_val;
public:
       tree_node(double val = INVALID_NODE_VAL): left(NULL), right(NULL), node_val(val) {}
       tree_node(tree_node* l, tree_node* r, const tree_node_split& s, double val = INVALID_NODE_VAL): left(l), right(r), best_split(s), node_val(val) {}
       tree_node* get_left() const {return left;}
       tree_node* get_right() const {return right;}
       const tree_node_split& get_best_split() const {return best_split;}
       double get_node_val() const {return node_val;}
};
