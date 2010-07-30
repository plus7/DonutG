
var Donut, strIniPath, nCount;

//Donutの機能を操作するためのDonutオブジェクトを取得する
Donut = new ActiveXObject("DonutP.API");

//API登録.batを実行していなかったりすると取得できず、
//この先進めないので中止する
if(Donut == null)
	WScript.Quit();

//スクリプトフォルダのパスを取得してiniファイルのパスを作成する
strIniPath = Donut.GetScriptFolder();
if(strIniPath == "")
	WScript.Quit();
strIniPath += "\ini.ini";

//保存してある値を読み込んで、1増やして、何回目か表示して、保存する
nCount = Donut.GetProfileInt(strIniPath,"Test","Count",0);
nCount++;
WScript.Echo(nCount + "回目の呼び出しです");
Donut.WriteProfileInt(strIniPath,"Test","Count",nCount);