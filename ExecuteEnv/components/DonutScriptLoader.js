const Cc = Components.classes;
const Ci = Components.interfaces;
const Cu = Components.utils;

Cu.import("resource://gre/modules/XPCOMUtils.jsm");

function donutScriptLoader() {}
donutScriptLoader.prototype = {
  classDescription: "Donut Script Loader",
  contractID: "@tnose.net/donut/loader-service;1",
  classID: Components.ID("{1a006373-2c4f-48db-a75f-a6af9cca0b1b}"),
  QueryInterface: XPCOMUtils.generateQI([Ci.donutIScriptLoader]),

  loadFile: function (aFile) {
	var promptSvc = Cc["@mozilla.org/embedcomp/prompt-service;1"]
	                .getService(Ci.nsIPromptService);

	try{

	    const ioService = Components.classes['@mozilla.org/network/io-service;1']
		.getService(Components.interfaces.nsIIOService);

	    var url = ioService.newFileURI(aFile).spec;

	    var loader = Components.classes["@mozilla.org/moz/jssubscript-loader;1"]
		.getService(Components.interfaces.mozIJSSubScriptLoader);

	    var ctx = function(){};

	    loader.loadSubScript(url, ctx);

	}catch(e){
	    promptSvc.alert(null, "[DonutScriptLoader]", e);
	}
  }
};

function NSGetModule(aCompMgr, aFileSpec)
  XPCOMUtils.generateModule([donutScriptLoader]);
