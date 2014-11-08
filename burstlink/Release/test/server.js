var net = require('net');

var HOST = '127.0.0.1';
var PORT = 9994;


// 创建一个TCP服务器实例，调用listen函数开始监听指定端口
// 传入net.createServer()的回调函数将作为”connection“事件的处理函数
// 在每一个“connection”事件中，该回调函数接收到的socket对象是唯一的
var databefore = "";
var count = 0;
net.createServer(function(sock) {

    // 我们获得一个连接 - 该连接自动关联一个socket对象
    console.log('CONNECTED: ' +
        sock.remoteAddress + ':' + sock.remotePort);
    
    // 为这个socket实例添加一个"data"事件处理函数
    sock.on('data', function(data) {
        console.log(data);
    });

    // 为这个socket实例添加一个"close"事件处理函数
    sock.on('close', function(data) {
        console.log('CLOSED' + count);
        count += 1;
    });

    sock.on('error',function(err){
        console.log(err);
    })

}).listen(PORT, HOST);

console.log('Server listening on ' + HOST +':'+ PORT);