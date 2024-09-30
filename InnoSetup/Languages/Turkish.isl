; *** Inno Setup version 5.1.0+ Turkish messages ***
; Language     " T�k�e"               ::::::    Turkish
; Translate by " �eviren "            ::::::    Adil YILDIZ
; E-Mail       " Elektronik Posta "   ::::::    adil@kde.org.tr
; Home Page    " Web Adresi "         ::::::    http://www.yildizyazilim.gen.tr
;
; $jrsoftware: issrc/Files/Default.isl,v 1.66 2005/02/25 20:23:48 mlaan Exp $
[LangOptions]
LanguageName=T<00FC>rk<00E7>e
LanguageID=$041f
LanguageCodePage=0
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8
[Messages]

; *** Application titles
SetupAppTitle=Kur
SetupWindowTitle=%1 - Kur
UninstallAppTitle=Kald�r
UninstallAppFullTitle=%1 Kald�r

; *** Misc. common
InformationTitle=Bilgi
ConfirmTitle=Sorgu
ErrorTitle=Hata

; *** SetupLdr messages
SetupLdrStartupMessage=Bu %1'i kuracak . Devam etmek istiyor musunuz?
LdrCannotCreateTemp=Bellek dosyas� olu�turulamad�. Kurulum iptal edildi
LdrCannotExecTemp=Bellek dosyas� a��lamad�. Kurulum iptal edildi

; *** Startup error messages
LastErrorMessage=%1.%n%n Hata %2: %3
SetupFileMissing=%1 adl� dosya kurulum dizininde bulunamad�. L�tfen problemi d�zeltin veya program�n yeni bir kopyas�n� al�n.
SetupFileCorrupt=Kurulum dosyalar� bozulmu�. L�tfen program�n yeni bir kopyas�n� al�n.
SetupFileCorruptOrWrongVer=Kurulum dosyalar� bozulmu� veya kurulumun bu s�r�m� ile uyu�muyor olabilir. L�tfen problemi d�zeltin veya Program�n yeni bir kopyas�n� al�n.
NotOnThisPlatform=Bu program %1'de �al��maz.
OnlyOnThisPlatform=Bu program sadece %1'de �al��t�r�lmal�d�r.
OnlyOnTheseArchitectures=Bu program sadace belirli mimarideki i�lemcilere i�in olan Windows'larda �al���r:%n%n%1
MissingWOW64APIs=Kullanm�� oldu�unuz Windows s�r�m� 64-bit kurulum i�in gerekli olan gereksinimlere sahip de�il. Bu problemi ortadan kald�rmak i�in l�tfen Service 1 y�leyin %1.
WinVersionTooLowError=Bu program� �al��t�rmak i�in %1 s�r�m %2 ve sonras� gereklidir.
WinVersionTooHighError=Bu program %1 s�r�m %2 ve sonras�nda �al��maz.
AdminPrivilegesRequired=Bu program� kurmak i�in y�netici olarak oturum a�man�z gerekir.
PowerUserPrivilegesRequired=Bu program� kurabilmek i�in Administrator veya Y�netici Grubu �yesi olarak giri� yapman�z gerekli.
SetupAppRunningError=Kur %1 in �al��t���n� tesbit etti.%n%nL�tfen b�t�n programlar� kapat�n sonra devam etmek i�in Tamam'a veya ��kmak i�in �ptal'e bas�n.
UninstallAppRunningError=Kald�r %1'in �al��t���n� tespit etti.%n%nL�tfen b�t�n programlar� kapat�n sonra devam etmek i�in Tamam'a veya ��kmak i�in �ptal'e bas�n.

; *** Misc. errors
ErrorCreatingDir=Kur " %1 " dizinini olu�turamad�.
ErrorTooManyFilesInDir=" %1 " dizininde dosya olu�turulamad�. ��nk� dizin �ok fazla dosya i�eriyor.

