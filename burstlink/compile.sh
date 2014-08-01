cd burstlink
gcc -o burstlink  server.c connect.c queue.c local_socks.c message_listener.c utils/utils.c -ljansson -luuid -lpthread -ltoxcore
mv burstlink ../Release/

cd ../burstlinkser
gcc -o burstlinkser client.c connect.c queue.c local_socks.c message_listener.c utils/utils.c -ljansson -luuid -lpthread -ltoxcore
mv burstlinkser ../Release/test/
