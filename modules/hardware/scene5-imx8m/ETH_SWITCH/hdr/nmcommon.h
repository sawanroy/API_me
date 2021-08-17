#include <stdbool.h>
#include <NetworkManager.h>

extern NMClient* getClient();

bool runcommand(const char *cmd, char *output, int size);
void nm_add_connection(NMClient *client, GMainLoop *loop, const char *con_name, const char *ifname);