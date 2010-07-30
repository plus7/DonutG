//画面のズームを行うスクリプト
//実行時引数として0.1～10の値を渡すことで10%～1000%
//の間で表示サイズを変更できる

var Donut = new ActiveXObject("DonutP.API");
var document = Donut.GetDocumentObject(Donut.TabIndex);

var nZoomSize = parseFloat(WScript.Arguments(0));
if(nZoomSize < 0.1 || 10.0 < nZoomSize){
	nZoomSize = 1.0;
}

document.body.style.zoom=nZoomSize;