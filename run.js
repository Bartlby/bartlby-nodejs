var bartlby = require('./build/Release/bartlby');

var obj = new bartlby.Instance("1.cfg");
console.log(obj);
console.log(obj.CFG());

svc=obj.getService(1);
console.log(svc);
console.log("ID:" + svc.service_id);
