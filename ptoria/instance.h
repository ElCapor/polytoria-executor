#ifndef INSTANCE_H
#define INSTANCE_H


#include <unity/unity.h>
#include <unity/object.h>

struct InstanceBase {
    UnityString* Name();
    UnityArray<InstanceBase*>* Children();
    UnityString* FullName();
    void CmdClicked();
};

struct Instance : public InstanceBase,
                  public Object<Instance, "Instance", Unity::AssemblyCSharp> {

        

};

#endif /* INSTANCE */
