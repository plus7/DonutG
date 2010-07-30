'LinkExtractorプラグインは一つ目(0番目)の引数としてURL一覧ファイルのフルパスを、
'二つ目(1番目)の引数としてURLの個数を与えてスクリプトを呼び出します。
'これらの引数にはWScript.Argument(i)からアクセスできます。
'URL一覧ファイルはURL毎に改行で区切られています。
'これらの情報を利用して、ダウンローダにURLを送るのがスクリプトの役割です。'

On Error Resume Next
Set objArg = WScript.Arguments
Set Irvine = CreateObject("Irvine.Api")
If err=0 And objArg.Count >= 1 Then
	Set objFileSystem = CreateObject("Scripting.FileSystemObject\")
	Set objFile       = objFileSystem.OpenTextFile(objArg(0))
	Dim strURLs
	strURLs = objFile.ReadAll()
	call Irvine.AddUrl(strURLs,1)
End If