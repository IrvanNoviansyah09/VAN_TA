<?php
$hostname = "localhost";
$username = "root";
$password = "";
$database = "dbsensor";

$conn = mysqli_connect($hostname, $username, $password, $database);

if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

echo "Database connection is OK<br>";

// Mendapatkan tanggal dan waktu saat ini
$currentDateTime = date('Y-m-d H:i:s');

if (isset($_POST["temperature"]) && isset($_POST["tempC"]) && isset($_POST["soilMoisture"]) && isset($_POST["phValue"])) {
    $temperature = $_POST["temperature"];
    $tempC = $_POST["tempC"];
    $soilMoisture = $_POST["soilMoisture"];
    $phValue = $_POST["phValue"];

    $sql = "INSERT INTO datasensor (temperature, nilai_stanah, nilai_lembab, ph_tanah, created_at) 
            VALUES ('$temperature', '$tempC', '$soilMoisture', '$phValue', '$currentDateTime')";

    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully for all sensors";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
} else {
    echo "Incomplete sensor data";
}

mysqli_close($conn);
?>
