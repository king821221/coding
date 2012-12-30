#include<stdio.h>
#include<assert.h>

/*
* To prevent (l+r) overflow 
*/
inline int midpoint(int l, int r)
{
   assert(l<=r);
   return l + (r -l)/2;
}

inline int midpoint2(int l, int r)
{
   assert(l<=r);
   return r - (r-l)/2;
}

/*
 * A[i]=k=A[i+1]..<A[i+k], l <= i <= r
 * The first A[i] that equals to k
 */
int binary_search_1st_equal_inclusive(int* A, int l, int r, int k)
{
   while(l<r)
   {
      int m = midpoint(l, r);
      if(A[m] < k)
      {
         l = m + 1;
      }
      else
      {
         r = m;
      }
   }
   if(l == r && l >= 0 && A[l] == k)
   {
      return l;
   }
   return -1; 
}

void test_binary_search_1st_equal_inclusive()
{
   int A[] = {0, 2, 3, 3, 3, 5, 6, 6};
   assert(binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 3) == 2);
   assert(binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 2) == 1);
   assert(binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 6) == 6);
   assert(binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 0) == 0);
   assert(binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 4) == -1);

   int B[] = {0};
   assert(binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 0) == 0);
   assert(binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 1) == -1);
}

/*
 * A[i]=k=A[i+1]..<A[i+k], l < i < r
 * The first A[i] that equals to k
 */
int binary_search_1st_equal_exclusive(int* A, int l, int r, int k)
{
   while(l + 1 < r)
   {
      int m = midpoint(l, r);
      if(A[m] < k)
      {
         l = m;
      }
      else
      {
         r = m;
      }
   }
   if(l + 1 == r && l >= -1 && A[l+1] == k)
   {
      return l;
   }
   return -1; 
}

/*
 * A[i-k]=..k=A[i], l <= i <= r
 * The last A[i] that equals to k
 */
int binary_search_last_equal_inclusive(int* A, int l, int r, int k)
{
   while(l<r)
   {
      int m = midpoint2(l, r);
      //printf("l: %d, r: %d, m: %d, A[%d]=%d\n", l, r, m, m, A[m]);
      if(A[m] > k)
      {
         r = m - 1;
      }
      else
      {
         l = m;
      }
   }
   if(l == r && l >= 0 && A[l] == k)
   {
      return l;
   }
   return -1; 
}

void test_binary_search_last_equal_inclusive()
{
   int A[] = {0, 2, 3, 3, 3, 5, 6, 6};
   assert(binary_search_last_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 3) == 4);
   assert(binary_search_last_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 2) == 1);
   assert(binary_search_last_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 6) == 7);
   assert(binary_search_last_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 0) == 0);
   assert(binary_search_last_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 4) == -1);

   int B[] = {0, 1, 1};
   assert(binary_search_last_equal_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 0) == 0);
   assert(binary_search_last_equal_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 1) == 2);
}

void test_find_all_equal()
{
   int A[] = {0, 2, 3, 3, 3, 5, 6, 6};
   int s = binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 3);
   int e = s;
   if (s >= 0)
   {
      e = binary_search_last_equal_inclusive(A, s, sizeof(A)/sizeof(int) - 1, 3);
   }
   assert(s == 2 && e == 4); 
   s = binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 6);
   e = s;
   if (s >= 0)
   {
      e = binary_search_last_equal_inclusive(A, s, sizeof(A)/sizeof(int) - 1, 6);
   }
   assert(s == 6 && e == 7);
   s = binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 5);
   e = s;
   if (s >= 0)
   {
      e = binary_search_last_equal_inclusive(A, s, sizeof(A)/sizeof(int) - 1, 5);
   }
   assert(s == 5 && e == 5);
   s = binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 0);
   e = s;
   if (s >= 0)
   {
      e = binary_search_last_equal_inclusive(A, s, sizeof(A)/sizeof(int) - 1, 0);
   }
   assert(s == 0 && e == 0);
   s = binary_search_1st_equal_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 4);
   e = s;
   if (s >= 0)
   {
      e = binary_search_last_equal_inclusive(A, s, sizeof(A)/sizeof(int) - 1, 4);
   }
   assert(s == -1 && e == -1);
 
}

/*
 * A[i-k]=..k=A[i], l < i < r
 * The last A[i] that equals to k
 */
int binary_search_last_equal_exclusive(int* A, int l, int r, int k)
{
   while(l+1<r)
   {
      int m = midpoint(l, r);
      if(A[m] > k)
      {
         r = m;
      }
      else
      {
         l = m;
      }
   }
   if(l == r && l >= 0 && A[l] == k)
   {
      return l;
   }
   return -1; 
}

/*
 * A[i] <= k < A[i+1], the smallest i, l - 1 <= i <= r
 * 
*/
int binary_search_1st_noless_inclusive(int* A, int l, int r, int k)
{
   while(l <= r)
   {
      int m = midpoint(l, r);
      if(A[m] <= k) 
      {
         l = m + 1;
      }
      else
      {
         r = m - 1;
      }
   }
   return r;
}

void test_binary_search_1st_noless_inclusive()
{
   int A[] = {0, 2, 3, 3, 3, 5, 6, 6};
   assert(binary_search_1st_noless_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 3) == 4);
   assert(binary_search_1st_noless_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 2) == 1);
   assert(binary_search_1st_noless_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 6) == 7);
   assert(binary_search_1st_noless_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 0) == 0);
   assert(binary_search_1st_noless_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 4) == 4);
   assert(binary_search_1st_noless_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, -1) == -1);
   assert(binary_search_1st_noless_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 7) == 7);
   
   int B[] = {1, 2};
   assert(binary_search_1st_noless_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 0) == -1);
   assert(binary_search_1st_noless_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 1) == 0);
   assert(binary_search_1st_noless_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 2) == 1);
   assert(binary_search_1st_noless_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 3) == 1);
}

