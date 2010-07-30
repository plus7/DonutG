////////////////////////////////////////////////////////////////////////////
【 ソフト名 】DonutG
【バージョン】0.2.6
【  作成日  】2010/07/29
【改  変  者】plus7
【 必要環境 】日本語版 WindowsXP, Vista, 7
　　　　　　　XULRunner1.9.2 以上
【 製作環境 】Microsoft Visual Studio 2008 ATL8/WTL8.0
【 動作確認 】Windows 7 + XULRunner 1.9.2
【 配布形態 】フリーウェア
【一時配布元】http://donut.tnose.net/
【転　　　載】連絡一切不要
【主な著作者】  2000-2001 MB        Donut
　　　　　　　　          D0GSt0re  DonutR
　　　　　　　　2001-2003 UH        DonutP
　　　　　　　　2003-2006 ミニット  unDonut
　　　　　　　　2007      座布団    unDonut+
　　　　　　　　2007-2009 +mod      unDonut+mod / 64unDonut
　　　　　　　　2010      plus7     DonutG
【ライセンス】一部にMPL1.1でライセンスされたコードを含みます．
////////////////////////////////////////////////////////////////////////////

■はじめに
DonutG は Geckoエンジンを用いたタブブラウザで，
Internet Explorer に似せたインターフェイス(概観および操作性)で豊富な機能
と程よいカスタマイズ性を持っています．


■ 免責
作者(原著者＆改変者)は，このソフトウェアを使うことによって生じた
いかなる損害にも，修正や更新も，責任を負いません．
使用にあたっては，自己責任でお願いします．


■新規インストール方法

□Step1. XULRunnerの導入
http://releases.mozilla.org/pub/mozilla.org/xulrunner/releases/
からバージョン1.9.2系列のXULRunner(xulrunner-1.9.2.??.en-US.win32.zip)を
ダウンロードし，お好みの位置に解凍します．次に，
コマンドプロンプトで解凍した位置に移動し，
xulrunner.exe --register-user
というコマンドを実行してください．これでシステムへのGeckoエンジンの
登録が完了します．

□Step2. DonutG本体の導入
ダウンロードしたファイル(donutg.zip)を解凍ツールを利用して解凍します．
作成されたフォルダをお好みの位置に配置します．

以上で基本的な導入は完了です．
DonutGを起動するにはDonutG.exeを実行してください．


■上書きインストール
新しいバージョンを既存のDonutG環境へ上書きする場合，
ご自身の*.iniファイルを誤って上書きしないよう，気をつけてください．

同梱物では，
- DonutG.ini               全般(随時更新)
- Menu.ini                  メニュー情報(随時更新)
- skin/default/ToolBar.ini  ツールバーの配置情報(随時)
- MouseEdit.ini             マウスボタン設定やマウスジェスチャ設定(オプションで変更)
- CloseURL.ini              url抑止(オプション等で変更)
- search/search.ini         検索エンジン設定(手で変更, 拡張プロパティで変更)
- menu/usermenu.txt         ユーザー設定のメニュー(手で変更)
- その他，配布物のファイル名で弄られているもの．(スクリプトやらプラグインやら)
あたりは気をつけてください．

これら以外の*.exeやdoc/,help/に関しては基本上書きを想定しています．

なお，バージョンアップの具合によっては，iniファイルの中も変わりますので，
場合によっては，新しいiniから設定しなおすか，古いiniに新しい設定を反映
してもらわないといけない場合があるかもしれません．


■ アンインストール方法
まずsetup.exeを実行して左下の全解除ボタンを実行します．
続いてDonutGをインストールしたフォルダごと削除してアンインストール完了です．
XULRunnerの登録を解除したい場合は，xulrunner.exe --unregister-user
をすればよいはずなのですが，なぜか消えないのでレジストリエディタを
使って適当に消してください．


■ ファイル
既知のバグは doc/bugs.txt
更新履歴は   doc/history.txt
をみてください．

その他，配布ファイルは以下のようになっています．

Readme.txt      このファイル
DonutG.exe      32ビット版exe
setup.exe       32ビット版の関連付けやDonutP-APIのon/off(DonutP-APIは設定しても動作しません)
Menu.ini        メニュー設定
MouseEdit.ini   マウス/マウスジェスチャ設定
CloseURL.ini    ポップアップ抑止url(unDonut r13test10付属のもの)

doc/            ドキュメント関係
help/           アプリが参照するヘルプファイルを入れるフォルダ
css/            切り替え用css
favoritegroup/  お気に入りグループ用
menu/           ユーザーメニュー
panel/          エクスプローラーバーのパネルの定義
plugin/         32ビット版のプラグイン
script/         スクリプト
search/         サーチエンジンリスト
skin/           スキン
(toolbar/       ツールバー用)


■ヘルプについて
ヘルプはありません．勘で使ってください．

祖先である unDonut,DonutP の解説サイトが公開されていますので
参考になると思いますが，Gecko化に伴って大幅に改変している部分があります
ので注意してください．

unDonutNavigator          http://www.geocities.jp/undonut_navi/
Donut食った？             http://www.geocities.jp/donut1817/tabbrowser/undonut/
めるてぃの小部屋          http://tabbrowser.info/dwimordene.html
unDonut(+mod)まとめwiki   http://www31.atwiki.jp/undonut_mod/


■ IE系Donutとの非互換
基本的に互換性は完全に無視して作っています．
特にお気に入りの拡張プロパティは全滅です．あしからず．


■ Vista/7
Vistaアプリらしくするために，そろそろ設定ファイルをexeと同じところに置くのをやめたいところです．
関連付けもVista以降だとUACのせいで動かないみたいですね．


■想定問答集

□XULRunnerのインストールが面倒くさい
Firefox3以降はFirefoxのGeckoエンジンが外部からも利用できるので，自動的に検知する仕組みを
作りたいところです．XULRunner同梱版は私が面倒くさい…．
