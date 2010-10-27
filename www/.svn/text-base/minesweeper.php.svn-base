<?php
$args = '';
$argc = 0;
$input = array();
if (isset($_GET['dim'])) {
	$input = $_GET['dim'];
	if (!is_array($input)) $input = explode(',', $input);
}
foreach ($input as $num) {
	$num = intval($num);
	if (!$num) continue;
	++$argc;
	$args .= escapeshellarg($num).' ';
}
if ($argc == 1) {
	$args = ''; $argc = 0;
} elseif ($argc > 4) {
	$args = ''; $argc = 0;
}
set_time_limit(30);
$s = shell_exec('/home/mortal/workspace/minesweeper/minesweeper --ai -F '.$args);
$s = preg_replace('/\033\[(?:A|(?:\d+;?)*m)?/', '', $s);
echo $s;
?>
