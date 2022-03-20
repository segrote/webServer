#!/bin/bash

saveIFS=$IFS
IFS='=&'
parm=($QUERY_STRING)
IFS=$saveIFS
a=$parm[1]
b=$parm[3]
VAR="$a$b"

echo 'Content-Type: text/html\r\n\r\n'
echo '<!DOCTYPE html>'
echo '<html>'
echo '<head>'
echo '<title>Result</title>'
echo '</head>'
echo '<body>'
echo '<h1>'
echo "$VAR"
echo '</h1>'
echo '</body>'
echo '</html>'