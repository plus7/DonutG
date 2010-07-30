
NemuBarForDonutPps.dll: dlldata.obj NemuBarForDonutP_p.obj NemuBarForDonutP_i.obj
	link /dll /out:NemuBarForDonutPps.dll /def:NemuBarForDonutPps.def /entry:DllMain dlldata.obj NemuBarForDonutP_p.obj NemuBarForDonutP_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del NemuBarForDonutPps.dll
	@del NemuBarForDonutPps.lib
	@del NemuBarForDonutPps.exp
	@del dlldata.obj
	@del NemuBarForDonutP_p.obj
	@del NemuBarForDonutP_i.obj
