//リンクバーの表示を切り替えます

var DonutP;

//Donutの機能を操作するためのDonutオブジェクトを取得する
DonutP = new ActiveXObject("DonutP.API");

//機能番号32933(リンクバーの表示切り替えコマンド)を実行する
//番号と機能の対応表はdocフォルダのcommand.txtに記載
DonutP.ExecuteCommand(32933);

