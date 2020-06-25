<?php
	print_r($_FILES);
	echo "file name: ", $_FILES['filename']['name'], "\n";
	echo "file type: ", $_FILES['filename']['type'], "\n";
	echo "file size: ", $_FILES['filename']['size'], "\n";
	
	if(file_exists($_FILES['filename']['tmp_name']) && is_uploaded_file($_FILES['filename']['tmp_name']))
	{
		$user_path = $_SERVER['DOCUMENT_ROOT']."/UploadDir";
		echo "user_path is: ", $user_path, "\n";
		if(!file_exists($user_path)) 
		{
			echo "no download directory, now we make...\n";
			if (mkdir($user_path, 0777, true) == FALSE)
			{
				echo "mkdir directory failed\n";
			}
			else
			{
				echo "mkdir directory success\n";
			}
		} 
		
		$upload_file = $user_path."/".$_FILES['filename']['name'];
		echo "Test ", $upload_file, " is existed...\n";
		if (file_exists($upload_file))
		{
			echo "delete existed file, and upload again...\n";
			if (unlink($upload_file) == FALSE)
			{
				echo "deleted failed\n";
			}
			else
			{
				echo "deleted success\n";
			}
		}
		else
		{
			echo "file not existed\n";
		}
		if (move_uploaded_file($_FILES['filename']['tmp_name'], $upload_file) == FALSE)
		{
			echo "moved failed\n";
		}
		else
		{
			echo "moved success\n";
		}
	} 
	else 
	{ 
		echo "uploaded failed\n";
	}  
?>