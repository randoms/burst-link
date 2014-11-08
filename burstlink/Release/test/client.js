var net = require('net');

var HOST = '127.0.0.1';
var PORT = 9990;

var client = new net.Socket();
var count = 0;
setInterval(function(){
    client.connect(PORT, HOST, function() {
        console.log('conncected');
        client.write("test");
        client.end();
    });
},100)

// 为客户端添加“close”事件处理函数
client.on('close', function() {
    console.log('Connection closed ' + count);
    count += 1;
});

client.on('error',function(err){
    console.log(err);
})