; *** Setup common messages
ExitSetupTitle=Kurdan ��k
ExitSetupMessage=Kur tamamlanmad�. �imdi ��karsan�z program kurulmayacak.%n%nBa�ka bir zaman Kur'u tekrar �al��t�rarak kuruluma devam edebilirsiniz.%n%nKurdan ��kmak istedi�inizden eminmisiniz?
AboutSetupMenuItem=Kur H&akk�nda...
AboutSetupTitle=Kur Hakk�nda
AboutSetupMessage=%1 s�r�m %2%n%3%n%n%1 internet:%n%4
AboutSetupNote=
TranslatorNote=�yi bir kurulum program� ar�yorsan�z buldunuz...%nhttp://www.yildizyazilim.gen.tr

; *** Buttons
ButtonBack=< G&eri
ButtonNext=�&leri >
ButtonInstall=&Kur
ButtonOK=Tamam
ButtonCancel=�ptal
ButtonYes=E&vet
ButtonYesToAll=T�m�ne E&vet
ButtonNo=&Hay�r
ButtonNoToAll=T�m�ne Ha&y�r
ButtonFinish=&Son
ButtonBrowse=&G�zat...
ButtonWizardBrowse=G�za&t...
ButtonNewFolder=Ye&ni Klas�r Olu�tur

; *** "Select Language" dialog messages
SelectLanguageTitle=Kurulum Dilini Se�in
SelectLanguageLabel=Kurulum s�ras�nda kullanaca��n�z dili se�in:

; *** Common wizard text
ClickNext=Devam etmek i�in �leri'ye , ��kmak i�in �ptal 'e bas�n.
BeveledLabel=Inno Setup 5.1+ T�rk�e
BrowseDialogTitle=Klas�re G�zat
BrowseDialogLabel=A�a��daki listeden bir klas�r se�ip, Tamam tu�una bas�n.
NewFolderName=Yeni Klas�r

; *** "Welcome" wizard page
WelcomeLabel1=[name] kur program�na ho�geldiniz.
WelcomeLabel2=Kur �imdi [name/ver] '� bilgisayar�n�za kuracak.%n%nDevam etmeden �nce �al��an di�er b�t�n programlar�  kapatman�z �iddetle tavsiye edilir. Bu, kurulum esnas�nda olu�abilecek hatalar� �nlemeye yard�mc� olur.

; *** "Password" wizard page
WizardPassword=�ifre
PasswordLabel1=Bu kurulum �ifre korumal�d�r.
PasswordLabel3=L�tfen �ifreyi girin. Sonra �leri'ye basarak devam edin. L�tfen �ifreyi girerken B�y�k -K���k harflere dikkat edin.
PasswordEditLabel=&�ifre:
IncorrectPassword=Girdi�iniz �ifre hatal� . L�tfen tekrar deneyiniz.

; *** "License Agreement" wizard page
WizardLicense=Lisans Anla�mas�
LicenseLabel=L�tfen devam etmeden �nce a�a��daki �nemli bilgileri okuyunuz.
LicenseLabel3=Lisans Anla�mas�n� l�tfen okuyun. Kuruluma devam edebilmek i�in bu anla�man�n ko�ullar�n� kabul etmelisiniz
LicenseAccepted=Anla�may� Kabul Ediyorum.
LicenseNotAccepted=Anla�may� Kabul Etmiyorum.

; *** "Information" wizard pages
WizardInfoBefore=Bilgi
InfoBeforeLabel=L�tfen devam etmeden �nce a�a��daki �nemli bilgileri okuyunuz.
InfoBeforeClickLabel=Kur ile devam etmeye haz�r oldu�unuz zaman �leri'yi t�klay�n.
WizardInfoAfter=Bilgi
InfoAfterLabel=L�tfen devam etmeden �nce a�a��daki �nemli bilgileri okuyunuz.
InfoAfterClickLabel=Kur ile devam etmeye haz�r oldu�unuz zaman �leri'yi t�klay�n.

