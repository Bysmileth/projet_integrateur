Commande ifshow

ifshow est un programme en C qui affiche les informations des interfaces réseau disponibles sur une machine. Il permet de lister les préfixes IPv4 et IPv6 des interfaces, ainsi que leurs masques sous forme de notation CIDR (par exemple, /24).

Le programme accepte deux options principales :

1. Afficher une interface réseau spécifique

./ifshow -i ethX

ethX : Nom de l'interface (par exemple, eth0).

Affiche les préfixes IPv4 et IPv6 ainsi que leurs masques pour l'interface spécifiée.

2. Afficher toutes les interfaces réseau

./ifshow -a

Liste toutes les interfaces réseau disponibles avec leurs adresses IPv4/IPv6 et masques.

Compilé pour système 32 bits


# ifnetshow - Programme Serveur/Client en C

Ce projet permet d'exposer les adresses IP d'un serveur via un programme client. Le serveur fournit les informations réseau disponibles (interfaces et leurs adresses IP associées), tandis que le client peut interroger le serveur pour récupérer ces informations.

## Fonctionnalités

- **Serveur (`ifnetshow_server`)** :
  - Récupère les interfaces réseau (IPv4 et IPv6) du système.
  - Traite les commandes du client pour afficher soit toutes les interfaces, soit une interface spécifique.
  - Envoie les informations réseau au client.

- **Client (`ifnetshow_client`)** :
  - Envoie une commande au serveur pour obtenir les informations réseau.
  - Affiche les informations réseau reçues dans le terminal du client.

---

## Prérequis

Pour compiler et exécuter ce programme, vous aurez besoin de :
- Un système Unix/Linux.
- Un compilateur C (comme gcc)
- Les bibliothèques standards C disponibles sur votre système.

---

## Compilation

Utilisez les commandes suivantes pour compiler le serveur et le client :

```bash
gcc -o ifnetshow_server ifnetshow_server.c
gcc -o ifnetshow_client ifnetshow_client.c
