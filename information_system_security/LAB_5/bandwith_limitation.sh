
iptables -F INPUT
iptables -F FORWARD
iptables -F OUTPUT

iptables -P FORWARD DROP

iptables -A FORWARD -p tcp -s 192.168.188.158 --dport 80 -j ACCEPT
iptables -A FORWARD -p tcp -d 192.168.188.158 --sport 80 -j ACCEPT
iptables -I FORWARD 2 -p tcp -d 192.168.188.158 --sport 80 --syn -j DROP

#public service
iptables -A FORWARD -p tcp -d 192.168.188.158 --dport 22 -j ACCEPT
iptables -A FORWARD -p tcp -s 192.168.188.158 --sport 22 --syn -j DROP
iptables -A FORWARD -p tcp -s 192.168.188.158 --sport 22 -j ACCEPT

#icmp
#iptables -A FORWARD -p icmp -s 192.168.188.158 --icmp-type echo-request -j ACCEPT
#iptables -A FORWARD -p icmp -d 192.168.188.158 --icmp-type echo-reply -j ACCEPT
#iptables -A FORWARD -p icmp -d 192.168.188.158 --icmp-type echo-request -j ACCEPT
#iptables -A FORWARD -p icmp -s 192.168.188.158 --icmp-type echo-reply -j ACCEPT

#bandwith limitation
#iptables -D FORWARD 1
iptables -A FORWARD -p icmp -s 192.168.188.158 --icmp-type echo-request -m limit --limit 10/minute --limit-burst 10 -j ACCEPT
iptables -A FORWARD -p icmp -d 192.168.188.158 --icmp-type echo-reply -j ACCEPT
