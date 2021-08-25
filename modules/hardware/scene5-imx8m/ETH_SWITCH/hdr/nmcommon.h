#include <stdbool.h>
#include <NetworkManager.h>

extern NMClient* getClient();

/*Function to runcommand and get the ouput*/
bool runcommand(const char *cmd, char *output, int size);
