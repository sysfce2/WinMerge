<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: About', TAB_ABOUT);
  $stablerelease = $page->getStableRelease();
?>
<h2>About WinMerge</h2>
<p>WinMerge is an Open Source (<a href="http://www.gnu.org/licenses/gpl-2.0.html">GPL</a>) visual text file differencing and merging tool for Windows. It is highly useful for determining what has changed between project versions, and then merging changes between versions.</p>
<?php $page->printDownloadNow(); ?>
<h3>Features</h3>
<ul>
  <li>Visual differencing and merging of text files</li>
  <li>Flexible editor with syntax highlighting, line numbers and word-wrap</li>
  <li>Handles Windows, Unix and Mac text file formats</li>
  <li>Unicode support</li>
  <li>Difference pane shows current difference in two vertical panes</li>
  <li>Location pane shows map of files compared</li>
  <li>Highlights differences inside lines in file compare</li>
  <li>Regular Expression based file filters in directory compare allow excluding and including items</li>
  <li>Moved lines detection in file compare</li>
  <li>Creates patch files</li>
  <li>Resolve conflict files</li>
  <li>Shell Integration (supports 64-bit Windows versions)</li>
  <li>Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
  <li>Archive file support using 7-Zip</li>
  <li>Plugin support</li>
  <li>Localizable interface</li>
  <li><a href="/docs/manual/">Online manual</a> and installed HTML Help manual</li>
</ul>

<h3>Screenshots</h3>
<p><a href="screenshots/screenshot.png" target="_blank"><img class="thumbnail" src="screenshots/screenshot_small.png" alt="File Comparison" border="0"></a>
<a href="screenshots/foldercmp.png" target="_blank"><img class="thumbnail" src="screenshots/foldercmp_small.png" alt="Folder Comparison Results" border="0"></a></p>
<p>See the <a href="screenshots/">screenshots page</a> for more screenshots.</p>

<h3>Installation</h3>
<p>Just download the <a href="<?php echo $stablerelease->getDownload('setup.exe');?>">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="<?php echo $stablerelease->getDownload('exe.zip');?>">binary zip package</a> into the desired location, and run
<code>WinMergeU.exe</code> (except for Windows 98/ME users who should use <code>WinMerge.exe</code>).</p>
<p>For more information, please see the <a href="/docs/manual/Installing.html">Installation section</a> of the <a href="/docs/manual/">Manual</a>.</p>

<h3>Using WinMerge</h3>
<p>For information on using WinMerge, see our <a href="/docs/manual/">online Manual</a>,
or you can use the local help file (just press <code>F1</code> inside WinMerge) for offline use.</p>

<p>Here is the current <a href="/docs/changelog.php">change log</a>.</p>
<?php
  $page->printFoot();
?>