; *** "User Information" wizard page
WizardUserInfo=Kullan�c� Bilgileri
UserInfoDesc=L�tfen bilgilerinizi giriniz.
UserInfoName=K&ullan�c� Ad�:
UserInfoOrg=�i&rket:
UserInfoSerial=Seri Numaras�:
UserInfoNameRequired=Bir isim girmelisiniz.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Kurulacak dizini se�in.
SelectDirDesc=[name] hangi dizine kurulsun?
SelectDirLabel3=Kurulum [name]'i a�a��daki dizine kuracak.
SelectDirBrowseLabel=Devam etmek i�in, �leri'ye bas�n. Ba�ka bir dizin se�mek istiyorsan�z, G�zat'a bas�n.
DiskSpaceMBLabel=Bu program i�in en az [mb] MB disk alan� gereklidir.
ToUNCPathname=Kur UNC  yollar�na kurulum yapamaz. E�er A� �zerinde kurulum yapmaya �al���yorsan�z. Bir a� s�r�c�s� tan�tman�z gerekir.
InvalidPath=S�r�c� ismi ile birlikte tam yolu girmelisiniz; �rne�in :%nC:\APP
InvalidDrive=Se�ti�iniz s�r�c� bulunamad�. L�tfen ba�ka bir s�r�c� se�in.
DiskSpaceWarningTitle=Yetersiz disk alan�
DiskSpaceWarning=Kur i�in en az %1 KB disk alan� gerekmektedir. Fakat se�ili diskte %2 KB bo� alan mevcuttur.%n%nDevam etmek istiyormusunuz?
DirNameTooLong=Dizin ad� veya you �ok uzun.
InvalidDirName=Dizin ad� ge�ersiz.
BadDirName32=Dizin ad� takib eden karakterlerden her hangi birini i�eremez:%n%n%1
DirExistsTitle=Dizin Mevcuttur
DirExists=Dizin:%n%n%1%n%nmevcut. Bu dizine kurmak istedi�inizden eminmisiniz?
DirDoesntExistTitle=Dizin Mevcut de�il
DirDoesntExist=Dizin:%n%n%1%n%nmevcut de�il. Bu dizini olu�turmak istedi�inizden eminmisiniz?

; *** "Select Components" wizard page
WizardSelectComponents=Bile�en Se�
SelectComponentsDesc=Hangi bile�enler kurulsun?
SelectComponentsLabel2=Kurmak istedi�iniz bile�enleri se�in; istemediklerinizi temizleyin.Devam etmeye haz�r oldu�unuz zaman �leri'ye t�klay�n.
FullInstallation=Tam Kurulum
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=K���k Kurulum
CustomInstallation=�zel Kurulum
NoUninstallWarningTitle=Mevcut Bile�enler
NoUninstallWarning=Kur a�a��daki bile�enlerin kurulu oldu�unu tespit etti:%n%n%1%n%nBu bile�enleri se�meyerek kald�ramazs�n�z.%n%nYinede devam etmek istiyormusunuz?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Se�ili bile�enler i�in en az [mb] MB disk alan� gerekmektedir.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=�lave G�rev Se�
SelectTasksDesc=Hangi g�revler yerine getirilsin?
SelectTasksLabel2=[name] kurulurken eklemek istedi�iniz g�revleri se�ip �leri'ye t�klay�n.

; *** "Ba�lat Men�s� Klas�r� Se�" sihirbaz sayfas�
WizardSelectProgramGroup=Ba�lat Men�s�nde klas�r se�
SelectStartMenuFolderDesc=Kur program k�sayollar�n� nereye yerle�tirsin?
SelectStartMenuFolderLabel3=Kurulum program�n k�sayollar�n� a�a��daki Ba�lat Men�s� dizinine kuracak.
SelectStartMenuFolderBrowseLabel=Devam etmek i�in, �leri'ye bas�n. Ba�ka bir dizin se�mek istiyorsan�z, G�zat'a bas�n.
MustEnterGroupName=Klas�r ismi girmelisiniz.
GroupNameTooLong=Dizin ad� veya you �ok uzun.
InvalidGroupName=Dizin ad� ge�ersiz.
BadGroupName=Klas�r ad� takib eden karakterlerden her hangi birini i�eremez:%n%n%1
NoProgramGroupCheck2=Ba�lat men�s�nde k�sayol olu�turma

; *** "Ready to Install" wizard page
WizardReady=Kur kurmaya haz�r
ReadyLabel1=Kur [name]'i bilgisayar�n�za kurmak i�in haz�r.
ReadyLabel2a=Kuruluma devam etmek i�in Kur'u , kontrol etmek veya de�i�tirmek i�in Geri'yi t�klay�n.
ReadyLabel2b=Kuruluma devam etmek i�in Kur'u t�klay�n.
ReadyMemoUserInfo=Kullan�c� Bilgisi:
ReadyMemoDir=Hedef Dizin:
ReadyMemoType=Kurulum Tipi:
ReadyMemoComponents=Se�ili Bile�enler:
ReadyMemoGroup=Ba�lat Men�s� :
ReadyMemoTasks=�lave G�revler:

