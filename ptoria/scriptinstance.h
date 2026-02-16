#ifndef SCRIPTINSTANCE
#define SCRIPTINSTANCE

#include <ptoria/basescript.h>


struct ScriptInstance : public BaseScriptBase,
                        public Object<ScriptInstance, "ScriptInstance", Unity::AssemblyCSharp> {};

#endif /* SCRIPTINSTANCE */
