#include <stdio.h>
#include <iostream>
using namespace std;

int lower_bound(int* X, int* M, int l, int r, int val)
{
     //find j, l <= j <= r, X[M[j - 1]] >= val > X[M[j]]

     while (l < r)
     {
          int m = l + (r - l) / 2;

          if (X[M[m]] < val)
          {
                r = m;
          }
          else
          {
                 l = m + 1;
          }
     }

     if (X[M[l]] < val)
     {
          return l;
     }

      return -1;
}

int upper_bound(int* X, int* M, int l, int r, int val)
{
   //find j, l <= j <= r, X[M[j - 1]] <= val < X[M[j]]

   while (l < r)
   {
       int m = l + (r - l) / 2;
       if (X[M[m]] > val)
       {
           r = m;
       }
       else
       {
           l = m + 1;
       }
   }

   if (X[M[l]] > val)
   {
        return l;
   }

   return -1;
}

void backtrack_longest_sequence(int* X, int* prev, int idx)
{
     if (idx < 0)
     {
         return;
     }
     backtrack_longest_sequence(X, prev, prev[idx]);
     printf(" %d", X[idx]);
}

int find_longest_non_decrease_sequence(int* X, int n)
{
   int* M = new int[n + 1];

   int* prev = new int[n + 1];

   int max_j = 0;

   M[0] =-1;

   if (n > 0)
   {
      M[1] = 0;
      prev[0] = -1;
      max_j = 1;

      for (int i = 1; i < n; i++)
      {
          int j = upper_bound(X, M, 1, max_j, X[i]);

          if (j < 0)
          {
              max_j++;
              j = max_j;
          }
          prev[i] = M[j - 1];
          M[j] = i;
      }
   }
   backtrack_longest_sequence(X, prev, M[max_j]);
   printf("\n");

   delete []M;
   delete []prev;

   return max_j;
}

int find_longest_non_increase_sequence(int* X, int n)
{
    int* M = new int[n + 1];

    int * prev = new int[n + 1];

    int max_j = (n > 0 ? 1: 0);

    M[0] = -1;

    if (n > 0)
    {
        M[1] = 0;
        prev[0] = -1;
    }

    for (int i = 1; i < n; i++)
    {
         int j = lower_bound(X, M, 1, max_j, X[i]);
         if (j < 0)
         {
             max_j++;
             j = max_j;
         }
         prev[i] = M[j - 1];
         M[j] = i;
         //printf("M[%d]=%d\n", j, M[j]);
         //printf("prev[%d]=%d\n", i, prev[i]);
    }

    backtrack_longest_sequence(X, prev, M[max_j]);
    printf("\n");

    delete[] M;
    delete[] prev;

     return max_j;
}

int main() {
    //int X[] = {3, 5, 3, 8, 8, 8};
    //int X[] = {1};
    //int X[] = {1, 2, 3};
    //int X[] = {3, 2, 1};
    printf("%d\n", find_longest_non_decrease_sequence(X, sizeof(X) / sizeof(X[0])));
    printf("%d\n", find_longest_non_increase_sequence(X, sizeof(X) / sizeof(X[0])));
    return 0;
}