; *** "Kur Haz�lan�yor" sihirbaz sayfas�
WizardPreparing=Kur Haz�rlan�yor
PreparingDesc=Kur [name] Bilgisayar�n�za kurmak i�in haz�rlan�yor.
PreviousInstallNotCompleted=Bir �nceki kurma/kald�rma program�na ait i�lem %ntamamlanmam��.�nceki kurulum i�leminin tamamlanmas� i�in bilgisayar�n�z� %nyeniden ba�latmal�s�n�z.%n%nBilgisayar�n�z kat�l�p-a��ld�ktan sonra, %n[name] adl� uygulamay� kurma i�lemine devam edecek.
CannotContinue=Kur devam edemiyor. L�tfen �ptal'e t�klay�p ��k�n.

; *** "Kuruluyor" sihirbaz
WizardInstalling=Kuruluyor
InstallingLabel=Kur [name]'i bilgisayar�n�za kurarken bekleyin.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] kurulum sihirbaz� tamamlan�yor
FinishedLabelNoIcons=Kur [name]'i bilgisayar�n�za kurma i�lemini tamamlad�.
FinishedLabel=Kur [name]'i bilgisayar�n�za kurma i�lemini tamamlad�. Program� ba�latmak i�in kurulu k�sayollar� t�klay�n.
ClickFinish=Kur'dan ��kmak i�in sona t�klay�n.
FinishedRestartLabel=[name]'in kurulumunu bitirmek i�in, Kur bilgisayar�n�z� yeniden ba�latacak. Biligisayar�n�z yeniden ba�lat�ls�n m�?
FinishedRestartMessage=[name]'in kurulumunu bitirmek i�in, Kur bilgisayar�n�z� yeniden ba�latacak. %n%nBiligisayar�n�z yeniden ba�lat�ls�n m�?
ShowReadmeCheck=Beni Oku dosyas�n� g�rmek istiyorum.
YesRadio=&Evet , Bilgisayar yeniden ba�lat�lls�n.
NoRadio=&Hay�r, Ben sonra yeniden ba�lat�r�m.
; used for example as 'Run MyProg.exe'
RunEntryExec=%1'� �al��t�r
; used for example as 'View Readme.txt'
RunEntryShellExec=%1'� g�r

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Bir sonraki disketi tak�n.
SelectDiskLabel2=%1 numaral� diski tak�p, Tamam'� t�klay�n.%n%nE�er dosyalar Sabit Disk �zerinde ise do�ru yolu yaz�n veya G�zat'� t�klay�n.
PathLabel=&Yol:
FileNotInDir2=" %1 " adl� dosya " %2 " dizininde bulunamad�. L�tfen do�ru diski veya dosyay� se�in.
SelectDirectoryLabel=L�tfen sonraki diskin yerini belirleyin.

; *** Installation phase messages
SetupAborted=Kurulum tamamlanamad�.%n%nL�tfen ya problemi d�zeltin yada Kur'u tekrar �al��t�r�n.
EntryAbortRetryIgnore=Tekrar denemek i�in Tekrar Dene'ye , yinede devam etmek i�in Yoksay'a , kurulumu iptal etmek i�in �ptal'e t�klay�n.

; *** Installation status messages
StatusCreateDirs=Dizinler olu�turuluyor...
StatusExtractFiles=Paketler a��l�yor...
StatusCreateIcons=Program k�sayollar� olu�turuluyor...
StatusCreateIniEntries=INI dosyalar� olu�turuluyor...
StatusCreateRegistryEntries=Registry g�ncelleniyor...
StatusRegisterFiles=Dosyalar sisteme tan�t�l�yor.
StatusSavingUninstall=Kald�r bilgileri kaydediliyor.
StatusRunProgram=Kurulum sonland�r�l�yor...
StatusRollback=Kay�t yeniden d�zenleniyor...

