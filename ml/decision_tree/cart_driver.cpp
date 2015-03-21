#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "cart.h"
#include <vector>
#include <string>
#include <fstream>

static void read_feature_file(const char* path, vector<string>& feats)
{
   std::ifstream ifs (path, std::ifstream::in);
   if (ifs.is_open())
   {
      while (ifs.good() && !ifs.eof())
      {
         string feat;
         std::getline(ifs, feat, '\t');
         if (feat.length() > 0)
         {
            feats.push_back(feat);
         }
      }
   }
}

static void read_data_file(const char* path, vector<string>& data)
{
   std::ifstream ifs (path, std::ifstream::in);

   if (ifs.is_open())
   {
       while (ifs.good() && !ifs.eof())
       {
           string sample;
           std::getline(ifs, sample);
           if (sample.length() > 0)
           {
               data.push_back(sample);
           }
       }
   }
}

static void visit(const tree_node* root, const vector<string>& feats)
{
     if (root)
     {
         const tree_node_split& best_split = root->get_best_split();
         size_t fid = best_split.fid;
         double val = best_split.val;
         printf("split feature: %s, split feature value: %f, node value: %f\n", feats[fid].c_str(), val, root->get_node_val());
     }
}

static void traverse(const tree_node* root, const vector<string>& feats)
{
     if (root)
     {
        visit(root, feats);
        traverse(root->get_left(), feats);
        traverse(root->get_right(), feats);
     }
}

int main(int argc, char** argv)
{
    vector<string> feats;
    vector<string> data;
    size_t min_samples;
    read_feature_file(argv[1], feats);
    read_data_file(argv[2], data);
    min_samples = atoi(argv[2]);
    cart dc(feats, data, min_samples);
    tree_node* root = dc.gen_subtree();
    traverse(root, feats);
    return 0;
}
