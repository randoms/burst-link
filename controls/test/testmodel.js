var connects = require("../models/connections.js");

var database = {};
var myCont = new connects(database);
myCont.from = "here";
myCont.sock = "hello";
myCont.objects.save();
console.log(database);
myCont.objects.delete();
console.log(database);