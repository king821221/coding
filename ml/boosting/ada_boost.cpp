#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

inline int sign(double v) {return v > std::numeric_limits<double>::epsilon() ? 1 : (v < -std::numeric_limits<double>::epsilon() ? -1 : 0);}

class Gmx
{
public:
       double v;
       int predict(int x) const {return x < v ? 1 : -1;}
};

inline int I(int a, int b) {return a != b ? 1 : 0;}

double cal_error(const vector<pair<int, int> > &training, const Gmx& gmx, const vector<double>& weights)
{
   double err = 0;
   for (int i = 0; i < training.size(); i++)
   {
       int predicted = gmx.predict(training[i].first);
       //printf("training: %d, gmx: %f, predicted: %d, actual: %d, err_cnt: %d, weight: %f\n", training[i].first, gmx.v, predicted, training[i].second, I(predicted, training[i].second), weights[i]);
       err +=  weights[i] * I(predicted, training[i].second);
   }
   //printf("training error: %f, gmx: %f\n", err, gmx.v);
   return err;
}

void train(const vector<pair<int, int> > &training, const vector<double>& weights, Gmx& gmx, double& em)
{
    int n = training.size();
    gmx.v = training[0].first;
    double min_err = cal_error(training, gmx, weights);
    double minv = gmx.v;
    for (int i = 1; i < n; i++)
    {
        gmx.v = training[i-1].first + (training[i].first - training[i-1].first) / 2.0;
        double err = cal_error(training, gmx, weights);
        if (err < min_err)
        {
            min_err = err;
            minv = gmx.v;
        }
        gmx.v = training[i].first;
        err = cal_error(training, gmx, weights);
        if (err < min_err)
        {
            min_err = err;
            minv = gmx.v;
        }
    }
    em = min_err;
    gmx.v = minv;
}

void update_weights(vector<double>& weights, double alpham, const vector<pair<int, int> >& training, const Gmx& gmx)
{
    int n = training.size();
    double z = 0;
    vector<double> new_weights(n);
    for (int i = 0; i < n; i++)
    {
        new_weights[i] = weights[i] * exp(-alpham * training[i].second * gmx.predict(training[i].first));
        z += new_weights[i];
    }
    for (int i = 0; i < n; i++)
    {
        weights[i] =  new_weights[i] / z;
        //printf("weights[%d]=%f, correct: %d\n", i, weights[i], I(gmx.predict(training[i].first), training[i].second));
    }
    //printf("-----------------------\n");
}

void ada_boost(vector<pair<int, int> > &training, vector<pair<double, Gmx> >& output, int M)
{
    int N = training.size();
    vector<double> weights;
    for (int i = 0; i < N; i++)
    {
        weights.push_back(1.0/N);
    }
    for (int m = 0; m < M; m++)
    {
        Gmx gmx;
        double em;
        train(training, weights, gmx, em);
        double alpham = (log(1-em) - log(em)) / 2;
        update_weights(weights, alpham, training, gmx);
        pair<double, Gmx> p;
        p.first = alpham;
        p.second = gmx;
        output.push_back(p);
    }
}

int predict(vector<pair<double, Gmx> >& model, int test)
{
    double f = 0;
    for (int j = 0; j < model.size(); j++)
    {
        double alpha = model[j].first;
        Gmx gmx = model[j].second;
        f = f + alpha * gmx.predict(test);
    }
    return sign(f);
}

void output_model(const vector<pair<double, Gmx> >& model)
{
    for (int i = 0; i < model.size(); i++)
    {
         const pair<double, Gmx>& sub_model = model[i];
         printf("model[%d] weight: %f, division: %f\n", i, sub_model.first, sub_model.second.v);
    }
}

int main(int argc, char** argv)
{
    int M = atoi(argv[1]);
    std::ifstream ifs (argv[2], std::ifstream::in);
    std::ifstream tfs (argv[3], std::ifstream::in);
    vector<pair<int, int> > train;
    vector<int> test;
    vector<pair<double, Gmx> > model;
    while (!ifs.eof())
    {
        int x,y;
        ifs >> x >> y;
        pair<int, int> p;
        p.first = x;
        p.second = y;
        train.push_back(p);
    }
    train.pop_back();
    ada_boost(train, model, M);
    output_model(model);
    ifs.close();
    while (!tfs.eof())
    {
        int x;
        tfs >> x;
        test.push_back(x);
    }
    tfs.close();
    double pred_err = 0;
    for (int i = 0; i < test.size(); i++)
    {
        int pred = predict(model, test[i]);
        printf("test: %d, pred: %d\n", test[i], pred);
    }
}
