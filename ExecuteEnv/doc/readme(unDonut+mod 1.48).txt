////////////////////////////////////////////////////////////////////////////
【 ソフト名 】unDonut+mod                     / 64unDonut
【バージョン】1.48                            / 1.48
【  作成日  】2009/03/16
【改  変  者】+mod
【 必要環境 】日本語版 Windows2000,Xp,Vista
　　　　　　　Microsoft Internet Explorer 6 (5.5?) 以上
【 製作環境 】Microsoft Visual Studio 2008 ATL8/WTL8.0
【 動作確認 】Windows Vista(64) + IE7
【 配布形態 】フリーウェア
【一時配布元】http://undonut.undo.jp/
【転　　　載】好きにしてください。再配布も。
【主な著作者】  2000-2001 MB        Donut
　　　　　　　　          D0GSt0re  DonutR
　　　　　　　　2001-2003 UH        DonutP
　　　　　　　　2003-2006 ミニット  unDonut
　　　　　　　　2007      座布団    unDonut+
　　　　　　　　2007-2009 +mod      unDonut+mod / 64unDonut
////////////////////////////////////////////////////////////////////////////



■はじめに

　unDonut(アンドーナツ) は IEコンポーネントを用いたタブブラウザで、
Internet Explorer に似せたインターフェイス(概観および操作性)で豊富な機能
と程よいカスタマイズ性を持っています。
※操作によっては（後にでたIEのバージョンで増えた機能等）互換性のないもの
　もあります。

この配布物は、その unDonut を改造した unDonut+ をさらに勝手改造したもの
です。



■ ファイル.

既知のバグは doc/bugs.txt
更新履歴は   doc/history.txt
をみてください。

その他、配布ファイルは以下のようになっています。

readme.txt      このファイル
unDonut.exe     32ビット版exe
64unDonut.exe   64ビット版exe
setup.exe       32ビット版の関連付けやDonutP-APIのon/off
Menu.ini        メニュー設定
MouseEdit.ini   マウス/マウスジェスチャ設定
CloseURL.ini    ポップアップ抑止url(r13test10付属のもの)

doc/            ドキュメント関係
help/           アプリが参照するヘルプファイルを入れるフォルダ
css/            切り替え用css
favoritegroup/  お気に入りグループ用
menu/           ユーザーメニュー
panel/          エクスプローラーバーのパネルの定義
plugin/         32ビット版のプラグイン
plugin64/       64ビット版のプラグイン
script/         スクリプト
search/         サーチエンジンリスト
skin/           スキン
(toolbar/       ツールバー用)



■ヘルプについて

UndonutNavigator のNike氏がundonut+mod版のヘルプを用意してくださっています。

http://www.geocities.jp/undonut_navi/

のより download して(解凍してできた) undonut+mod.chm を、
undonut+modのhelpフォルダにいれてください。


また、+mod版の祖先である unDonut,DonutP の解説サイトが公開されていますので
参考になると思います。
(+mod版ではバグや細かな仕様変更があって違う部分もあります)

unDonutNavigator          http://www.geocities.jp/undonut_navi/
Donut食った？             http://www.geocities.jp/donut1817/tabbrowser/undonut/
めるてぃの小部屋          http://tabbrowser.info/dwimordene.html
unDonut(+mod)まとめwiki   http://www31.atwiki.jp/undonut_mod/



■新規インストール方法

ダウンロードしたファイル(undonut+mod.1.??_pack.zip)を解凍ツールを利用して
解凍します。
作成されたフォルダをお好みの位置に配置します。
以上で基本的な導入は完了です。
setup.exeを実行すると関連付けとスクリプト機能のON/OFFの設定ができますが
必要ない場合は行わなくても構いません。
unDonutを起動するにはunDonut.exeを実行してください。


※本家unDonutと非互換があり、.exeのみ上書きしたり本家と+/+modを同時利用
　した場合、不具合がでる可能性があります。下記別項参照。



■上書きインストール

undonut+mod.?.??_pack.zip から、既存のundonut+mod環境へ上書きされる場合
ご自身の*.iniファイルを誤って上書きしないよう、気をつけてください。

同梱物では、
- unDonut.ini               全般(随時更新)
- Menu.ini                  メニュー情報(随時更新)
- skin/default/ToolBar.ini  ツールバーの配置情報(随時)
- MouseEdit.ini             マウスボタン設定やマウスジェスチャ設定(オプションで変更)
- CloseURL.ini              url抑止(オプション等で変更)
- search/search.ini         検索エンジン設定(手で変更, 拡張プロパティで変更)
- menu/usermenu.txt         ユーザー設定のメニュー(手で変更)
- その他、配布物のファイル名で弄られているもの。(スクリプトやらプラグインやら)
あたりは気をつけてください。

