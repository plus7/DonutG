var DonutP;
var index;
var document, window;
var strAnchor;

DonutP = new ActiveXObject("DonutP.API"); 
index  = DonutP.TabIndex; 
document = DonutP.GetDocumentObject(index); 
window =   DonutP.GetWindowObject(index); 
if (index != -1) { 
	strAnchor = "<A HREF=\"" + document.URL + "\">" + document.title + "</A>";
	window.clipboardData.setData("text",strAnchor); 
} 
DonutP = null; 