#!/bin/bash

a=${QUERY_STRING//&/;}
var=$(cat ../html/$a)
echo "$var"