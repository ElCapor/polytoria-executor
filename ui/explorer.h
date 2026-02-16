#ifndef EXPLORER_H
#define EXPLORER_H

#include <ptoria/instance.h>

class ExplorerUI
{
public:
    static void Init();
    static void DrawTab();    

public:
    static Instance* selectedInstance;
};


#endif /* EXPLORER_H */
