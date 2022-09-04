<?php
// Error report = for debugging
ini_set('display_errors',1);
error_reporting(E_ALL);

$target_dir = "/projects/uploads/";

if(isset($_POST["submit"]))
{
    $allowed = array('c', 'tar', 'php');

    $check = $_FILES['fileToUpload']['name'];

    // get the file extension
    $ext = strtolower(pathinfo($check, PATHINFO_EXTENSION));

    if(in_array($ext,$allowed) )
    {
      $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
      if (checkIfFileExist($target_file))
      {
        echo "File already exist!";
        return;
      }

      $fileSize = $_FILES["fileToUpload"]["size"];
      if (!isFileSizeOk($fileSize))
      {
        echo "File is to big!";
        return;
      }

      if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file))
      {
        echo "File Uploaded!";
      }
      else
      {
        echo "Error on uploading your file!";
      }

    }
    else
    {
        echo "File not permited!";
    }
}
?>

<?php
function checkIfFileExist($fName)
{
  return file_exists($fName);
}
?>

<?php
function isFileSizeOk($fSize)
{
  if ($fSize > 500000)
  {
    return false;
  }
  return true;
}
?>