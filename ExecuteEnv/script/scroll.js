//実行時引数に指定した量だけスクロール

var donut = Components.classes["@tnose.net/donut/api-service;1"]
    .getService(Components.interfaces.donutIAPI);

var nIndex = donut.tabIndex;
var window = donut.getWindowObject(nIndex);

var amount = 100;
window.scrollBy(0,amount);
