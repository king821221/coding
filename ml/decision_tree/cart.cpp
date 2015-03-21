#include "cart.h"
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>
using namespace std;

class construct_exception: public exception
{
private:
    size_t line_no;
    string data_inst;
    char* err_msg;
public:
    construct_exception(size_t line, const string& data)
    {
        line_no = line;
        data_inst = data;
        err_msg = new char[1024 + data.length()];
        memset(err_msg, 0, sizeof(char) * (1024 + data.length()));
        sprintf(err_msg, "Caught exception on constructing cart tree object from external data, line: %ld, data: %s", line_no, data_inst.c_str());
    }
    ~construct_exception() throw()
    {
         if (err_msg)
         {
             delete []err_msg;
         }
    }
    virtual const char* what() const throw()
    {
        return err_msg;
    }
};

cart::cart(const vector<string>& feats, const vector<string>& data, size_t min_samples)
{
    size_t line = 0;
    for (vector<string>::const_iterator it = data.begin(); it != data.end(); it++)
    {
         const string& instance = *it;
         istringstream f(instance);
         string fval;
         vector<string> fvec;
         while (getline(f, fval, '	'))
         {
             fvec.push_back(fval);
         }
         if (fvec.size() < feats.size() + 1)
         {
             samples.clear();
             construct_exception ex(line, instance);
             throw ex;
         }
         sample sa;
         for(size_t idx = 0; idx < feats.size(); idx++)
         {
             const string& fval = fvec[idx];
             double xval = atof(fval.c_str());
             sa.x.push_back(xval);
         }
         sa.y = atof(fvec[fvec.size() - 1].c_str());
         samples.push_back(sa);
         ++line;
    }
    features = feats;
    min_num_samples = min_samples;
}

cart::~cart()
{}

tree_node* cart::gen_subtree()
{
     bitset splitted_features(features.size());
     vector<const sample*> sample_refs;
     for (size_t idx = 0; idx < samples.size(); idx++)
     {
          sample_refs.push_back(&samples[idx]);
     }
     return recursive_gen(sample_refs, splitted_features, min_num_samples);
}

bool cart::should_terminate(const vector<const sample*>& samples,
                           const bitset& splitted_features,
                           size_t min_num_samples)
{
     if (splitted_features.num_bits_set() == splitted_features.size())
     {
         return true;
     }
     if (samples.size() < min_num_samples)
     {
         return true;
     }
     return false;
}

double cart::get_optimum_val(const vector<const sample*>& samples)
{
     double sum = 0;
     for (std::vector<const sample*>::const_iterator it = samples.begin() ;
          it != samples.end(); ++it)
     {
          const sample* psa = *it;
          sum += psa->y;
     }
     return samples.size() == 0 ?  0.0 : sum / samples.size();
}
class feature_comp
{
private:
        size_t fid;
public:
        feature_comp(size_t id) : fid(id) {}
        bool operator() (const sample* sa, const sample* sb)
        {
            return sa->x[fid] < sb->x[fid];
        }
};

void cart::find_best_split(vector<const sample*>& samples,
                          const bitset& splitted_features,
                          tree_node_split& best_split)
{
     /*for each feature f not set in splitted_features
          for each splittable value s of feature f
              R1 = {samples whose feature f's value is smaller than s}
              R2 = {samples whose feature f's value is not smaller than s}
              calculate the cost
                  C = sum{(y1 - c1)^2} + sum{(y2 - c2)^2}
              c1 = mean(y) in R1, c2 = mean(y) in R2
       find (f,s) with mininum C
     */
     //for each feature f
     //  sort samples by f's value
     //  scan samples in ascending order of f's value
     //  for splittable value s
     //  C = sum(y1 ^ 2) + sum(c1 ^ 2) - 2 * c1 * sum(y1) + sum(y2 ^ 2) + sum(c2 ^ 2) - 2 * c2 * sum(y2)
     //  C = CL + CR
     //  c1 = sum(y1) / |CL|
     //  c2 = sum(y2) / |CR|
     //  CL = CLY2 + CLC2 - 2 * c1 * CLY
     //  CR = CRY2 + CRC2 - 2 * c2 * CRY
    double min_ds = -1;
    best_split.fid = splitted_features.size();
    best_split.val = FLT_MIN;
    for (size_t fid = 0; fid < splitted_features.size(); fid++)
    {
        if (splitted_features.test(fid))
        {
            continue;
        }
        //sort samples by feature fid
        feature_comp fcmp(fid);
        std::sort(samples.begin(), samples.end(), fcmp);
        double square_cy1, square_cy2, sum_y1, sum_y2, my1, my2, ds1, ds2;
        size_t ny1, ny2;
        square_cy1 = square_cy2 = sum_y1 = sum_y2 = ny1 = ny2 = my1 = my2 = ds1 = ds2 = 0;
        for (vector<const sample*>::iterator it = samples.begin(); it != samples.end(); it++)
        {
           const sample* sa = *it;
           square_cy2 += sa->y * sa->y;
           sum_y2 += sa->y;
           ny2++;
        }
        for (vector<const sample*>::iterator it = samples.begin(); it != samples.end(); it++)
        {
           const sample* sa = *it;
           //each sample's value is a split value
           square_cy1 += sa->y * sa->y;
           ny1++;
           sum_y1 += sa->y;
           my1 = sum_y1 / ny1;
           ds1 = square_cy1 + my1 * my1 * ny1 - 2 * my1 * sum_y1;
           square_cy2 -= sa->y * sa->y;
           ny2--;
           sum_y2 -= sa->y;
           my2 = sum_y2 / ny2;
           ds2 = square_cy2 + my2 * my2 * ny2 - 2 * my2 * sum_y2;
           if (min_ds < 0 || min_ds > ds1 + ds2)
           {
               min_ds = ds1 + ds2;
               best_split.fid = fid;
               best_split.val = sa->x[fid];
           }
        }
    }
}

void cart::split_samples(const tree_node_split& best_split,
                       const vector<const sample*>& samples,
                       vector<const sample*>& left_samples,
                       vector<const sample*>& right_samples)
{
    for (vector<const sample*>::const_iterator it = samples.begin(); it != samples.end(); it++)
    {
        const sample* psa = *it;
        size_t fid = best_split.fid;
        if (psa->x[fid] > best_split.val)
        {
            right_samples.push_back(psa);
        }
        else
        {
            left_samples.push_back(psa);
        }
    }
}

tree_node* cart::recursive_gen(vector<const sample*>& samples,
                              bitset& splitted_features,
                              size_t min_num_samples)
{
     bool terminate = should_terminate(samples, splitted_features, min_num_samples);
     if (!terminate)
     {
         tree_node_split best_split;
         find_best_split(samples, splitted_features, best_split);
         unsigned int split_fid = best_split.fid;
         splitted_features.set(split_fid);
         vector<const sample*> left_samples;
         vector<const sample*> right_samples;
         split_samples(best_split, samples, left_samples, right_samples);
         tree_node* left = recursive_gen(left_samples, splitted_features, min_num_samples);
         tree_node* right = recursive_gen(right_samples, splitted_features, min_num_samples);
         splitted_features.unset(split_fid);
         return new tree_node(left, right, best_split);
     }
     else
     {
         //for least squares loss, the optimal output value is the mean of the samples label
         double val = get_optimum_val(samples);
         return new tree_node(val);
     }
}
