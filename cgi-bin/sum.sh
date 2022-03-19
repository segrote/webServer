#!/bin/bash

a=${QUERY_STRING//&/;}
b=${QUERY_STRING//&/;}
VAR="$a$b"

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