var request = require('request');

var r = request.defaults({'proxy':'http://127.0.0.1:9990'})

r.get('http://winfly.shareyan.cn').on('response', function(response) {
    console.log(response.statusCode) // 200
    console.log(response.headers['content-type']) // 'image/png'
}).on('error',function(error){
	console.log('error');
	console.log(error);
})