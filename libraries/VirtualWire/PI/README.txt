
1/Installtion db mysql
----------------------

sudo apt-get install php5 mysql-server
sudo apt-get install python-mysqldb



2/Creation db mysql
-------------------

mysql -uroot -hlocalhost -p     puis password root
CREATE DATABASE CHIPKIT;
CREATE USER 'edh'@'localhost' IDENTIFIED BY 'edh';
GRANT ALL PRIVILEGES ON CHIPKIT.* TO 'edh'@'localhost';
FLUSH PRIVILEGES;


mysql -uedh -hlocalhost CHIPKIT -p   puis password edh

CREATE TABLE raw_data (
id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
reg_date TIMESTAMP,
source INT(6) UNSIGNED,
type INT(6) UNSIGNED,
value INT(6) UNSIGNED
);


Pour alimenter la base, lancer python PI_SerialChipkit.py



3/Installation server apache
----------------------------

sudo apt-get install apache2 php5 libapache2-mod-php5
copie les fichiers MyChart.php et data.php dans /var/www/chart/
executer sudo chmod -R 777 /var/www/chart/
acceder au site via URL: http://192.168.0.18/chart/MyChart.php

