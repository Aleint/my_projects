#!/bin/bash

echo -e "Download file size: 10 kb"

curl --trace-time -v -o /dev/null http://192.168.82.78/ipsec/file_10kb

echo -e "\n\nDownload file size: 100 kb"

curl --trace-time -v -o /dev/null http://192.168.82.78/ipsec/file_100kb

echo -e "\n\nDownload file size: 1 mb"

curl --trace-time -v -o /dev/null http://192.168.82.78/ipsec/file_1mb

echo -e "\n\nDownload file size: 10 mb"

curl --trace-time -v -o /dev/null http://192.168.82.78/ipsec/file_10mb

echo -e "\n\nDownload file size: 100 mb"

curl --trace-time -v -o /dev/null http://192.168.82.78/ipsec/file_100mb
