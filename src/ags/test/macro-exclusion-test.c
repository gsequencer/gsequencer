#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <math.h>

#define MY_DATA_LENGTH (8192)
#define MY_DUMMY_VAL0 (MY_DATA_LENGTH / 2)
#define MY_DUMMY_VAL1 (0xff)

static void my_static_dummy_function0();
static void my_static_dummy_function1();

struct _MyDummy{
  unsigned long val0;
  unsigned char val1;
};

double** my_data_function_v3f(int length);
void my_dummy_init(struct _MyDummy *my_dummy);

/* tests if global static memory is reachable within an exclusion macro */
int do_global_static_memory_test();

/* tests if global memory is reachable within an exclusion macro */
int do_global_memory_test();

/* tests if static memory is reachable within an exclusion macro */
int do_static_memory_test();

/* tests if local memory is reachable within an exclusion macro */
int do_local_memory_test();

/* tests if automatic memory is reachable within an exclusion macro */
int do_automatic_memory_test();

/* declare global static */
#ifdef MACRO_EXCLUSION_TEST_GLOBAL_STATIC_SWITCH
#define MACRO_EXCLUSION_TEST_GLOBAL_STATIC_MY_STR0_LENGTH (22)

static char *macro_exclusion_test_global_static_my_str0 = "default not reachable\0";
static void (*macro_exclusion_test_global_static_my_func_ptr0) = &my_static_dummy_function0;
#else
#define MACRO_EXCLUSION_TEST_GLOBAL_STATIC_MY_STR1_LENGTH (18)

static char *macro_exclusion_test_global_static_my_str1 = "default reachable\0";
static void (*macro_exclusion_test_global_static_my_func_ptr1) = &my_static_dummy_function1;
#endif

/* declare global */
#ifdef MACRO_EXCLUSION_TEST_GLOBAL_SWITCH
#define MACRO_EXCLUSION_TEST_GLOBAL_MY_STR0_LENGTH (22)

char *macro_exclusion_test_global_my_str0 = "default not reachable\0";
void (*macro_exclusion_test_global_my_func_ptr0) = &my_static_dummy_function0;
#else
#define MACRO_EXCLUSION_TEST_GLOBAL_MY_STR1_LENGTH (18)

char *macro_exclusion_test_global_my_str1 = "default reachable\0";
void (*macro_exclusion_test_global_my_func_ptr1) = &my_static_dummy_function1;
#endif

double **my_complex_i = NULL;

/* generate some data */
double**
my_data_function_v3f(int length)
{
  double **plot_v3f;

  double value;

  unsigned int i, i_stop;
  unsigned int j, j_stop;

  /* allocate vector array */
  plot_v3f = (double**) malloc(length * sizeof(double*));
  
  /* do a plot */
  i_stop =
    j_stop = length / 2;

  /* generate a sin curve quarter through -1|1 */
  for(i = 0; i < i_stop; i++){
    /* allocate vector */
    plot_v3f[i] = (double *) malloc(3 * sizeof(double));

    /* calculate value and fill */
    value = -1.0 * sin((i_stop) * (2.0 * M_PI) / ((double) i) / 4.0);

    plot_v3f[i][0] = -1.0 * value;
    plot_v3f[i][1] = value;
    plot_v3f[i][2] = value;
  }

  /* generate exponential curve */
  for(j = 0; j < j_stop; i++, j++){
    /* allocate vector */
    plot_v3f[i] = (double *) malloc(3 * sizeof(double));
    
    /* calculate value and fill */
    plot_v3f[i][0] = -1.0 * exp((1.0 / 3.0) * log((double) j));
    plot_v3f[i][1] = exp(3.0 * log((double) j));
    plot_v3f[i][2] = (double) j;
  }

  /* return allocated and filled buffer */
  return(plot_v3f);
}

void
my_dummy_init(struct _MyDummy *my_dummy)
{
  my_dummy->val0 = MY_DUMMY_VAL0;
  my_dummy->val1 = MY_DUMMY_VAL1;
}

/* just for checking address */
static void
my_static_dummy_function0()
{
  static const char **my_str = {
    "gcc\0",
    "first\0",
    "local\0",
    "variable\0",
  };
}

/* just for checking address */
static void
my_static_dummy_function1()
{
  static const char **my_str = {
    "gcc\0",
    "second\0",
    "local\0",
    "variable\0",
  };
}

/* tests global static memory */
int
do_global_static_memory_test()
{
  /* declare some variables */
#ifdef MACRO_EXCLUSION_TEST_GLOBAL_STATIC_SWITCH
  char *test_str0 = macro_exclusion_test_global_static_my_str0;
  void (*test_func_ptr0) = macro_exclusion_test_global_static_my_func_ptr0;
#else
  char *test_str1 = macro_exclusion_test_global_static_my_str1;
  void (*test_func_ptr1) = macro_exclusion_test_global_static_my_func_ptr1;
#endif

#ifdef MACRO_EXCLUSION_TEST_GLOBAL_STATIC_SWITCH
  /* check reachable */
  if(test_str0 != macro_exclusion_test_global_static_my_str0){
    printf("ERROR\n\0");
    return(-1);
  }

  if(strncmp(test_str0,
	     macro_exclusion_test_global_static_my_str0,
	     MACRO_EXCLUSION_TEST_GLOBAL_STATIC_MY_STR0_LENGTH)){
    printf("ERROR\n\0");
    return(-1);
  }
#else
  /* check reachable */
  if(test_str1 != macro_exclusion_test_global_static_my_str1){
    printf("ERROR\n\0");
    return(-1);
  }

  if(strncmp(test_str1,
	     macro_exclusion_test_global_static_my_str1,
	     MACRO_EXCLUSION_TEST_GLOBAL_STATIC_MY_STR1_LENGTH)){
    printf("ERROR\n\0");
    return(-1);
  }
#endif
  
  return(0);
}

