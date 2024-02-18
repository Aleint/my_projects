echo -e "\e[1;31;5msha256 - 100 b\e[0m"
openssl speed -rand file.100b sha256

echo -e "\n\e[1;31;5msha256 - 10 kb\e[0m"
openssl speed -rand file.10kb sha256

echo -e "\n\e[1;31;5msha256 - 1 mb\e[0m"
openssl speed -rand file.1mb sha256

echo -e "\n\e[1;31;5msha256 - 100 mb\e[0m"
openssl speed -rand file.100mb sha256

echo -e "\n\e[1;31;5msha512 - 100 b\e[0m"
openssl speed -rand file.100b sha512

echo -e "\n\e[1;31;5msha512 - 10 kb\e[0m"
openssl speed -rand file.10kb sha512

echo -e "\n\e[1;31;5msha512 - 1 mb\e[0m"
openssl speed -rand file.1mb sha512

echo -e "\n\e[1;31;5msha512 - 100 mb\e[0m"
openssl speed -rand file.100mb sha512
