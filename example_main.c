#include <stdio.h>
#include "ezsmt.h"

int main() {
    //char p[] = " (]. ";
    char p[] = "2 { f(1..4) } 2.";
    //char p[] = "a.  b :- a.";
    void* ez = ezsmt_mk();
    char* out = ezsmt_clausify(ez, p);
    if (out)
       printf("%s\n", out);
    else {
       char *err = ezsmt_get_error(ez);
       printf("%s\n", err? err:"UNKNOWN");
    }
    ezsmt_del(ez);
    return  0;
}

