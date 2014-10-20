/* 
 * File:   main.c
 * Author: des
 *
 * Created on September 29, 2014, 6:22 PM
 */
#include<stdio.h>
#include<time.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/types.h>       /* need to calculate which I will implement later */

typedef signed long long int fibval; /*just to make everything less cluttered*/

void *fibr (void *n);
void *fibr_1 (void *k);
fibval fibonacci (fibval);

typedef struct _param_basket{
    fibval in_param; /* Will hold value to be operated in thread */
    fibval out_param; /* Will hold result of operation in thread */
}param_basket;

int main ()
{
/* Commented to remove warnings
  clock_t begin, end;
  double time_spent;
*/
  pthread_t tid, tid1;
  int result, result1;
  fibval n = 6;

  param_basket b1, b2;
  b1.in_param = n-1; /* This will contain fib(n-1) eventually*/
  b1.out_param = 0;

  b2.in_param = n-2;
  b2.out_param = 0; /* This will contain fib(n-2) eventually*/

  result = pthread_create (&tid, NULL, fibr, &b1);
  if (result)
    {
      perror ("pthread_create");
      return EXIT_FAILURE;
    }
  result1 = pthread_create (&tid1, NULL, fibr, &b2);
  if (result1)
    {
      perror ("pthread_create");
      return EXIT_FAILURE;
    }
  if (pthread_join (tid, NULL))
    {
      perror ("pthread_join");
      return EXIT_FAILURE;
    }
  if (pthread_join (tid1, NULL))
    {
      perror ("pthread_join");
      return EXIT_FAILURE;
    }

 /* fib(n) = fib(n-1) + fib(n-2)*/
  printf ("Fib value (%lld) =%lld\n",n, b1.out_param + b2.out_param);

  pthread_exit (NULL);

  return EXIT_SUCCESS;
}

void *
fibr (void *n) /* will receive n-1 & n-2 */
{
  param_basket * b = (param_basket *)n;

  pthread_t tid2, tid3;

  /* How do I assign values to i , j in order to
 achieve the level viz fib(n-2)....fib(n-4) */
  param_basket b3, b4;
  b3.in_param = b->in_param-1; /* n-2 when n-1 , n-3 when n-2 */
  b3.out_param = 0; /* This will contain fib(n-2) & fib(n-3) eventually in diff threads*/
  b4.in_param = b->in_param-2; /* n-3 when n, n-4 when n-2 */
  b4.out_param = 0; /* This will contain fib(n-3) & fib(n-4) eventually in diff threads*/

  if (pthread_create (&tid2, NULL, fibr_1, &b3))
    {
      perror ("pthread_create");
      return NULL;
    }

  if (pthread_create (&tid3, NULL, fibr_1, &b4))
    {
      perror ("pthread_create");
      return NULL;
    }
  if (pthread_join (tid2, NULL))
    {
      perror ("pthread_join");
      return NULL;
    }

  if (pthread_join (tid3, NULL))
    {
      perror ("pthread_join");
      return NULL;
    }
  /* How to return the values of i, j combined with *y . if I do *y+i+j, the result
     is not coming correctly */
  /* Just combine the outputs! */
  b->out_param = b3.out_param + b4.out_param;

  return NULL;
}

/* Second level thread ... no more wretched spawn! Direct calc*/
void *
fibr_1 (void *k)
{
  param_basket * b = (param_basket *)k;
  b->out_param = fibonacci(b->in_param);

  return NULL;
}

fibval
fibonacci (fibval x)
{
  if ((x == 0) || (x == 1))
    return x;
  return fibonacci (x - 1) + fibonacci (x - 2);
}