JSON udf plugin for mysql.

usage:

    mysql> select json(JSON_TEXT,"foo.bar");

or you can convert encode like this:

    mysql> select convert(json(JSON_TEXT,"foo.bar") using utf8);

TODO list:

1. ARRAY's support
