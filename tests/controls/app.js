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
var local_tox_port = 0;
var localhost = '127.0.0.1'
var async = require('async');
var uuid = require('node-uuid');
var spawn = require('child_process').spawn
var toToxTcpServer = "";
var toToxTcpServerSock = "";
var localTcpServerSock = "";
var localTcpClientSock = "";



function init(local_port,remote_tox_id,remote_ip,remote_port){
    console.log("init burst link ...");
    var toxcore;
    if(remote_tox_id == null){
        // start toxcore in server mode
        toxcore = spawn(__dirname+'/../server',[local_port],{cwd:__dirname+'/../'});

    }else{
        // start toxcore in req mode
        toxcore = spawn(__dirname+'/../server',[local_port,remote_tox_id,remote_ip,remote_port],{cwd:__dirname+'/../'});
    }
    toxcore.on("error",function(err){
        console.log(err);
    })

    toxcore.stderr.on('data', function (data) {
        console.log('stderr: ' + data);
    });
    
    toxcore.stdout.on('data',function(data){
        console.log(data.toString());
        data = data+"";
    })
    
    toxcore.on("close",function(code){
        if(code == 0){
            console.log("TOXCORE:EXITED");
        }else{
            if(cb != null)cb({
                status:"ERROR"
            })
        }
    })
}


function isFromLocal(data){
    var localCMD = "LOCAL:";
    var equalFlag = 1;
    for(var i=0;i<localCMD.length;i++){
        if(localCMD.charCodeAt(i) != data[i])equalFlag = 0;
    }
    if(equalFlag == 1)return true;
    return false;
}

function parseCmd(data){
    data = data.toString();
    var cmd = data.substring("LOCAL:".length);
    if(cmd == "402"){
        // toxcore ready
        console.log("TOXCORE:READY");
    }else if(cmd.indexOf("INIT_REQ") == 0){
        var content = cmd.substring("INIT_REQ:".length);
        content = JSON.parse(content);
        connectLocal(content.TARGET_IP,content.PORT,function(res){
            if(res == "OK"){
                toToxTcpServerSock.write("INIT_REQ:OK");
                console.log("Start Local port OK");
            }
            else{
                toToxTcpServerSock.write("INIT_REQ:ERROR");  
                console.log("Start Local port error");
            } 
        });
        
        
    }
    
}

function listenLocal(local_port,remote_tox_id,cb){
    
    var toToxTcpServer = net.createServer(function(toxsock){
        toToxTcpServerSock = toxsock;
        console.log('CONNECTED: ' +
            toToxTcpServerSock.remoteAddress + ':' + toToxTcpServerSock.remotePort);
        
        net.createServer(function(msock) {

            localTcpServerSock = msock;
            console.log('CONNECTED: ' +
                localTcpServerSock.remoteAddress + ':' + localTcpServerSock.remotePort);


            localTcpServerSock.on('data', function(data) {
                console.log('DATA ' + localTcpServerSock.remoteAddress + ': ' + data);
                // 加工收到的数据，如果太大就进行分包处理
                
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
                    toToxTcpServerSock.write(JSON.stringify(req,null,4));
                }
                console.log(data.length);
            });

            localTcpServerSock.on('close', function(data) {
                console.log('CLOSED: ' +
                    localTcpServerSock.remoteAddress + ' ' + localTcpServerSock.remotePort);
                // 关闭和toxcore的连接
                toToxTcpServerSock.end();
            });

        }).listen(local_port, localhost);
        
        
        // 远端来数据的处理
        
        toToxTcpServerSock.on('data',function(data){
            // 转发给本地
            if(isFromLocal(data)){
                parseCmd(data);
                console.log(data.toString());
            }else{
                console.log(data.toString());
                localTcpServerSock.write(data);
            }
        })
        
        toToxTcpServerSock.on('close',function(data){
            // 关闭本地
            //localTcpServerSock.end(data);
        })
        
    }).listen(0, localhost)
    
    // get local toToxTcpServer port
    toToxTcpServer.on('listening', function() {
    local_tox_port = toToxTcpServer.address().port
    if(cb != null)cb();
  })
}

function connectLocal(target_ip,target_port,cb){    

    var client = new net.Socket();
    localTcpClientSock = client;
    
    client.connect(target_port, target_ip, function() {
        cb("OK");
    });

    client.on('data', function(data) {
        console.log(data);
        toToxTcpServerSock.write(data);
    });

    client.on('close', function() {
        console.log('Connection closed');
    });
    
    client.on('error',cb);
}

function listenLocalServ(cb){
    
    var toToxTcpServer = net.createServer(function(toxsock){
        toToxTcpServerSock = toxsock;
        console.log('CONNECTED: ' +
            toToxTcpServerSock.remoteAddress + ':' + toToxTcpServerSock.remotePort);
        
        // 远端来数据的处理
        
        toToxTcpServerSock.on('data',function(data){
            // 转发给本地
            if(isFromLocal(data)){
                parseCmd(data);
                console.log(data.toString());
            }else{
                console.log(data.toString());
                localTcpClientSock.write(data);
            }
        })
        
        toToxTcpServerSock.on('close',function(data){
            // 关闭本地
            //localTcpServerSock.end(data);
        })
        
    }).listen(0, localhost)
    
    // get local toToxTcpServer port
    toToxTcpServer.on('listening', function() {
    local_tox_port = toToxTcpServer.address().port
    if(cb != null)cb();
  })
}

function connect(remote_tox_id,local_port,remote_ip,remote_port){
    
    listenLocalServ(function(err){
        init(local_tox_port,null,null,null);
    })
}

connect();