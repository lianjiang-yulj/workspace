mysql -u root -p

use mysql;
select user,host,authentication_string from mysql.user;


CREATE USER 'flowable'@'%' IDENTIFIED BY 'flowable';

update user set host='%' where user='flowable';

SELECT User, Host from mysql.user;
#GRANT ALL PRIVILEGES ON *.* TO 'flowable'@'localhost';
GRANT ALL PRIVILEGES ON *.* TO 'flowable'@'%';

FLUSH PRIVILEGES;
source /DockerDataDir/jeect-boo-db.conf/flowable-mysql-5.7.sql;




