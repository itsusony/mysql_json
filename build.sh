gcc -Wall -fPIC -I/usr/include/mysql -lmysqlclient -shared -o json.so libs/parson.c json.c
cp json.so /usr/lib64/mysql/plugin/
mysql -uroot -p -e "use mysql;drop function if exists json;create function json returns string soname 'json.so';"
