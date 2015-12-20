#include <stdlib.h>

static void my_static_dummy_function0();
static void my_static_dummy_function1();

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
static char *macro_exclusion_test_global_static_my_str0 = "default not reachable\0";
static void (*macro_exclusion_test_global_static_my_func_ptr0) = &my_static_dummy_function0;
#else
static char *macro_exclusion_test_global_static_my_str = "default reachable\0";
static void (*macro_exclusion_test_global_static_my_func_ptr) = &my_static_dummy_function1;
#endif

/* declare global */
#ifndef MACRO_EXCLUSION_TEST_GLOBAL_SWITCH
char *macro_exclusion_test_global_my_str0 = "default not reachable\0";
void (*macro_exclusion_test_global_my_func_ptr0) = &my_static_dummy_function0;
#else
char *macro_exclusion_test_global_my_str1 = "default reachable\0";
void (*macro_exclusion_test_global_my_func_ptr1) = &my_static_dummy_function1;
#endif

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
#ifdef MACRO_EXCLUSION_TEST_GLOBAL_STATIC_SWITCH
  //TODO:JK: implement me
#else
  //TODO:JK: implement me
#endif
  
  return(0);
}

/* tests global memory */
int
do_global_memory_test()
{
#ifdef MACRO_EXCLUSION_TEST_GLOBAL_SWITCH
  //TODO:JK: implement me
#else
  //TODO:JK: implement me
#endif
  
  return(0);
}

/* tests static memory */
int
do_static_memory_test()
{
#ifdef MACRO_EXCLUSION_TEST_STATIC_SWITCH
  //TODO:JK: implement me
#else
  //TODO:JK: implement me
#endif
  
  return(0);
}

/* tests local memory */
int
do_local_memory_test()
{
#ifdef MACRO_EXCLUSION_TEST_LOCAL_SWITCH
  //TODO:JK: implement me
#else
  //TODO:JK: implement me
#endif
  
  return(0);
}

/* tests automatic memory */
int
do_automatic_memory_test()
{
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
