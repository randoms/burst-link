Burstlink
==========

***BurstLink*** is a socket proxy. It can bind other computer's port to local port. You can directly access your local port to communicate with other computers. This software is based on [Tox](https://github.com/irungentoo/toxcore "toxcore") project

So why it is useful?
You can use it to play LAN games with your friends.
SSH to your office computer from home, and many many things that used to be possible only in LANs.
Burstlink is unstable right now, and cannot be used in production environment. If you want a stable version, [SharpLink](https://github.com/randoms/SharpLink) is a better choice. It's almost the same with burstlink but scripted in C#.
##Compile
###Linux
The following guide is based on ubuntu 14.04. You may have to adjust it to fit your own OS.

First you have to compile and install toxcore. It is described in detail on tox main page. Tox https://github.com/irungentoo/toxcore  
**Be careful, toxcore has changed a lot. This program only works with old versions of toxcore. You have to run the following command after cloning toxcore.**
``` git checkout 909db029412ddbd499281711a9cd46c3df4fe5a0```

Then you have to install jansson

    wget http://www.digip.org/jansson/releases/jansson-2.6.tar.gz
    tar -xzvf jansson-2.6.tar.gz
    cd jansson-2.6/
    ./configure
    make
    sudo make install
    
And then uuid-dev

    sudo apt-get install uuid-dev

Finally compile this program

    git clone https://github.com/randoms/burst-link
    cd burst-link/burstlink/
    ./compile.sh

The results are generated in burst-link/burstlink/Release/.
burstlink is the client and burstlinkser is the server.

###Windows
clone this project

    git clone https://github.com/randoms/burst-link
    
If you simply want to use it, you do not need to compile it. It is already compiled in burst-link/burstlink/Release/. burstlink.exe is the client and burstlinkser.exe is the server. Be careful that the .dll files in Release folder is also needed.

How to compile
open burstlink.sln in the burstlink folder with visual studio. You can compile it with visual studio.

##Usage
For example if computer B wants to connect to computer A's port 3128. But computer A and computer B are not in the same LAN.
A is connected to the internet via a router.
First run server program on computer A

    burstlinkser
    
    terminal output would be
    MYID:A5F437F9014941214300912F3E66CBB6F42B4CB0A354B187332552C7C500706C40C249A2823C

record the long string after MYID:. This is the ID of computer A. We will use it later.

run the following cmd on computer B

    burstlink 9990 A5F437F9014941214300912F3E66CBB6F42B4CB0A354B187332552C7C500706C40C249A2823C 127.0.0.1 3128
    
the long string above is computer A's ID. 9990 is computer B's local port. 127.0.0.1 is the ip address computer B want to connect with via computer A. 3128 is the port computer B want to connect.

    If connected successfully, terminal output would be
    
    TOXCORE:ONLINE
    SERVER:LISTEN ON 9990
    online
    MESSAGE RECEIVED
    HANDSHAKE RECEIVED
    402
    CONNECT:OK

You can now connect to you local port 9990, it feels as if you have connected to computer A's port 3128. Be careful it may takes up to one minute to start a connection.

Command formate

    burstlink local_port target_id target_ip target_port
    
    local_port  local port
    target_id   target computer's ID, the long string in terminal output
    target_ip   the IP address you want target computer to connect with. If you just want to connect with target computer, then it would be 127.0.0.1 
    target_port target port you want to connect
    
How this is achieved

In fact this process is simple. We create a socket both on local computer and remote computer. The two socket comminute via tox. And it seems as if we have binded the remote computer's port to local port.
    
##Licence
  You are free to do anything...  

***BurstLink*** 简单来说就是一个端口映射软件。它能够把任意远程电脑的端口映射到本地端口。这样你就可以通过直接连接本地端口来和远程的电脑通信。这是一个p2p软件所以连接速度要比vpn之类的快很多。这个软件是建立在[Tox](https://github.com/irungentoo/toxcore "toxcore")项目之上的。

有什么用？
有很多用处，你可以利用这个和不同局域网的人玩局域网游戏。远程控制不在同一个局域网内的电脑。连接不在同一个局域网的ftp服务器。总之，几乎所有被局域网限制的应用都可用这个来解除限制。

如果你对这个软件有什么建议欢迎在issue里面提出

##编译安装

###Linux
下面以ubuntu为例，其他的系统可以参照调整。

首先要编译安装toxcore,toxcore的安装方法在其项目主页上很详细。主页地址 https://github.com/irungentoo/toxcore
**注意：toxcore经过这么长时间已经变化了太多，这个软件只能使用特定版本的toxcore。在clone完toxcore后请执行以下代码。**
``` git checkout 909db029412ddbd499281711a9cd46c3df4fe5a0```

接着要安装jansson

    wget http://www.digip.org/jansson/releases/jansson-2.6.tar.gz
    tar -xzvf jansson-2.6.tar.gz
    cd jansson-2.6/
    ./configure
    make
    sudo make install

安装uuid-dev
    
    sudo apt-get install uuid-dev
    
编译主程序
    
    git clone https://github.com/randoms/burst-link
    cd burst-link/burstlink/
    ./compile.sh
    
这样就编译完成了，生成的文件在 burst-link/burstlink/Release/ 文件夹中
其中 burstlink 是客户端程序， burstlinkser 是服务端程序
    
    
###Windows

获取主程序

    git clone https://github.com/randoms/burst-link
    
如果只是使用的话是可以不用编译的。在 burst-link/burstlink/Release/ 文件夹中有已经编译好的文件，其中burstlink.exe 是客户端程序，burstlinkser.exe 是服务端程序。注意Release文件夹中的dll文件也是必须的，如果想要在其他地方使用一定要带上dll文件。

如何编译
用 visual studio 打开burstlink文件夹内的burstlink.sln,点击visual studio 内的编译就行了。


##使用方法
现在假如B电脑想要连接到A电脑的3128端口，但是A和B并不在一个局域网中，A经过路由器连接到互联网中。
首先在A电脑上运行服务端程序

    burstlinkser
    
    这时终端会显示
    MYID:A5F437F9014941214300912F3E66CBB6F42B4CB0A354B187332552C7C500706C40C249A2823C

记录下MYID:后面的一长串字符，这个是A电脑的ID，这个以后会用到。

在B电脑上执行

    burstlink 9990 A5F437F9014941214300912F3E66CBB6F42B4CB0A354B187332552C7C500706C40C249A2823C 127.0.0.1 3128
    
中间一长串就是A的ID， 9990 代表本地端口， 127.0.0.1 是B想要A连接的IP，3128是B想要A连接的端口。整个的效果就是A把自己的3128端口映射到B的9990端口上.

    等待成功连接终端显示
    
    TOXCORE:ONLINE
    SERVER:LISTEN ON 9990
    online
    MESSAGE RECEIVED
    HANDSHAKE RECEIVED
    402
    CONNECT:OK

    
这时只要连接到本地的9990端口就好像直接连接到A的3128端口上一样。注意这个连接过程最长可能要一分钟，不过连接成功之后速度就很快了。

指令的格式

    burstlink local_port target_id target_ip target_port
    
    local_port  本地端口
    target_id   目标电脑的ID，就是执行服务端程序后显示的那一长串字符
    target_ip   想要目标电脑映射的IP，如果是目标电脑自身，那就是127.0.0.1
    target_port 想要映射的目标端口
    
实现原理

这个实际上是在本地和远程电脑上分别创建了一个socket，然后这两个socket的通信通过tox来实现。由于tox本身是p2p的所以实现了p2p的效果。从外表看来就好像直接把远程的端口绑定到了本地一样。
    
##软件许可
  You are free to do anything...  
