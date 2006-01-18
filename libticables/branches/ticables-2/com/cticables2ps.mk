
cticables2ps.dll: dlldata.obj cticables2_p.obj cticables2_i.obj
	link /dll /out:cticables2ps.dll /def:cticables2ps.def /entry:DllMain dlldata.obj cticables2_p.obj cticables2_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del cticables2ps.dll
	@del cticables2ps.lib
	@del cticables2ps.exp
	@del dlldata.obj
	@del cticables2_p.obj
	@del cticables2_i.obj