これら以外の*.exeやdoc/,help/に関しては基本上書きを想定しています。

なお、バージョンアップの具合によっては、iniファイルの中も変わりますので、
場合によっては、新しいiniから設定しなおすか、古いiniに新しい設定を反映
してもらわないといけない場合があるかもしれません。



■アンインストール方法

まずsetup.exeを実行して左下の全解除ボタンを実行します。
続いてunDonutをインストールしたフォルダごと削除してアンインストール完了
です。



■ unicode版/MB(マルチバイト文字)版

　Win-API/unDonut内部での文字の扱いの違いにより、unicode版とMB版があり、
このパッケージに入っているのはunicode版です。

　MB版でも、ページ表示においては、IEコンポーネント内においてunicode文字は
正しく表示されるのですが、SJISで表現しきれないUnicode文字が
ページ名に混ざっていた場合はunDonutが保存する履歴情報中のページ名が化けたり
お気に入り(.url)の表示で化けたり、unicodeテキストのコピペで化ける場合が
あります。
現状、日本語のページだけならば余り問題ありませんが、他言語のページだったり、
今後は日本語でもSJISにない記号等の使用は増えるようにも思うので、基本は
unicode版にしています。

　ただ、unicode版にすると、既存のプラグインや外部アプリとの連携で不具合が
でるものもあるとのことなので、MB版も別途用意しています。
必要な方は同梱はしていないので、別途、undonut.undo.jp からダウンロードして
ください。



■ 本家unDonut との非互換
　本家unDonutと unDonut+/unDonut+mod とでは、iniファイルに一部非互換が
　あり、unDonutを使っていた環境に exe を上書きした場合、おかしな挙動に
　なるようです。

　インストールでは本家のunDonut.iniは用いずに新規にオプション設定して
　やってください。

　また、.dfgやお気に入り(.url),searchエンジンで拡張プロパティを用いて
　いる場合も非互換があり、"メッセージ(ステータスバー)フィルタ"
　"マウス・ジェスチャー使用" "mailto無効" "ページを平面的に表示"
　の設定がずれています。本家の設定がある場合は設定し直してやってくだ
　さい。
　※つまり拡張プロパティを設定した.dfgやお気に入り(.url)は本家unDonut
　　と+/+modとで共存できません。同時利用すると、どちらかで不都合あり。



■ win2k

オプションのツールチップヘルプの表示に msxml3.dll か msxml4.dllを使い
ますが、win2kでは標準ではついていません(xp以降は標準装備)。
別途 MS のサイトよりmsxml3か4をダウンロードしてインストールしてください。
（msxml6等の別のバージョンでは機能しませんので注意）



■ Vista 

　Vistaで出る不具合は当然なんとかしようとしていますが、対応具合はまだまだ
です。

　またVistaアプリとしての作法/体裁を揃えることは二の次なので、vista対応
というと嘘になるかもしれません。

・setupでの関連付けはうまくいかないです。
　自力でレジストリ設定等すればある程度なんとかなるかも、ですが、
　詳しくない（というか検索結果を控えてなくて、自分の環境がうまくってません）

・c:\Program files\ や c:\program files (x86)\ に、unDonutをインストール
　するのは避けたほうがよいでしょう。

・bugs.txt にVistaでの対処をいくつか書いてますのでそちらも見てやってください。




■ 64ビット版 (64undonut.exe)

64ビット版は、警告に従ってコンパイル通したら動いてしまっただけのもので、
32ビット版以上に使いこまれていません。
当然プラグインも64ビット版が必要になります。
なお、32ビット版とは別ソフトとして起動するようになっており、基本的には
32ビット版とは別フォルダにインストールして使用したほうがよいでしょう。

※64ビット版の一番の問題は unDonut 自身よりもまず、Adobe が 64ビット版の
各種ActiveXをリリースしていないことでしょうか。flv,swf,pdf が軒並みアウト
なので...（にこ動やyoutubeがみれません。gyaoはみれるけど)



■ Donut について

私自身詳しいことは知らないのですが、著作者欄に書いてあるように MB氏作の 
Donut を元に分岐していったソフトで Donut 系と呼ばれたりもします。

詳しくは以下のサイト等を参照.
http://web.archive.org/web/20041028104724/http://www.dsn.jp/~yasushi/spdonut.html
http://rapt21.com/family.php
http://www.geocities.jp/undonut_navi/function.html

