#!/bin/bash

echo 'Content-Type: text/html\r\n\r\n'

declare -A param   
while IFS='=' read -r -d '&' key value && [[ -n "$key" ]]; do
    param["$key"]=$value
done <<<"${QUERY_STRING}&"

key=h?FILENAME
a="${param[$key]}"
b=$(pwd)
var=$(cat $b/html/$a)
echo "$var"