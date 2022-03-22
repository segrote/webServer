#!/bin/bash

declare -A param   
while IFS='=' read -r -d '&' key value && [[ -n "$key" ]]; do
    param["$key"]=$value
done <<<"${QUERY_STRING}&"

echo 'Content-Type: text/html\r\n\r\n'
echo '<!DOCTYPE html>'
echo '<html>'
echo '<head>'
echo '<title>Result</title>'
echo '</head>'
echo '<body>'
echo '<h1>'
echo "${param["First"]}${param["Second"]}"
echo '</h1>'
echo '</body>'
echo '</html>'