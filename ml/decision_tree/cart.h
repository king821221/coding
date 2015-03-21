#include "tree_gen.h"
#include "bitset.h"
#include <vector>
#include <string>
#include <set>
using namespace std;

#pragma once

class cart: public tree_gen
{
private:
        vector<sample> samples;
        vector<string> features;
        size_t min_num_samples;
public:
        cart(const vector<string>& feats, const vector<string>& data, size_t min_samples);
        ~cart();
        virtual tree_node* gen_subtree();
protected:
        virtual bool should_terminate(const vector<const sample*>& samples, const bitset& splitted_features, size_t min_num_samples);
        virtual void find_best_split(vector<const sample*>& samples, const bitset& splitted_features, tree_node_split& split);
        virtual tree_node* recursive_gen(vector<const sample*>& samples, bitset& splitted_features, size_t min_num_samples);
        virtual double get_optimum_val(const vector<const sample*>& samples);
        virtual void split_samples(const tree_node_split& best_split, const vector<const sample*>& samples, vector<const sample*>& left_samples, vector<const sample*>& right_samples);
};
