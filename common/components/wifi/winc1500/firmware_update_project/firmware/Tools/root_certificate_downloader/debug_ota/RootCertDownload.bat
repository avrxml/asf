@ECHO off
@TITLE	Root Certificate Downloader - Atmel Corporation
setlocal EnableDelayedExpansion
set /a c=0
set seq=
for %%X in (..\crt\*.cer) do (
	set /a c+=1
	@set seq=!seq! %%X
)

root_certificate_downloader -n %c% %seq% -e
