On Error Resume Next
Set objArg = WScript.Arguments
set JetCarCatch = CreateObject("JetCar.Netscape")
If err=0 And objArg.Count >= 2 Then
	Set objFileSystem = CreateObject("Scripting.FileSystemObject")
	Set objFile       = objFileSystem.OpenTextFile(objArg(0))
	Redim ary(objArg(1)*2)
	ary(0) = ""
	For i=0 To objArg(1)-1
		ary(i*2+1) = objFile.ReadLine
		ary(i*2+2) = CStr(i)
	Next
	JetCarCatch.AddUrlList ary
End If
