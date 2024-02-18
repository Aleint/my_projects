#!/bin/bash

echo "flush of the states"
ip xfrm state flush

echo "flush of the policies"
ip xfrm policy flush

if [ !$1 ]; then

    ip_address_A="192.168.82.97"
    ip_address_B="192.168.82.78"

    echo "ip address A: $ip_address_A"
    echo "ip address B: $ip_address_B"

    echo -e "\nESP with AES-128-CBC"

    ip xfrm state add src $ip_address_A dst $ip_address_B proto esp spi 0x1000 enc aes 0xaa223344556677889900aabbccddeeff
    ip xfrm state add src $ip_address_B dst $ip_address_A proto esp spi 0x2000 enc aes 0xbb223344556677889900aabbccddeeff
    ip xfrm policy add src $ip_address_B dst $ip_address_A dir out tmpl proto esp mode transport level required
    ip xfrm policy add src $ip_address_A dst $ip_address_B dir in tmpl proto esp mode transport level required


    # echo -e "\nESP with AES-128-CBC and HMAC-SHA1"

    # ip xfrm state add src $ip_address_A dst $ip_address_B proto esp spi 0x1000 enc aes 0xaa223344556677889900aabbccddeeff auth sha1 0xbbccddeeff00112233445566778899aabbccddeeff
    # ip xfrm state add src $ip_address_B dst $ip_address_A proto esp spi 0x2000 enc aes 0xbb223344556677889900aabbccddeeff auth sha1 0xaaccddeeff00112233445566778899aabbccddeeff
    # ip xfrm policy add src $ip_address_B dst $ip_address_A dir out tmpl proto esp mode transport level required
    # ip xfrm policy add src $ip_address_A dst $ip_address_B dir in tmpl proto esp mode transport level required

    # echo -e "\nAH with HMAC-SHA1"

    # ip xfrm state add src $ip_address_A dst $ip_address_B proto ah spi 0x3000 auth sha1 0xbbccddeeff00112233445566778899aabbccddeeff
    # ip xfrm state add src $ip_address_B dst $ip_address_A proto ah spi 0x4000 auth sha1 0xaaccddeeff00112233445566778899aabbccddeeff
    # ip xfrm policy add src $ip_address_B dst $ip_address_A dir out tmpl proto ah mode transport level required
    # ip xfrm policy add src $ip_address_A dst $ip_address_B dir in tmpl proto ah mode transport level required

    echo -e "\nESP with AES-128-CBC and AH with HMAC-SHA1"

    # ip xfrm state add src $ip_address_A dst $ip_address_B proto esp spi 0x1000 enc aes 0xaa223344556677889900aabbccddeeff
    # ip xfrm state add src $ip_address_B dst $ip_address_A proto esp spi 0x2000 enc aes 0xbb223344556677889900aabbccddeeff
    # ip xfrm state add src $ip_address_A dst $ip_address_B proto ah spi 0x3000 auth sha1 0xbbccddeeff00112233445566778899aabbccddeeff
    # ip xfrm state add src $ip_address_B dst $ip_address_A proto ah spi 0x4000 auth sha1 0xaaccddeeff00112233445566778899aabbccddeeff
    # ip xfrm policy add src $ip_address_B dst $ip_address_A dir out tmpl proto esp mode transport level required
    # ip xfrm policy add src $ip_address_A dst $ip_address_B dir in tmpl proto esp mode transport level required
    # ip xfrm policy add src $ip_address_B dst $ip_address_A dir out tmpl proto ah mode transport level required
    # ip xfrm policy add src $ip_address_A dst $ip_address_B dir in tmpl proto ah mode transport level required
fi

echo -e "\nstates:"
ip xfrm state

echo -e "\npolicies:"
ip xfrm policy

echo -e "\nrestart of ipsec service"
ipsec restart
