/**
 * this is the model object to store current valid connects
 */
var uuid = require("node-uuid");


function Connections(database){
    this.from = "";
    this.sock = "";
    this.uuid = "";
    
    // basic data
    this.database = database;
    if(typeof database.connections == 'undefined')
        database.connections = [];
    this.connections = database.connections;
    
    this.objects = {};
    this.objects.getInfo = function(){
        var info = {};
        info.from = this.from;
        info.sock = this.sock;
        info.uuid = this.uuid;
        return info;
    }.bind(this);
    
    this.objects.save = function(){
        if(this.from == "" || this.sock == ""){
            throw "Invalid Connection data";
        }
        
        if(this.uuid == ""){
            // add the first time
            if(this.objects.get(this.name) != '' && this.name != "LOCAL"){
                throw "this connection is already exsited";
            }
            this.uuid = uuid.v1();
            var info = this.objects.getInfo();
            database.connections.push(info);
        }else{
            //change exsit record
            for(var cont in database.connections){
                if(this.uuid == database.connections[cont].uuid){
                    database.connections[cont] = this.objects.getInfo();
                    return;
                }
            }
            throw "target not found"
        }
    }.bind(this);
    
    this.objects.toObj = function(info){
        var myConnect = new Connections(this.database);
        myConnect.from = this.from;
        myConnect.sock = this.sock;
        myConnect.uuid = this.uuid;
        return myConnect;
    }.bind(this);

    this.objects.all = function(){
        var ResList = [];
        for(var cont in this.connections){
            ResList.push(this.objects.toObj(this.connections[cont]));
        }
        return ResList;
    }.bind(this);

    this.objects.get = function(uuid,raiseErr){
        for(var cont in this.connections){
            if(this.connections[cont].uuid == uuid){
                return this.toObj(this.connections[cont]);
            }
        }
        if(raiseErr){
            throw "target not found";
        }
        return '';
    }.bind(this);

    this.objects.delete = function(){
        for(var cont in this.connections){
            if(this.connections[cont].uuid == this.uuid){
                this.connections.splice(cont,1);
                return;
            }
        }
        throw "target not found";
    }.bind(this);
    
}

module.exports = Connections;