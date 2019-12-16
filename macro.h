#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>

#define TEST(val)           if((val)  <    0) { return;     }
#define PTEST(val)          if((val) == NULL) { return;     }
#define RTEST(val, ret)     if((val)  <    0) { return ret; }
#define RPTEST(val, ret)    if((val) == NULL) { return ret; }

#define ERTEST(val, ret)\
    if((val) < 0)\
    {\
        printf("error - line %d in %s\n", __LINE__, __FILE__);\
        perror("\treason: ");\
        return ret; \
    }
#define ERPTEST(val, ret)\
    if((val) == NULL)\
    {\
        printf("error - line %d in %s\n", __LINE__, __FILE__);\
        perror("\treason: ");\
        return ret; \
    }

#endif /*MACRO_H*/
