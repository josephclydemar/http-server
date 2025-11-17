#ifndef PROFILING_H_
#define PROFILING_H_

#include "logging.h"

#if NOPROFL
    #define PROFL_STOP(_component, _expr, ...) ((void)(_expr))
#else
    // runtime checking
    #define PROFL_STOP(_component, _expr, ...) do {                            \
        if (!(_expr)) {                                                        \
            printf("\n");                                                      \
            PROFL_LOG(                                                         \
                _component,                                                    \
                "\n"                                                           \
                MAGENTA_COLOR_TEXT "[Profiling `%s` finished.] %s" RESET_TEXT   \
                DEFAULT_COLOR_TEXT "\n\n",                                     \
                #_expr,                                                        \
                " " #__VA_ARGS__);                                             \
            exit(0);                                                           \
        }                                                                      \
    } while(0)
#endif


#endif // PROFILING_H_


