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

if (isset($_POST["temperature"])) {
    $temperature = $_POST["temperature"];
    $sql = "INSERT INTO suhu_udara (temperature) VALUES ('$temperature')";

    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully for suhu_udara";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
}

if (isset($_POST["tempC"])) {
    $tempC = $_POST["tempC"];
    $sql = "INSERT INTO suhu_tanah (nilai_stanah) VALUES ('$tempC')";

    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully for suhu_tanah";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
}

if (isset($_POST["soilMoisture"])) {
    $soilMoisture = $_POST["soilMoisture"];
    $sql = "INSERT INTO kelembaban (nilai_lembab) VALUES ('$soilMoisture')";

    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully for kelembaban";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
}

if (isset($_POST["phValue"])) {
    $phValue = $_POST["phValue"];
    $sql = "INSERT INTO ph (ph_tanah) VALUES ('$phValue')";

    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully for ph";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
}

mysqli_close($conn);
?>