Donutの派生物は、細かな分岐は多々あるようですが、大別すると、
Donutから派生したDonutRとDonutPの流れがあるようです。

Dount┬→ DonutR → DonutR(Jobby,Dai) → DonutRAPT,DonutQ,DonutL
     │             ↓
     └─────→ DonutP → (DonutP unoficial=)unDonut → unDonut+ → unDonut+mod

unDonut+modは P/un系列として5世代目といったところ。
P/un系のほうは各readme.txt 等を見る限り、ソフトの更新が止まったから
残されたソースを元に勝手改造…の連続のようです（長くても3年なんで、
どこぞの温室の妖精さん状態とも:-)

※DonutPは 本家Donut2.52をベースにDonutRの変更も反映したもののようなので、
  一応 派生図的には Donut の直系のようになりますが、Pの作者の方のページや
  readmeには「DonutRの名前のままにしようと思ったんですが、ややこしいと
  思ったんでこうなりました」「Donut Pがdonut,donut R のソースを元に改造
  されているソフトで有ることを明記」とあるので、Rの子孫でもあります。
  (なのでR→Pでもよいかも... 多少嘘?でもRの子孫であることを示せますし)


Donutの元配布サイトは以下のとおり.

Donut           mb        消失 //hp.vector.co.jp/authors/VA016589/ 
DonutR          D0GSt0re  消失 //tds.sourceforge.net/ 
DonutR          Jobby,dai //www.geocities.com/jobbya/ 
DonutRAPT,Q,L   Rapt      http://rapt21.com/ 
DonutP          UH        消失 //donutp.com/ 
unDonut         Minit     http://undonut.sakura.ne.jp/ 
　 　                     http://www5.ocn.ne.jp/~minute/tab/index.html 
unDonut_g       gae       http://syoboi.jp/wiki/index.php?unDonut_g 
unDonut+        座布団    消失 //tekito.genin.jp/undonut+.html 

※Donut,DonutR,DonutはunDonut Navigatorにてソース&exeが再配布されています
	http://www.geocities.jp/undonut_navi/download.html

※多少反則ですが、http://web.archive.org/ で検索すれば、当時の
ページのいくつかをみることができるかもしれません。



■ unDonut

　ミニット氏の作られていた unDonut は release13 test10 というのが
最新版となるのですが、公開されているソースが release13 test1付近の
ものだったため、test2～10で増えた機能については unDonut+ および
unDonut+mod ではいくつか対応しているものの未対応の機能があります。

大きな違いとしては release13 からのドッキングバーの機能が廃止され
release12と同様のエクスプローラーバーに戻されています。

その他、r13test10で取れてるバグで+modで取れていないものもあったり
しますし…逆に+, +mod で取れてるバグがあったり追加機能もありますが。
(dfgオートセーブをしている場合とかは+modのほうが安定してるハズ)



■意見・要望・バグ報告・簡易サポート・免責

作者(原著者＆改変者)は、このソフトによって生じた如何なる損害にも、
修正や更新も、責任を負わないこととします。
使用にあたっては、自己責任でお願いします。

ろくに確認作業せずリリースしていて恥ずかしながらエンバグも結構多いの
で...人柱覚悟はもってもらっておいたほうがよいです。

私個人は主に 自宅 vista64(ie8), 職場 xp32+ie8 という環境で、
ie と半々くらいの割合で使用といったところです。

unDonut を使いこなしているとはいいがたく使っていない機能もたくさんあり、
なかなか不具合に気づいていなかったりします。
ソースもまだまだよくわかっていない状態。

もし、バグ報告をしていただけるようでしたら、

・undonut.undo.jpのバグ報告掲示板
・2ch の undonut part?? スレ

を覗いているので、そちらでしていただけると助かります。
(ただ申し訳ないですが、文章書くの遅くアクティブな人間ではないので
レス等はあまり期待しないでやってください。バグが取れた版がでたら
それが返事と...)

そのとき +modバージョンや使用os&ieバージョンを示していただけると
助かります。

また、わかっている範囲での発生頻度/条件や、具体的な再現方法も教えて
もらえるとありがたいです。

webページ表示に絡んだものの場合、具体的にバグの発生するurlやクリック
箇所(場合によっては検索文字列や選択範囲)等もわかれば直ぐに試せるので
非常にありがたいです。
（100%発生にみえるバグでもたまたま大丈夫なパターンもありえ、実際それ
　でしばらく放置してたバグもあり...）

※正直、すぐにこちらで再現しない/再現方法のわからないバグは、
　後回し/放置になってしまいます。
