#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef struct item
{
   const void* data;
   double weight;
   bool sampled;

   item()
   {
     this->data = NULL;
     this->weight = 0;
     this->sampled = false;
   }
   item(const struct item& other)
   {
     data = other.data;
     weight = other.weight;
     sampled = other.sampled;
   }
   item(const void* data, double weight)
   {
      this->data = data; //referece to data
      this->weight = weight;
      sampled = false;
   }
   struct item& operator =(const struct item& other)
   {
      data = other.data;
      weight = other.weight;
      sampled = other.sampled;
      return *this;
   }
} item;

inline void swap(int& a, int& b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

static int select_next_sample(int* population_ref, const item* populations, int start, int end, double weight_sum, double percent);

void sample_by_weight(item* populations,
                 int population_size,
                 int sample_cnt)
{
   if (sample_cnt >= population_size)
   {
      for (int i = 0; i < population_size; i++)
      {
          populations[i].sampled = true;
      }
      return;
   }

   double weight_sum = 0;
   int* population_ref = new int[population_size];
   for (int i = 0; i < population_size; i++)
   {
       populations[i].sampled = false;
       population_ref[i] = i;
       weight_sum += populations[i].weight;
   }
   int start = 0;
   srand(time(0));
   while (sample_cnt > 0)
   {
       double percent = ((double)rand()) / RAND_MAX;
       int selected = select_next_sample(population_ref, populations, start, population_size, weight_sum, percent);
       if (selected == population_size)
       {
           selected = (int)((population_size - start) * percent + start);
       }
       //printf("selected: %d, start: %d, sample_cnt: %d, weight: %f\n", selected, start, sample_cnt, populations[population_ref[selected]].weight);
       populations[population_ref[selected]].sampled = true;
       swap(population_ref[start], population_ref[selected]);
       weight_sum -= populations[population_ref[start]].weight;
       start++;
       sample_cnt--;
   }
   delete[] population_ref;
}

static int select_next_sample(int* population_ref, const item* populations, int start, int end, double weight_sum, double percent)
{
   double cumulative_weight_sum = 0;
   while(start < end)
   {
       cumulative_weight_sum += populations[population_ref[start]].weight;
       //printf("start: %d, total_weight: %f, cumulative_weight_sum: %f, weight: %f, percent: %f\n",
         //start, weight_sum, cumulative_weight_sum, populations[population_ref[start]].weight, percent);
       if (cumulative_weight_sum > weight_sum * percent)
       {
           return start;
       }
       start++;
   }
   return end;
}

inline void swap(item& a, item& b)
{
   item tmp = a;
   a = b;
   b = tmp;
}

static void shuffle(item* items, int n)
{
   for (int i = 0; i < n - 1; i++)
   {
       int j = (int)((n - 1 - i) * (((double)rand()) / RAND_MAX) + i);
       if (i != j)
       {
          swap(items[i], items[j]);
       }
   }
}

#define N 10
void experiment(struct item* items, int* acc, int n)
{
   shuffle(items, n);
   sample_by_weight(items, n, 5);
   for (int i = 0; i < n; i++)
   {
       if (items[i].sampled)
       {
          acc[(int)(items[i].weight)]++;
       }
   }
}
int main(int argc, char** argv)
{
   int data[N];
   int acc[N] = {0};
   for (int i = 0; i < N; i++)
   {
      data[i] = i;
   }
   item items[N];
   for (int i = 0; i < N; i++)
   {
      items[i].data = &data[i];
      items[i].weight = (double)i;
   }
   for (int i = 0; i < 10000; i++)
   {
       experiment(items, acc, N);
   }
   for (int i = 0; i < N; i++)
   {
       printf("%d\t%d\n", i, acc[i]);
   }
}
