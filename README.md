burst-link
==========


####基本功能

一个端口映射程序。能够把任意电脑的端口映射到本地端口。比如你可以把局域网中电脑的22端口映射到本地。
然后你就可以直接用ssh连接本地端口来控制局域网中的电脑。从此局域网再也不能阻拦我们。
这个程序是p2p的。所以在速度上相对于其他软件要有优势。你也不需要一个公网IP来建立你的p2p连接，一切
都已解决。

####实现原理
这个软件是建立在一个p2p的通信软件之上的[Tox](https://github.com/irungentoo/toxcore "Tox")之上的
在本地和远程分别建立sock，然后sock收到的信息通过tox来进行传递。这样看起来就像直接连接到了远程的sock一样。


####编译安装

1.toxcore
首先你要安装toxcore，具体的安装方法tox项目里面很详细

2.jansson

    wget http://www.digip.org/jansson/releases/jansson-2.6.tar.gz
    tar -xzvf jansson-2.6.tar.gz
    cd jansson-2.6.tar.gz
    ./configure
    make
    sudo make install
    
3.uuid
    具体的安装可能和系统相关，下面只说明ubuntu的安装
    sudo apt-get install uuid-dev
    
4.编译主程序
    cd 到主文件夹下
    
    编译本地程序
    gcc -o server server.c handler.c route.c connect.c message_listener.c queue.c utils/utils.c local_socks.c -ljansson -ltoxcore -lpthread -luuid
    
    编译远程用程序
    cd 到test文件夹下
    gcc -o client client.c handler.c route.c connect.c message_listener.c queue.c utils/utils.c local_socks.c -ljansson -ltoxcore -lpthread -luuid
    
    
####使用方法

    先从比较简单的来
    网络环境
    
    A------>route A -------->route B-------->B
    
    route B是B的路由，B处在自己的局域网之中，同样A也在自己的局域网之中。现在A想要连接B的22端口
    首先在B电脑上
    cd 到test文件夹下
    ./client
    看到提示toxcore online，就说明已经成功启动了。同时还会显示MYID：××××,记录下这个ID.
    接着在A电脑上
    cd 到主文件夹下
    ./server 9990 刚才的ID 127.0.0.1 22
    这时会显示listening on 9990,说明已经启动正常了。下面只要等待出现 connect:OK,就表示成功连接了。
    这个过程可能会有些慢，最长可能一分钟左右。
    
    参数说明
        ./server 本地端口 ID 远程IP 远程端口
        本地端口就是要映射到的本地端口。远程IP是远程电脑要连接的IP，这里B是要连接自己的22端口，所以是22
        远程端口就是要映射的远程端口
        
    开始连接
        ssh -p 9990 username@localhost
        username 是你在B电脑上的用户名。这时就能成功连接了
        
        
####目前的问题
    目前还有很多问题，比如文件传输总是失败，安全问题，只能支持TCP连接。同时连接太多的时候会卡死。
    
        