/* tests global memory */
int
do_global_memory_test()
{
  if(my_complex_i[MY_DATA_LENGTH - 1][1] > 1.0){ /* y is definitely bigger if not MY_DATA_LENGTH < 2 */
#ifdef MACRO_EXCLUSION_TEST_GLOBAL_SWITCH
    printf("don't define MACRO_EXCLUSION_TEST_GLOBAL_SWITCH\n\0");

    return(-1);
#else
    char *test_str1 = macro_exclusion_test_global_my_str1;
    void (*test_func_ptr1) = macro_exclusion_test_global_my_func_ptr1;

    printf("global memory test\n\0");
  
    /* check reachable */
    if(test_str1 != macro_exclusion_test_global_my_str1){
      printf("ERROR\n\0");
      return(-1);
    }

    if(strncmp(test_str1,
	       macro_exclusion_test_global_my_str1,
	       MACRO_EXCLUSION_TEST_GLOBAL_MY_STR1_LENGTH)){
      printf("ERROR\n\0");
      return(-1);
    }
#endif
  }

  return(0);
}

/* tests static memory */
int
do_static_memory_test()
{
  if(my_complex_i[MY_DATA_LENGTH - 1][1] > 1.0){ /* y is definitely bigger if not MY_DATA_LENGTH < 2 */
#ifdef MACRO_EXCLUSION_TEST_STATIC_SWITCH
    printf("don't define MACRO_EXCLUSION_TEST_STATIC_SWITCH\n\0");

    return(-1);
#else
  //TODO:JK: implement me
#endif
  }
  
  return(0);
}

/* tests local memory */
int
do_local_memory_test()
{
  struct _MyDummy my_dummy_a, my_dummy_b;

#ifndef MACRO_EXCLUSION_TEST_LOCAL_SWITCH
  if(my_complex_i[MY_DATA_LENGTH - 1][1] > 1.0){ /* y is definitely bigger if not MY_DATA_LENGTH < 2 */
    my_dummy_init(&my_dummy_a);
  }
#endif
  
  if(my_complex_i[MY_DATA_LENGTH - 1][1] > 1.0){ /* y is definitely bigger if not MY_DATA_LENGTH < 2 */
#ifdef MACRO_EXCLUSION_TEST_LOCAL_SWITCH
    printf("don't define MACRO_EXCLUSION_TEST_LOCAL_SWITCH\n\0");

    return(-1);
#else
    long val0;
    static const long val1 = 1.0 / 1000.0;
    
    my_dummy_init(&my_dummy_b);

    if(my_dummy_a.val0 != MY_DUMMY_VAL0){
      printf("ERROR\n\0");
      return(-1);
    }

    if(my_dummy_a.val1 != MY_DUMMY_VAL1){
      printf("ERROR\n\0");
      return(-1);
    }

    if(my_dummy_b.val0 != MY_DUMMY_VAL0){
      printf("ERROR\n\0");
      return(-1);
    }

    if(my_dummy_b.val1 != MY_DUMMY_VAL1){
      printf("ERROR\n\0");
      return(-1);
    }

    val0 = my_dummy_a.val0 - my_dummy_b.val0;

    if(val0 != 0){
      printf("ERROR\n\0");
      return(-1);
    }
#endif
  }
  
  return(0);
}

/* tests automatic memory */
int
do_automatic_memory_test()
{
#ifdef MACRO_EXCLUSION_TEST_AUTOMATIC_SWITCH
    printf("don't define MACRO_EXCLUSION_TEST_AUTOMATIC_SWITCH\n\0");

    return(-1);
#else
  //TODO:JK: implement me
#endif

#ifdef MACRO_EXCLUSION_TEST_AUTOMATIC_SWITCH
  //TODO:JK: implement me
#else
  //TODO:JK: implement me
#endif

  return(0);
}

/* test entry */
int
main(int argc, char **argv)
{
  int retval;
  unsigned int failed_tests;

  failed_tests = 0;

  /* initialize data */
  my_complex_i = my_data_function_v3f(MY_DATA_LENGTH);
  
  /* test #1 */
  retval = do_global_static_memory_test();

  if(retval != 0){
    failed_tests++;
  }
  
  /* test #2 */
  retval = do_global_memory_test();

  if(retval != 0){
    failed_tests++;
  }
  
  /* test #3 */
  retval = do_static_memory_test();

  if(retval != 0){
    failed_tests++;
  }
  
  /* test #4 */
  retval = do_local_memory_test();

  if(retval != 0){
    failed_tests++;
  }
  
  /* test #5 */
  retval = do_automatic_memory_test();

  if(retval != 0){
    failed_tests++;
  }

  /* show result */
  //TODO:JK: implement me
  
  return(0);
}
