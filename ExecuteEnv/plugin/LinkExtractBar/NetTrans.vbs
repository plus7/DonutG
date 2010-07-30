On Error Resume Next
Set objArg = WScript.Arguments
set NTIECatcher = CreateObject("NTIEHelper.NTIEAddUrl")
If err=0 And objArg.Count >= 2 Then
	Set objFileSystem = CreateObject("Scripting.FileSystemObject")
	Set objFile       = objFileSystem.OpenTextFile(objArg(0))
	Redim ary(objArg(1))
	Redim mark(objArg(1))
	For i=0 To objArg(1)-1
		ary(i) = objFile.ReadLine
		mark(i) = CStr(i)
	Next
	call NTIECatcher.AddList("", ary(0), mark(0))
End If