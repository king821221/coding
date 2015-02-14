#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "info_gain_ratio.h"
using namespace std;

int main(int argc, char** argv)
{
    fstream fs ("test.txt");
    vector<pair<string,int> > samples;
    while (!fs.eof())
    {
        string f;
        int l;
        fs >> f >> l;
        if (f.length() == 0) break;
        pair<string, int> p;
        p.first = f;
        p.second = l;
        samples.push_back(p);
        cout << "feature: " << f << ", label: " << l << endl;
    }
    printf("info_gain_ratio: %f\n", info_gain_ratio(samples));
    fs.close();
}
