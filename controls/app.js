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


function init(remote_tox_id,cb){
    console.log("init burst link ...");
    
    var tastList = [
        // 加好友
        function(callback){
            
            var cmd = {
                'type':'REQ',
                'mode':'SERVER',
                'cmd':'ADD_FRIEND',
                'from':'LOCALHOST',
                'remoteID':remote_tox_id,
            }
            
            sendMessage(JSON.stringify(cmd,null,4),function(res){
                if(res.status != "OK"){
                    var err = {
                        status:'ERROR',
                        description:res.description,
                    }
                    console.log(JSON.stringify(err,null,4));
                    console.log(res);
                    process.exit(1);
                }
                console.log("ADD_FRIEND:SUCCESS");
                callback(null,res)
            })
        },
        function(callback){
            callback(null,0);
        }
    ]
    
    
    async.series(tastList,function(res){
        console.log(res);
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


function listenLocal(local_port){
    
    var client = new net.Socket();
    var sock = "";
    client.connect(local_tox_port,localhost,function(){
        
        net.createServer(function(msock) {

            sock = msock;
            console.log('CONNECTED: ' +
                sock.remoteAddress + ':' + sock.remotePort);


            sock.on('data', function(data) {
                console.log('DATA ' + sock.remoteAddress + ': ' + data);
                // 把接收的数据发送给toxcore
                client.write(data);
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
    
    init(remote_tox_id,function(data){
        console.log(data);
        listenLocal(local_port);
    })


    
}

connect("",9992,50001)