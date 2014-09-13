#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fstream>
#include <vector>
using namespace std;

class string_iterator
{
public:
   virtual bool has_next() = 0;
   virtual char next() = 0;
};

class default_string_iterator: public string_iterator
{
private:
   const char* str;
   const char* p;
public:
   default_string_iterator(const char* s): str(s), p(s){}
   bool has_next()
   {
      return p && *p;
   }
   char next()
   {
      assert(has_next());
      return *p++;
   }
};

class KMP
{
private:
    const char* pattern;
    int len;
    int* next;
public:
    KMP(const char* p) : pattern(p){build_next();}
    void find_all(string_iterator& str_iter, vector<int>& found)
    {
        int matched = 0;
        int pos = -1;
        while (str_iter.has_next())
        {
            char c = str_iter.next();
            ++pos;
            while (matched > 0 && pattern[matched] != c)
            {
                matched = next[matched - 1] + 1;
            }
            if (pattern[matched] == c)
            {
                matched++;
                if (matched == len)
                {
                   found.push_back(pos);
                   matched = next[matched - 1] + 1;
                }
            }
        }
    }
    ~KMP()
    {
        if (next)
        {
            delete []next;
        }
    }
protected:
    void build_next()
    {
        //ABABAC
        //next[0] = -1, next[1] = -1, next[2] = 0, next[3] = 1, next[4] =  2, next[5] = -1
        assert(pattern != NULL && *pattern != 0);
        len = strlen(pattern);
        next = new int[len];
        assert(next != NULL);
        next[0] = -1;
        for (int i = 1; i < len; i++)
        {
           next[i] = -1;
           int j = next[i - 1];
           while(j >= 0 && pattern[j + 1] != pattern[i])
           {
                j = next[j];
           }
           if (pattern[j + 1] == pattern[i])
           {
                next[i] = j + 1;
           }
        }
    }
};


int main(int argc, char* argv[])
{
     const char* pattern = argv[1];
     const char* file = argv[2];
     ifstream ifs(file, std::ifstream::in);
     ifs.seekg (0, ifs.end);
     int length = ifs.tellg();
     ifs.seekg (0, ifs.beg);
     char * buffer = new char [length + 1];
     ifs.read (buffer,length);
     buffer[length] = 0;
     default_string_iterator iter(buffer);
     vector<int> found;
     KMP kmp(pattern);
     kmp.find_all(iter, found);
     ifs.close();
     for (int i = 0; i < found.size(); i++)
     {
         printf("%d\n", found[i]);
     }
}
