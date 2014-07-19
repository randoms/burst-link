var net = require('net');

var HOST = '127.0.0.1';
var PORT = 9990;

var client = new net.Socket();
client.connect(PORT, HOST, function() {

    console.log('CONNECTED TO: ' + HOST + ':' + PORT);
    // 建立连接后立即向服务器发送数据，服务器将收到这些数据
    setInterval(function(){
        var buf = new Buffer(1024);
        for(var i=0;i<1024;i++){
            buf[i] = i;
        }
        client.write(buf);
        console.log("writing");
    },5);

});

// 为客户端添加“data”事件处理函数
// data是服务器发回的数据
client.on('data', function(data) {
    console.log(data);
    // 完全关闭连接
    //client.destroy();

});

// 为客户端添加“close”事件处理函数
client.on('close', function() {
    console.log('Connection closed');
});