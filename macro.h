#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>

#define TEST(val)           if((val)  <    0) { return;     }
#define PTEST(val)          if((val) == NULL) { return;     }
#define RTEST(val, ret)     if((val)  <    0) { return ret; }
#define PRTEST(val, ret)    if((val) == NULL) { return ret; }

#define ERETEST(val, ret)\
    if((val) < 0)\
    {\
        printf("error - line %d in %s\n", __LINE__, __FILE__);\
        perror("\treason: ");\
        return ret; \
    }
#define PRETEST(val, ret)\
    if((val) == NULL)\
    {\
        printf("error - line %d in %s\n", __LINE__, __FILE__);\
        perror("\treason: ");\
        return ret; \
    }

#endif /*MACRO_H*/