; *** Misc. errors
ErrorInternal2=�� hata: %1
ErrorFunctionFailedNoCode=%1 ba�ar�s�z.
ErrorFunctionFailed=%1 ba�ar�s�z oldu; sat�r  %2
ErrorFunctionFailedWithMessage=%1 ba�ar�s�z oldu ; sat�r  %2.%n%3
ErrorExecutingProgram=%1 adl� dosya �al��t�r�lamad�.

; *** Registry errors
ErrorRegOpenKey=Kay�t Defteri anahtar�n� a�arken hata olu�tu:%n%1\%2
ErrorRegCreateKey=Kay�t Defteri anahtar�n� olu�tururken hata olu�tu:%n%1\%2
ErrorRegWriteKey=Kay�t Defteri anahtar�na yazarken hata olu�tu:%n%1\%2

; *** INI errors
ErrorIniEntry=" %1 " adl� dosyada INI yazma hatas�.

; *** File copying errors
FileAbortRetryIgnore=Yeniden denemek i�in Yeniden Dene'ye, dosyay� atlamak i�in Yoksay'a (�nerilmez), Kurulumu iptal etmek i�in �ptal'e t�klay�n.
FileAbortRetryIgnore2=Yeniden denemek i�in Yeniden Dene'ye , yine de devam etmek i�in Yoksay'a (�nerilmez), Kurulumu �ptal etmek i�in �ptal'e t�klay�n.
SourceIsCorrupted=Kaynak Dosya Bozulmu�
SourceDoesntExist=%1 adl� Kaynak Dosya Mevcut De�il.
ExistingFileReadOnly=Dosya Salt Okunur.%n%nSalt Okunur �zelli�ini kald�r�p yeniden denemek i�in Yeniden Dene'yi , dosyas� atlamak i�in Yoksay'� , Kurulumu iptal etmek i�in �ptal'i t�klay�n.
ErrorReadingExistingDest=Dosyay� okurken bir hata olu�tu :
FileExists=Dosya mevcut.%n%nKur �zerine yazs�n m�?
ExistingFileNewer=Mevcut dosya Kur'un y�klemek istedi�i dosyadan daha yeni. Mevcut dosyay� saklaman�z �nerilir.%n%nMevcut dosya saklans�n m�?
ErrorChangingAttr=Dosyan�n �zelli�i de�i�tirilirken hata olu�tu:
ErrorCreatingTemp=Hedef dizinde dosya olu�turulurken hata olu�tu:
ErrorReadingSource=Kaynak okunurken hata olu�tu:
ErrorCopying=Dosya kopyalan�rken hata olu�tu:
ErrorReplacingExistingFile=Mevcut dosya de�i�tirilirken hata olu�tu:
ErrorRestartReplace=Tekrar de�i�tirme i�lemi ba�ar�s�z oldu:
ErrorRenamingTemp=Hedef dizinde olu�turulacak dosyan�n ad� de�i�tirilirken hata oldu:
ErrorRegisterServer=%1 adl� DLL/OCX sisteme tan�t�lamad�.
ErrorRegisterServerMissingExport=DLL'i Sisteme tan�tma arac� bulunamad�
ErrorRegisterTypeLib=%1 adl� k�t�phane sisteme tan�t�lamad�

; *** Post-installation errors
ErrorOpeningReadme=Beni Oku dosyas� a��l�rken hata olu�tu.
ErrorRestartingComputer=Kur bilgisayar� yeniden ba�latamad�. L�tfen kendiniz kapat�n.

