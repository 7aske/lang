//
// Created by nik on 2/10/22.
//

#ifndef LANG_UTIL_H
#define LANG_UTIL_H

#define REPORT_LINE(__Str, ...) \
fprintf(stderr, __Str" %s:%d:1\n", ##__VA_ARGS__, __FILE__, __LINE__)

#endif //LANG_UTIL_H
