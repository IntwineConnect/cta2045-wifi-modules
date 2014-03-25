#include "HardwareProfile.h"

#if defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)

#include "LOG_Manager.h"

void PlugStripSetRelay(UINT8 state, LogWhodunnit_e who, char *extra);
void PlugStripRestoreState(void);
void PlugStripOneTimeSetup(void);

#endif //#if defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)

