# Microsoft Developer Studio Project File - Name="Merge7z313" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Merge7z313 - Win32 UnicodeDebug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Merge7z313.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Merge7z313.mak" CFG="Merge7z313 - Win32 UnicodeDebug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Merge7z313 - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Merge7z313 - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Merge7z313 - Win32 UnicodeDebug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Merge7z313 - Win32 UnicodeRelease" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Merge7z313 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\Merge7z313___Win32_Release"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z313___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\..\..\7z313" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7z313_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../Build/MergeRelease/Merge7z313.dll"

!ELSEIF  "$(CFG)" == "Merge7z313 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\Merge7z313___Win32_Debug"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z313___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "..\..\..\7z313" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7z313_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"../Build/MergeDebug/Merge7z313.pdb" /debug /machine:I386 /out:"../Build/MergeDebug/Merge7z313.dll"
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "Merge7z313 - Win32 UnicodeDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeDebug"
# PROP BASE Intermediate_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeDebug"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "..\..\..\7z313" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "..\..\..\7z313" /D "MERGE7z313_EXPORTS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /map /debug /machine:I386 /out:"../Build/MergeDebug/Merge7z313.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"../Build/MergeUnicodeDebug/Merge7z313U.pdb" /debug /machine:I386 /out:"../Build/MergeUnicodeDebug/Merge7z313U.dll"
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "Merge7z313 - Win32 UnicodeRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeRelease"
# PROP BASE Intermediate_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeRelease"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z313___Win32_UnicodeRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\7z313" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\..\..\7z313" /D "MERGE7z313_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../Build/MergeRelease/Merge7z313.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../Build/MergeUnicodeRelease/Merge7z313U.dll"

!ENDIF 

# Begin Target

# Name "Merge7z313 - Win32 Release"
# Name "Merge7z313 - Win32 Debug"
# Name "Merge7z313 - Win32 UnicodeDebug"
# Name "Merge7z313 - Win32 UnicodeRelease"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Merge7z.def
# End Source File
# Begin Source File

SOURCE=.\Merge7z.h
# End Source File
# Begin Source File

SOURCE=.\Merge7z311.cpp
# End Source File
# Begin Source File

SOURCE=.\Merge7zCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Merge7zCommon.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tools.cpp
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Ui\Gui\FM.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\UI\GUI\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Ui\Gui\resource.rc
# End Source File
# End Group
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileIO.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\PropVariant.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\PropVariant.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\PropVariantConversions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\PropVariantConversions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\ResourceString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\ResourceString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Synchronization.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Synchronization.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z313\Common\IntToString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\IntToString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\Lang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\Lang.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\StdInStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\StdInStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\String.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\String.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\StringConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\StringConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\TextConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\TextConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\UTFConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\UTFConvert.h
# End Source File
# End Group
# Begin Group "7zip Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Ui\Agent\ArchiveExtractCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\UI\Agent\ArchiveExtractCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Ui\Agent\ArchiveUpdateCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\UI\Agent\ArchiveUpdateCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Control\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Control\Dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\ExtractCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\ExtractCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Common\FileStreams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Common\FileStreams.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\FormatUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\FormatUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\LangUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\LangUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\Resource\MessagesDialog\MessagesDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\OpenCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\OpenCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\Resource\OverwriteDialog\OverwriteDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\Resource\PasswordDialog\PasswordDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\Resource\PasswordDialog\PasswordDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\Resource\ProgressDialog2\ProgressDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\Resource\ProgressDialog2\ProgressDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Registry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Registry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\RegistryUtils.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\RegistryUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\FileManager\UpdateCallback100.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Window.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\Window.h
# End Source File
# End Group
# Begin Group "Extract"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Ui\Common\ExtractingFilePath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\UI\Common\ExtractingFilePath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileDir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileDir.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileFind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileFind.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Windows\FileName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Common\FilePathAutoRename.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Common\FilePathAutoRename.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\Vector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\Vector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\Wildcard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z313\Common\Wildcard.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\7z313\7zip\Ui\Gui\7zG.exe.manifest
# End Source File
# End Target
# End Project
