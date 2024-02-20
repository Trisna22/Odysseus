# ChangeLog
Change log for every push

## (before) 27 aug 2023
Added the implant downloader  

## 6 sept 2023
Page for generating scripts for implant binaries created, backend doesn't work yet.

## 16 feb 2024
ELF implant can now execute BOF retrieved from the server. Also the JobHunter feature has been added to create parallel execution and job management.


```
ToDo:  

+ Client
  - Add feature to make the generator have the payload in a downloadable script,
so that the implants won't download the implant directly.
  - Add implant sleep and jitter options.
  - 

+ Implant
  - Add crypter functions
  - Different payload execution methods (add options to implant generation):
    *  CreateRemoteThread()
    * ...
  - Sleep/jitter features

+ Server
  - Windows implant compiler

+ Beacon Object Files
  - AMSI bypass

```
