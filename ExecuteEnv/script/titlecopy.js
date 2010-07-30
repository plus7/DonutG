//アクティブタブのタイトルをクリップボードにコピー

var Donut = new ActiveXObject("DonutP.API");
var nIndex = Donut.TabIndex;
var document = Donut.GetDocumentObject(nIndex);
var window = Donut.GetWindowObject(nIndex);

window.clipboardData.setData("text", document.title);