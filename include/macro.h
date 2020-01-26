/**
 *  \file
 */

#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>

#define TEST(val)        do{ if((val)  <    0) { return;     } }while(0) 
#define PTEST(val)       do{ if((val) == NULL) { return;     } }while(0) 
#define RTEST(val, ret)  do{ if((val)  <    0) { return ret; } }while(0) 
#define RPTEST(val, ret) do{ if((val) == NULL) { return ret; } }while(0) 

#define ERTEST(val, ret)                                      \
  do{                                                         \
    if((val) < 0)                                             \
    {                                                         \
      printf("error - line %d in %s\n", __LINE__, __FILE__);  \
      perror("\treason: ");                                   \
      return ret;                                             \
    }                                                         \
  }while(0)

#define ERPTEST(val, ret)                                     \
  do{                                                         \
    if((val) == NULL)                                         \
    {                                                         \
      printf("error - line %d in %s\n", __LINE__, __FILE__);  \
      perror("\treason: ");                                   \
      return ret;                                             \
    }                                                         \
  }while(0)

#endif /*MACRO_H*/
