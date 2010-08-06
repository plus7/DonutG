var donut = Components.classes["@tnose.net/donut/api-service;1"]
    .getService(Components.interfaces.donutIAPI);

var index = donut.tabIndex;
var document = donut.getDocumentObject(index);

if (index != -1) { 
	var text = "<a href=\"" +
	    document.URL + 
	    "\">" +
	    document.title +
	    "</a>";
	const gClipboardHelper = Components.classes["@mozilla.org/widget/clipboardhelper;1"].  
	    getService(Components.interfaces.nsIClipboardHelper);
	gClipboardHelper.copyString(text);
}