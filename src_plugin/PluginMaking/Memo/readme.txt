Memo.dll

undonut+mod版での修正内容

2008-07-11 +mod
・vs2005/2008 でコンパイル. x64生成追加.
・テキストの最大サイズが32Kバイトだが、32Kバイト以上のテキストを
　よんだとき、確保外のメモリに0x00を書き込む可能性があったのを修正.
・適当にインデント等を付け直した。


2008-08-10 +mod
・UNICODE-apiベースのコンパイル対策。
　といってもファイルへのロードセーブはSJISのため、あまり意味なし。
　ただundonut起動中のコピペ前提でちょっとした編集に使う分には影響あるかも.
