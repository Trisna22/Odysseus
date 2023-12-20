@echo off

@REM from https://0xpat.github.io/Malware_development_part_1/

makecert -r -pe -n "CN=MalwreBytes CA" -ss CA -sr CurrentUser -a SHA256 -cy authority -sky signature -sv MalwrCA.pvk MalwrCA.cer
certutil -user -addstore Root MalwrCA.cer
makecert -pe -n "CN=MalwreBytes CA" -a sha256 -cy end -sky signature -ic MalwrCA.cer -iv MalwrCA.pvk -sv MalwrCert.pvk MalwrCert.cer
pvk2pfx -pvk MalwrCert.pvk -spc MalwrCert.cer -pfx MalwrCert.pfx
signtool sign /v /fd SHA256 /f MalwrCert.pfx /t http://timestamp.digicert.com/scripts/timstamp.dll TARGET.exe