; *** Uninstaller messages
UninstallNotFound=%1 adl� dosya bulunamad�. Kald�r�lamaz.
UninstallOpenError="%1" dosyas� a��lam�yor. Kald�rma i�lemi ger�ekle�tirilemedi.
UninstallUnsupportedVer=%1 adl� Kald�r bilgi dosyas� kald�r�n bu s�r�m� ile uyu�muyor. Kald�r�lamaz
UninstallUnknownEntry=Kald�r Bilgi dosyas�ndaki %1 adl� sat�r anla��lamad�
ConfirmUninstall=%1 ve bile�enlerini kald�rmak istedi�inizden emin misiniz?
UninstallOnlyOnWin64=Bu kurulum sadece 64-bit Windows'larda kald�r�labilir.
OnlyAdminCanUninstall=Kurlumu sadece y�netici yetkisine sahip ki�iler kald�rabilir.
UninstallStatusLabel=%1 adl� program bilgisayar�n�zdan kald�r�l�rken bekleyin...
UninstalledAll=%1 bilgisayar�n�zdan tamamen kald�r�ld�.
UninstalledMost=%1'� kald�rma i�lemi sona erdi.%n%nBaz� bile�enler kald�r�lamam�� olabilir. Bu dosyalar� kendiniz silin.
UninstalledAndNeedsRestart=%1 kald�r�l�lmas� tamamland�, Bilgisayar�n�z� yeniden ba�latmal�s�n�z.%n%nYeniden Ba�lat�lmas�n� istiyor musunuz?
UninstallDataCorrupted="%1" adl� dosya bozuk. Kald�r�lam�yor.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Payla��l�m dosyas�n� sil?
ConfirmDeleteSharedFile2=Sistemde belirtilen payla��lm�� dosya baz� programlar tarf�ndan kullan�l�yor. Kald�r bu payla��lan dosyay� silsin mi?%n%n Bu dosya baz� programlar tafar�ndan kullan�l�yorsa ve silinirse bu programalar d�zg�n bir �ekilde �al��mayabilir. Emin de�ilseniz. Hay�r Butonuna t�klay�n�z.
SharedFileNameLabel=Dosya Ad�:
SharedFileLocationLabel=Dizin:
WizardUninstalling=Kald�rma Durumu
StatusUninstalling=%1 Kald�r�l�yor...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 s�r�m %2
AdditionalIcons=Ek simgeler:
OptionalFeatures=Optional Features:
CreateDesktopIcon=Masa�st� simg&esi olu�tur
CreateQuickLaunchIcon=H�zl� Ba�lat simgesi &olu�tur
ProgramOnTheWeb=%1 Web Sitesi
UninstallProgram=%1 Program�n� Kald�r
LaunchProgram=%1 Program�n� �al��t�r
AssocFileExtension=%2 dosya uzant�lar�n� %1'e ata
AssocingFileExtension=%2 dosya uzant�lar� %1'e atan�yor...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Typical Installation
FullInstallation=Full Installation
CompactInstallation=Compact Installation
CustomInstallation=Custom Installation

;Components
AppCoreFiles=WinMerge Core Files
ApplicationRuntimes=Application Runtimes
UsersGuide=User's Guide
Filters=Filters
Plugins=Plugins (See Plugins.txt)

;Localization Components
Languages=Languages
BulgarianLanguage=Bulgarian menus and dialogs
CatalanLanguage=Catalan menus and dialogs
ChineseSimplifiedLanguage=Chinese (Simplified) menus and dialogs
ChineseTraditionalLanguage=Chinese (Traditional) menus and dialogs
CzechLanguage=Czech menus and dialogs
DanishLanguage=Danish menus and dialogs
DutchLanguage=Dutch menus and dialogs
FrenchLanguage=French menus and dialogs
GermanLanguage=German menus and dialogs
HungarianLanguage=Hungarian menus and dialogs
ItalianLanguage=Italian menus and dialogs
JapaneseLanguage=Japanese menus and dialogs
KoreanLanguage=Korean menus and dialogs
NorwegianLanguage=Norwegian menus and dialogs
PolishLanguage=Polish menus and dialogs
PortugueseLanguage=Portuguese (Brazilian) menus and dialogs
RussianLanguage=Russian menus and dialogs
SlovakLanguage=Slovak menus and dialogs
SpanishLanguage=Spanish menus and dialogs
SwedishLanguage=Swedish menus and dialogs
TurkishLanguage=Turkish menus and dialogs

;Tasks
ExplorerContextMenu=&Enable Explorer context menu integration
IntegrateTortoiseCVS=Integrate with &TortoiseCVS
IntegrateDelphi4=Borland� Delphi &4 Binary File Support
UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
ReadMe=Read Me
UsersGuide=User's Guide
UpdatingCommonControls=Updating the System's Common Controls
ViewStartMenuFolder=&View the WinMerge Start Menu Folder

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?

; Project file description
ProjectFileDesc=WinMerge Project file
