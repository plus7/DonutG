//Javaの実行・非実行を切り替える

var DonutP, nIndex, nState;
var FLAG_SE_JAVA      = 0x00000040;
var FLAG_SE_NOREFRESH = 0x01000000; 

//Donutの機能を操作するためのDonutオブジェクトを取得する
DonutP = new ActiveXObject("DonutP.API");

//アクティブなタブのインデックス（通し番号）を取得する
nIndex = DonutP.TabIndex;　//タブがないときは-1を返す

if(nIndex != -1){
	//タブの状態を取得する
	nState = DonutP.GetExtendedTabState(nIndex);

	if(nState & FLAG_SE_JAVA){
		//FLAG_SE_JAVAのフラグをOFFにする
		nState &= ~FLAG_SE_JAVA; 
	}else{
		//FLAG_SE_JAVAのフラグをONにする
		nState |= FLAG_SE_JAVA;  
	}

    //リロードしないようにする
	nState |= FLAG_SE_NOREFRESH;

	//新しくタブの状態を設定する
	DonutP.SetExtendedTabState(nIndex,nState);
}


//
//ビット操作に関する簡単な説明
//
//nStateをタブの状態の値だとします。
//
//特定のフラグAをONにしたいとき
//nState |= A;
//
//特定のフラグAをOFFにしたいとき
//nState &= ~A;
//
//特定のフラグAの状態を逆にしたいとき
//nState ^= A;
//
//特定のフラグAがONであるか確認したいとき
//nState & A
//　が0でないとき : ON
//　　0のとき　　 : OFF
//
//複数のフラグの場合でも同じです。
//複数のフラグA,B,CをすべてONにしたいとき
//nState |= (A | B | C);
//