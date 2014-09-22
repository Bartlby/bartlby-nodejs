var bartlby = require('./build/Release/bartlby');

var obj = new bartlby.Instance("/opt/bartlby/etc/bartlby.cfg");
console.log(obj);
console.log(obj.CFG());

svc=obj.getService(1);
console.log(svc);
console.log("ID:" + svc.service_id);

info=obj.getInfo();
console.log(info);

obj.close();





var add=obj.addService({
	"service_name": "Name",
	"server_id": 2

});

console.log(add);