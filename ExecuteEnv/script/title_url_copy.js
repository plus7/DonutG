//アクティブタブのタイトルとURLをクリップボードにコピー

var Donut = new ActiveXObject("DonutP.API");
var nIndex = Donut.TabIndex;
var document = Donut.GetDocumentObject(nIndex);
var window = Donut.GetWindowObject(nIndex);

var text = document.title + "\n" + document.URL + "\n";
window.clipboardData.setData("text", text);