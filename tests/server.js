var net = require('net');

var HOST = '127.0.0.1';
var PORT = 9994;


// 创建一个TCP服务器实例，调用listen函数开始监听指定端口
// 传入net.createServer()的回调函数将作为”connection“事件的处理函数
// 在每一个“connection”事件中，该回调函数接收到的socket对象是唯一的
var databefore = "";
net.createServer(function(sock) {

    // 我们获得一个连接 - 该连接自动关联一个socket对象
    console.log('CONNECTED: ' +
        sock.remoteAddress + ':' + sock.remotePort);

    // 为这个socket实例添加一个"data"事件处理函数
    sock.on('data', function(data) {
        //console.log(data);
        // 回发该数据，客户端将收到来自服务端的数据
        if(databefore != ""){
            if(data[0] != databefore +1 && data[0] != 0){
                console.log(databefore + ' '+ data[0]);
                var res = [];
                for(var i=0;i<data.length;i++){
                    res.push(data[i]);
                }
                console.log(JSON.stringify(res));
                throw "DATA ERROR 1";
            }
        }
        for(var i=0;i<data.length-1;i++){
            if(data[i+1] != data[i]+1 && data[i+1] != 0){
                console.log(data[i]+ ' ' +data[i+1] + ' '+i);
                var res = [];
                for(var i=0;i<data.length;i++){
                    res.push(data[i]);
                }
                console.log(JSON.stringify(res));
                throw "DATA ERROR 2";
            }
        }
        databefore = data[data.length-1];
        sock.write(data);
    });

    // 为这个socket实例添加一个"close"事件处理函数
    sock.on('close', function(data) {
        console.log('CLOSED: ' +
            sock.remoteAddress + ' ' + sock.remotePort);
    });

}).listen(PORT, HOST);

console.log('Server listening on ' + HOST +':'+ PORT);