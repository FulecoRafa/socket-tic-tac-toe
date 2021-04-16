#ifndef _HELPER_MACROS_TICTACTOE_
#define _HELPER_MACROS_TICTACTOE_

#include <stdio.h>

// Macros for easier programing
#define lambda(l_ret_type, l_arguments, l_body)        \
  ({                                                   \
   l_ret_type l_anonymous_functions_name l_arguments   \
   l_body                                              \
   &l_anonymous_functions_name;                        \
   })

#define forEachInArray(fe_arrType, fe_arr, fe_lambda)             \
  {                                                               \
    for (int i = 0; i < sizeof(fe_arr) / sizeof(fe_arrType); i++) \
    {                                                             \
      fe_lambda(&fe_arr[i]);                                     \
    }                                                             \
  }

#define eprintf(format, ...) \
  fprintf(stderr, "\n[\033[31m✘\033[0m]\t" format, ##__VA_ARGS__)

typedef enum {
  false,
  true
} bool;

#endif
