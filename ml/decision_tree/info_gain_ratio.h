#include <algorithm>
#include <vector>
#include <string>
#include <limits>
#include <assert.h>
#include <map>
#include <math.h>
using namespace std;

inline double plogp(double p)
{
    assert(p > 0);
    return fabs(p) < std::numeric_limits<double>::epsilon() ? 0 : p * log(p) / log(2);
}

double info_gain_ratio(vector<pair<string, int> >& samples)
{
    int n = samples.size();
    //H(D) - H(D|A)
    map<int, int> label_cnt;
    for (int i = 0; i < n; i++)
    {
       int label = samples[i].second;
       if (label_cnt.find(label) == label_cnt.end())
       {
           pair<int, int> lp;
           lp.first = label;
           lp.second = 1;
           label_cnt.insert(lp);
       }
       else
       {
           label_cnt[label]++;
       }
    }

    ///////
    double hd = 0;
    for (map<int, int>::iterator it = label_cnt.begin(); it != label_cnt.end(); ++it)
    {
        int cnt = it->second;
        double p = (double)cnt / n;
        hd += -plogp(p);
    }
    map<string, map<int, int> > feature_cnt;
    double hda = 0;
    for (int i = 0; i < n; i++)
    {
       string& f = samples[i].first;
       int label = samples[i].second;
       if (feature_cnt.find(f) == feature_cnt.end())
       {
          pair<string, map<int, int> > fp;
          fp.first = f;
          map<int, int> lcnt;
          pair<int, int> lpp;
          lpp.first = label;
          lpp.second = 1;
          lcnt.insert(lpp);
          fp.second = lcnt;
          feature_cnt.insert(fp);
       }
       else
       {
          map<int, int>& fcnt = feature_cnt[f];
          if (fcnt.find(label) == fcnt.end())
          {
             pair<int, int> lpp;
             lpp.first = label;
             lpp.second = 1;
             fcnt.insert(lpp);
          }
          else
          {
             fcnt[label]++;
          }
       }
    }
    double had = 0.0;
    for (map<string, map<int, int> >::iterator it = feature_cnt.begin(); it != feature_cnt.end(); ++it)
    {
        map<int,int>& feature_label_cnt = it->second;
        int flcnt = 0;
        double hdf = 0;
        for (map<int, int>::iterator fit = feature_label_cnt.begin(); fit != feature_label_cnt.end(); fit++)
        {
             flcnt += fit->second;
        }
        //printf("feature: %s, flcnt: %d, n: %d\n", it->first.c_str(), flcnt, n);
        had += -plogp((double)flcnt/n);
        for (map<int, int>::iterator fit = feature_label_cnt.begin(); fit != feature_label_cnt.end(); fit++)
        {
             double p = (double)(fit->second) / flcnt;
             hdf += - plogp(p);
        }
        hda +=  hdf * flcnt / n;
    }
    printf("hd = %f, hda = %f, had = %f\n", hd, hda, had);
    return (hd - hda)/had;
}
