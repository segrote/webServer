#!/bin/bash

echo "Content-Type: text/html\r\n\r\n"
a=${QUERY_STRING}
var=$(cat ../html/$a)
echo "$var"