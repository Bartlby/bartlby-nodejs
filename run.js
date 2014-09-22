var bartlby = require('./build/Release/bartlby');

var obj = new bartlby.MyObject(42);
console.warn(obj);
console.warn(obj.value());
