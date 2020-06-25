<?php
	//与libcurl配对的php脚本禁止用 "</br>" 作换行
	$name = $_POST['username'];
	$age = $_POST['userage'];
	echo "username : ", $name, "\n";
	echo "userage : ", $age, "\n";
?>