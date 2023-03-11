#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h> // wait()
#include <unistd.h> // fork()

#define T 1

#if T
typedef double type;
#else
typedef float type;
#endif

const type eps = 1e-14;

type exp(type point)
{
   type elem = 1;
   type e = 1;

   for (size_t i = 1; fabs(elem) > eps; ++i)
   {
      elem *= point / i;
      e += elem;
   }
   return e;
}

type func(type point, FILE** file)
{
   pid_t ProcessID1;
   pid_t ProcessID2;

   fpos_t pos;

   fgetpos(*file, &pos);

   // Порождаем первый дочерний процесс
   ProcessID1 = fork();

   // Если возникла ошибка при порождении первого процесса-потомка
   if (ProcessID1 == -1)
   {
      perror("1:fork()");
      exit(EXIT_FAILURE);
   }

   // Если это процесс-предок, породить второй дочерний процесс от предка
   if (ProcessID1 != 0)
   {
      ProcessID2 = fork();
      
      // Если возникла ошибка при порождении второго процесса-потомка
      if (ProcessID2 == -1)
      {
         perror("2:fork()");
         exit(EXIT_FAILURE);
      }
   }

   if (ProcessID1 == 0)
   {
      type e1 = exp(point);
      fwrite(&e1, sizeof(type), 1, *file);
      exit(EXIT_SUCCESS);
   }

   if (ProcessID2 == 0)
   {
      type e2 = -exp(-point);
      fwrite(&e2, sizeof(type), 1, *file);
      exit(EXIT_SUCCESS);
   }

   int t;

   if (ProcessID1 != 0)
   {
      wait(&t);
   }

   if (ProcessID2 != 0)
   {
      wait(&t);
   }

   type e1,e2;
   
   fsetpos(*file, &pos);

   fread(&e1, sizeof(type), 1, *file);
   fread(&e2, sizeof(type), 1, *file);

   return e1 + e2;
}

type TrapezoidMethod(const type a, const type b, const int k)
{
   FILE* TemporaryFile = tmpfile();
  
   if (TemporaryFile == NULL)
   {
      perror("tmpfile:");
      exit(EXIT_FAILURE);
   }

   const type h = (b - a) / k;

   type point = a;
   type result = 0;
  
   for (size_t i = 0; i < k; ++i)
   {
      result += func(point, &TemporaryFile) + func(point += h, &TemporaryFile);
   }

   fclose(TemporaryFile);

   return result * h / 4;
}

int main(int numberOfArguments, char* arguments[])
{
   if (numberOfArguments < 4)
   {
      fprintf(stderr,"Arguments too few (There are no boundaries of the interval or k)\n");
      return EXIT_FAILURE;
   }
   
   type a, b;
   int k;

   if (sscanf(arguments[1], "%lf", &a) == 0 ||
       sscanf(arguments[2], "%lf", &b) == 0 || 
       sscanf(arguments[3], "%d", &k) == 0)
   {
      fprintf(stderr,"Argument exception -> ");
      perror("sscanf()");
      return EXIT_FAILURE;
   }

   type answer = TrapezoidMethod(a,b,k);
   printf("%.15lf\n", answer);

   return EXIT_SUCCESS;
}
