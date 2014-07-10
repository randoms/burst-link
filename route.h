#include <jansson.h>
#include "handler.h"
#include "toxcore/tox.h"

void route(const char * cmds, Tox *m, int sock);