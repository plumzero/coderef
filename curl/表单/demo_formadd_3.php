<?php
	//libcurl 调用脚本
	$contents = $_POST['filename'];
	echo "file contents : ", $contents, "\n";
	
	//执行 demo_formadd_3.html 调用下面的注释脚本，脚本有问题
	// print_r($_FILES['filename']);
	// echo "\n";
	// $fname = $_FILES['filename']['name'];
	// echo "\nfname : ", $fname, "\n";
	// $handle = fopen($_FILES['filename']['tmp_name'], "r");
	// if ($handle == FALSE)
	// {
		
		// echo "open file failed\n";
		// exit(-1);
	// }
	// else
	// {
		// echo "open success\n";
	// }
	
	// while(($buffer = fgets($handle, 4096)) != FASLE)
	// {
		// echo $buffer, "\n";
	// }
	
	// if (!feof($handle))
	// {
		// echo "Error: unexpected fgets() faile\n";
	// }	
	// fclose($handle);
?>