/*
 * A[i] <= A[i+1], l < i < r
 * A[i] <= k < A[i+1], the smallest i (l,r)
 * return i
 * 
*/
int binary_search_1st_noless_exclusive(int* A, int l, int r, int k)
{
   while(l+1<r)
   {
      int m = midpoint(l, r);
      if(A[m] <= k) 
      {
         l = m;
      }
      else
      {
         r = m;
      }
   }
   return l;
}

void test_binary_search_1st_noless_exclusive()
{
   int A[] = {0, 2, 3, 3, 3, 5, 6, 6};
   assert(binary_search_1st_noless_exclusive(A, -1, sizeof(A)/sizeof(int), 3) == 4);
   assert(binary_search_1st_noless_exclusive(A, -1, sizeof(A)/sizeof(int), 2) == 1);
   assert(binary_search_1st_noless_exclusive(A, -1, sizeof(A)/sizeof(int), 6) == 7);
   assert(binary_search_1st_noless_exclusive(A, -1, sizeof(A)/sizeof(int), 0) == 0);
   assert(binary_search_1st_noless_exclusive(A, -1, sizeof(A)/sizeof(int), 4) == 4);
   assert(binary_search_1st_noless_exclusive(A, -1, sizeof(A)/sizeof(int), -1) == -1);
   assert(binary_search_1st_noless_exclusive(A, -1, sizeof(A)/sizeof(int), 7) == 7);
   
   int B[] = {1, 2};
   assert(binary_search_1st_noless_exclusive(B, -1, sizeof(B)/sizeof(int), 0) == -1);
   assert(binary_search_1st_noless_exclusive(B, -1, sizeof(B)/sizeof(int), 1) == 0);
   assert(binary_search_1st_noless_exclusive(B, -1, sizeof(B)/sizeof(int), 2) == 1);
   assert(binary_search_1st_noless_exclusive(B, -1, sizeof(B)/sizeof(int), 3) == 1);
}

/*
 * A[i] < k <= A[i+1], the smallest i, l - 1 <= i <= r
 *
*/
int binary_search_1st_greater_inclusive(int* A, int l, int r, int k)
{
   while(l <= r)
   {
      int m = midpoint(l, r);
      if(A[m] < k)
      {
         l = m + 1;
      }
      else
      {
         r = m - 1;
      }
   }
   return r;
}

void test_binary_search_1st_greater_inclusive()
{
   int A[] = {0, 2, 3, 3, 3, 5, 6, 6};
   assert(binary_search_1st_greater_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 3) == 1);
   assert(binary_search_1st_greater_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 2) == 0);
   assert(binary_search_1st_greater_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 6) == 5);
   assert(binary_search_1st_greater_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 0) == -1);
   assert(binary_search_1st_greater_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 4) == 4);
   assert(binary_search_1st_greater_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, -1) == -1);
   assert(binary_search_1st_greater_inclusive(A, 0, sizeof(A)/sizeof(int) - 1, 7) == 7);

   int B[] = {1, 1, 2};
   assert(binary_search_1st_greater_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 0) == -1);
   assert(binary_search_1st_greater_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 1) == -1);
   assert(binary_search_1st_greater_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 2) == 1);
   assert(binary_search_1st_greater_inclusive(B, 0, sizeof(B)/sizeof(int) - 1, 3) == 2);
}

/*
 * A[i] < k <= A[i+1], the smallest i, l < i < r
 *
*/
int binary_search_1st_greater_exclusive(int* A, int l, int r, int k)
{
   while(l + 1 < r)
   {
      int m = midpoint(l, r);
      if(A[m] < k)
      {
         l = m;
      }
      else
      {
         r = m;
      }
   }
   return l;
}

void test_binary_search_1st_greater_exclusive()
{
   int A[] = {0, 2, 3, 3, 3, 5, 6, 6};
   assert(binary_search_1st_greater_exclusive(A, -1, sizeof(A)/sizeof(int), 3) == 1);
   assert(binary_search_1st_greater_exclusive(A, -1, sizeof(A)/sizeof(int), 2) == 0);
   assert(binary_search_1st_greater_exclusive(A, -1, sizeof(A)/sizeof(int), 6) == 5);
   assert(binary_search_1st_greater_exclusive(A, -1, sizeof(A)/sizeof(int), 0) == -1);
   assert(binary_search_1st_greater_exclusive(A, -1, sizeof(A)/sizeof(int), 4) == 4);
   assert(binary_search_1st_greater_exclusive(A, -1, sizeof(A)/sizeof(int), -1) == -1);
   assert(binary_search_1st_greater_exclusive(A, -1, sizeof(A)/sizeof(int), 7) == 7);

   int B[] = {1, 1, 2};
   assert(binary_search_1st_greater_exclusive(B, -1, sizeof(B)/sizeof(int), 0) == -1);
   assert(binary_search_1st_greater_exclusive(B, -1, sizeof(B)/sizeof(int), 1) == -1);
   assert(binary_search_1st_greater_exclusive(B, -1, sizeof(B)/sizeof(int), 2) == 1);
   assert(binary_search_1st_greater_exclusive(B, -1, sizeof(B)/sizeof(int), 3) == 2);
}

int main(int argc, char** argv)
{
   test_binary_search_1st_equal_inclusive();
   test_binary_search_last_equal_inclusive();
   test_find_all_equal();
   test_binary_search_1st_noless_inclusive();
   test_binary_search_1st_noless_exclusive();
   test_binary_search_1st_greater_inclusive();
   test_binary_search_1st_greater_exclusive();
}
