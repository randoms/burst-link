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
var localhost = '127.0.0.1'
var async = require('async');
var uuid = require('node-uuid');
var spawn = require('child_process').spawn
var connectM = require('./models/connections.js');

var database = {}
database.local_tox_port = 0;
database.localTcpServer = "";
database.localCmdList = [];

var command_free_flag = 0; // 0 current ctrl sock is not free, 1 free

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


function init(local_port,remote_tox_id,remote_ip,remote_port,cb){
    console.log("init burst link ...");
    var toxcore;
    if(remote_tox_id == null){
        // start toxcore in server mode
        toxcore = spawn(__dirname+'/../server',[local_port],{cwd:__dirname+'/../'});

    }else{
        // start toxcore in req mode
        toxcore = spawn(__dirname+'/../server',[local_port,remote_tox_id,remote_ip,remote_port],{cwd:__dirname+'/../'});
    }
    
    toxcore.on("close",function(code){
        if(code == 0){
            console.log("TOXCORE:EXITED");
        }else{
            
        }
    })
    
    // start local cmd loop
    setInterval(function(){
        while(command_free_flag == 1 && database.localCmdList.length != 0){
            
        }
    },10) 
}


function connect(remote_tox_id,local_port,remote_ip,remote_port){
    
    listenLocal(local_port,remote_tox_id,function(){
        init(local_tox_port,remote_tox_id,remote_ip,remote_port,function(err){
            if(err.status == "OK");
            console.log("Burst Link:READY");
        });
        
    })
}

connect("468A885803010AB022BD738453C8C0739D354F42AF1261CD8F4978CA6775D02E97A6D9141723",9998,"127.0.0.1",22)


/**
 * 第一个连接的是tox指令sock
 * 第二个连接的是tox数据sock
 * 以后连接的是普通本地sock
 */
sockCount = 0;
function initLocalSock(localPort){
    database.localTcpServer = net.createServer(function(sock){
        if(sockCount == 0){
            // tox ctrl sock connected
            onNewSock(sock,"CTRL");
            sockCount += 1;
        }else if(sockCount == 1){
            // tox data sock connected
            onNewSock(sock,"DATA");
            sockCount += 1;
        }else{
            // local sock received
            onNewSock(sock,"LOCAL");
        }
        
    }).listen(localPort);
}

/**
 * this function is called when new sock connect to local sock server
 */
function onNewSock(sock,type){
    var mySock = new connectM(database);
    mySock.from = type;
    mySock.sock = sock;
    mySock.objects.save();
    if(type == "LOCAL"){
        mySock.sock.on("data",function(data){
            var cmd = mySock.objects.getInfo();
            cmd.data = data;
            onLocalDataReceived(cmd);
        })
        
        mySock.sock.on("error",function(){
            // delete record
            closeSock(mySock.uuid);
        })
        
        mySock.sock.on("close",function(){
            closeSock(mySock.uuid);
        })
    }
    if(type == "CTRL"){
        mySock.sock.on("data",function(data){
            try{
                data = JSON.parse(data)
            }catch(e){
                console.log("JOSN Parse Error");
                return;
            }
            
            if(data == "OK")
        })
        
    }
    if(type == "DATA"){
        mySock.sock.on("data",function(data){
            
        })
    }
}

function closeSock(uuid){
    
}

function sendLocalData(uuid,data){
    
}

function sendRemoteData(uuid,data){
    
}

function sendLocalCmd(cmd){
    if(command_free_flag == 0){
        // get ctrl sock
        var ctrlSock = new connectM(database);
        ctrlSock = connectM.objects.find("CTRL").sock;
        // get data sock
        dataSock = connectM.objects.find("DATA").sock;
        
    }
}

function sendRemoteCmd(cmd){
    
}

function onRemoteDataReceived(){
    
}

function onRemoteCmdReceived(){
    
}
/**
 * this function is called when local sock data is received
 * cmd format
 * {
 *  from:
 *  uuid:
 *  sock
 *  data
 * }
 */
function onLocalDataReceived(cmd){
    // send to toxcore
    
}
