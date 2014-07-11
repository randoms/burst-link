/**
 * node 主要用于对 texcore进行控制，把连接初始化之后，监听一个tcp端口，把端口接收的数据
 * 转发到 toxcore去， 通过 toxcore 的p2p网路连接到远程主机。 整体上看就是实现了一个p2p
 * 的socket 连接
 * 
 * connect(remote_tox_id,local_port,remote_port,cb);
 * 
 * 测试参数
 * 本地要映射的端口 9992
 * 远程端口 50001
 * 本地toxcore监听的端口 9990
 * nodejs和toxcore通信的端口 9991
 * 
 */

var net = require('net');
var local_tox_port = 9990
var localhost = '127.0.0.1'
var async = require('async');
var uuid = require('node-uuid');

function sendMessage(msg,cb){
    var res = "";
    var client = new net.Socket();
    client.connect(local_tox_port, localhost, function() {
        client.write(msg);
        client.end();
    });

    client.on('data', function(data) {
        res += data;
    });

    client.on('close', function() {
        console.log('Connection closed');
        cb(res);
    });
}

function addFriend(remote_tox_id,cb){
    var cmd = {
        'type':'REQ',
        'mode':'SERVER',
        'cmd':'ADD_FRIEND',
        'from':'LOCALHOST',
        'remoteID':remote_tox_id,
    }
    
    sendMessage(JSON.stringify(cmd,null,4),function(res){
        console.log("RES:"+res);
        res = JSON.parse(res);
        if(res.retcode == 102){
            // add friend failed
            var err = {
                status:'ERROR',
                description:res.description,
            }
            console.log(JSON.stringify(err,null,4));
            console.log(res);
            process.exit(1);
        }else if(res.retcode == 104){
            // friend req has send
            console.log("Connecting ...");
        }
        console.log("ADD_FRIEND:SUCCESS");
        cb();
    })
}


function sendHandshake(remote_tox_id,cb){
    var req = {
        type:'REQ',
        mode:'SERVER',
        cmd:'SEND_MESSAGE',
        from:'LOCALHOST',
        remoteID:remote_tox_id,
        msg:{
            packID:"0",
            total:"1",
            order:"0",
            package:[],
        },
    }
    
    var reconnectCount = 0;
    
    var handshake = function(){
        sendMessage(JSON.stringify(req,null,4),function(res){
            console.log("RES:"+res);
            res = JSON.parse(res);
            if(res.retcode == 204){
                // target is offline
                reconnectCount ++;
                if(reconnectCount >5){
                    console.log("target is offline");
                    process.exit(1);
                }
                setTimeout(handshake,1000); // try reconnect
            }else if(res.retcode == 203){
                console.log("HANDSHAKE:Send message failed");
                process.exit(0);
            }else if(res.retcode == 201){
                cb();
            }
        })
    }
    handshake();
}

function init(remote_tox_id,cb){
    console.log("init burst link ...");
    
    var tastList = [
        // 加好友
        function(callback){
            addFriend(remote_tox_id,function(){
                callback(null,0);
            })
        },
        function(callback){
            console.log("here");
            sendHandshake(remote_tox_id,function(){
                callback(null,0);
            })
        }
    ]
    
    
    async.series(tastList,function(res){
        console.log("INIT:COMPLETE");
        cb();
    })
    
    /**
     * response format
     * res = {
     *      type:'RES',
     *      status:'OK or ERROR'
     *      retcode:,
     *      description:"",
     *      mode:'SERVER',
     *      cmd:'ADD_FRIEND',
     *      from:'LOCALHOST',
     *      remoteID:'',
     *   }
     */
    
    
}


function listenLocal(local_port,remote_tox_id){
    
    var client = new net.Socket();
    var sock = "";
    client.connect(local_tox_port,localhost,function(){
        
        net.createServer(function(msock) {

            sock = msock;
            console.log('CONNECTED: ' +
                sock.remoteAddress + ':' + sock.remotePort);


            sock.on('data', function(data) {
                console.log('DATA ' + sock.remoteAddress + ': ' + data);
                // 加工收到的数据，如果太大就进行分包处理
                /**
                 * 封包格式
                 * var cmd = {
                 *   'type':'REQ',
                 *   'mode':'SERVER',
                 *   'cmd':'SEND_MESSAGE',
                 *   'from':'LOCALHOST',
                 *   'remoteID':remote_tox_id,
                 *   'packID':uuid,
                 *   'total':total package num,
                 *   'order':the order of currnet package
                 * }
                 */
                var max_pack_size = 1024;
                var total = parseInt(data.length/max_pack_size)+1;
                var packArray = [];
                var packID = uuid.v1();
                for(var count = 0;count<total;count++){
                    packArray[count] = [];
                    var packSize = 0;
                    if(count == total -1){
                        packSize = data.length%max_pack_size;
                    }else{
                        packSize = max_pack_size;
                    }
                    for(var i = 0;i<packSize;i++){
                        packArray[count][i] = data[count*max_pack_size+i];
                    }
                    var req = {
                        type:'REQ',
                        mode:'SERVER',
                        cmd:'SEND_MESSAGE',
                        from:'LOCALHOST',
                        remoteID:remote_tox_id,
                        msg:{
                            packID:packID,
                            total:total,
                            order:count,
                            package:packArray[count],
                        },
                    }
                    // 把接收的数据发送给toxcore
                    console.log(req);
                    client.write(JSON.stringify(req,null,4));
                }
                console.log(data.length);
            });

            sock.on('close', function(data) {
                console.log('CLOSED: ' +
                    sock.remoteAddress + ' ' + sock.remotePort);
                // 关闭和toxcore的连接
                client.end();
            });

        }).listen(local_port, localhost);
    })
    
    client.on('data',function(data){
        // 把接收到的数据传给本地端口
        sock.write(data);
    })
    
    client.on('close',function(){
        console.log("Burst Link ended")
    })
    
    
}

function connect(remote_tox_id,local_port,remote_port){
    
    init(remote_tox_id,function(){
        listenLocal(local_port,"341CCFBCC4D41C5B3AB89E31E7561C5D37E201D5DDBFA7AFC6B4EDD2D6A82F4B7D06A2ED3DE4");
    })


    
}

connect("341CCFBCC4D41C5B3AB89E31E7561C5D37E201D5DDBFA7AFC6B4EDD2D6A82F4B7D06A2ED3DE4",9992,50001)