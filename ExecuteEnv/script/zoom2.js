//画面のズームを行うスクリプト
//実行時引数として-5～5の値を渡すことで-500%～500%
//の間で表示サイズを変更できる
//こちらは相対的にサイズを変化させる

var Donut = new ActiveXObject("DonutP.API");
var document = Donut.GetDocumentObject(Donut.TabIndex);

var nZoomSize = parseFloat(WScript.Arguments(0));
if(nZoomSize < -5.0 || 5.0 < nZoomSize){
	nZoomSize = 0.0;
}

var present = parseFloat(document.body.style.zoom);
if(isNaN(present)) present = 1.0;
var nNewSize = present+nZoomSize;
if(nNewSize <= 0.0) nNewSize = 1.0;

document.body.style.zoom = nNewSize;
