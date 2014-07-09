#define SLEEP_TIME 50000
#define BOOTSTRAP_ADDRESS "23.226.230.47"
#define BOOTSTRAP_PORT 33445
#define BOOTSTRAP_KEY "A09162D68618E742FFBCA1C2C70385E6679604B2D80EA6E84AD0996A1AC8A074"

#define MY_NAME "ImoutoBot"

/**
 * call this when new message arrives
 */
void on_message(char *msg);


/**
 * call this when want to send message
 */
void send_message(char *msg);

/**
 * call to connect to server
 */
void connect(char *target_info);


/**
 * call to add a friend
 */
int friend_add(const char *friendID);
int friend_accept(char *friendID);