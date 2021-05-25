#ifndef _HELPER_MACROS_TICTACTOE_
#define _HELPER_MACROS_TICTACTOE_

#include <stdio.h>
#include <stdbool.h>

/*
 * Macros for easier programing
 * */
// GNU-C implementation for lambda functions
#define lambda(l_ret_type, l_arguments, l_body)        \
  ({                                                   \
   l_ret_type l_anonymous_functions_name l_arguments   \
   l_body                                              \
   &l_anonymous_functions_name;                        \
   })

// GNU-C implementation for lambda functions
#define forEachInArray(fe_arrType, fe_arr, fe_lambda)             \
  {                                                               \
    for (int i = 0; i < sizeof(fe_arr) / sizeof(fe_arrType); i++) \
    {                                                             \
      fe_lambda(&fe_arr[i]);                                     \
    }                                                             \
  }

// Easier error printing
#define eprintf(format, ...) \
  fprintf(stderr, "\n[\033[31m✘\033[0m]\t" format, ##__VA_ARGS__)

#define MAX(x,y) x>y?x:y
#define MIN(x,y) x>y?y:x

#endif
