Commande ifshow

ifshow est un programme en C qui affiche les informations des interfaces réseau disponibles sur une machine. Il permet de lister les préfixes IPv4 et IPv6 des interfaces, ainsi que leurs masques sous forme de notation CIDR (par exemple, /24).

Le programme accepte deux options principales :

1. Afficher une interface réseau spécifique

./ifshow -i <ifname>

<ifname> : Nom de l'interface (par exemple, eth0).

Affiche les préfixes IPv4 et IPv6 ainsi que leurs masques pour l'interface spécifiée.

2. Afficher toutes les interfaces réseau

./ifshow -a

Liste toutes les interfaces réseau disponibles avec leurs adresses IPv4/IPv6 et masques.