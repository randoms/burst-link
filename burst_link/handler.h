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
void add(char *friend_info);