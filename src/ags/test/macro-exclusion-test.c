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
#ifdef AGS_GCC_TEST_EXCLUSION_MACRO_GLOBAL_STATIC_SWITCH
static char *ags_gcc_test_exclusion_macros_global_static_my_str = "default not reachable\0";
static void (*ags_gcc_test_exclusion_macros_global_static_my_func_ptr) = &my_static_dummy_function0;
#else
static char *ags_gcc_test_exclusion_macros_global_static_my_str = "default reachable\0";
static void (*ags_gcc_test_exclusion_macros_global_static_my_func_ptr) = &my_static_dummy_function1;
#endif

/* declare global */
#ifndef AGS_GCC_TEST_EXCLUSION_MACRO_GLOBAL_SWITCH
char *ags_gcc_test_exclusion_macros_global_static_my_str = "default not reachable\0";
void (*ags_gcc_test_exclusion_macros_global_static_my_func_ptr) = &my_static_dummy_function0;
#else
char *ags_gcc_test_exclusion_macros_global_static_my_str = "default reachable\0";
void (*ags_gcc_test_exclusion_macros_global_static_my_func_ptr) = &my_static_dummy_function1;
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
#ifdef AGS_GCC_TEST_EXCLUSION_MACRO_GLOBAL_STATIC_SWITCH
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
#ifdef AGS_GCC_TEST_EXCLUSION_MACRO_GLOBAL_SWITCH
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
#ifdef AGS_GCC_TEST_EXCLUSION_MACRO_STATIC_SWITCH
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
#ifdef AGS_GCC_TEST_EXCLUSION_MACRO_LOCAL_SWITCH
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
#ifdef AGS_GCC_TEST_EXCLUSION_MACRO_AUTOMATIC_SWITCH
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
