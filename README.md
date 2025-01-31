# ifshow

ifshow est un programme en C qui affiche les informations des interfaces réseau disponibles sur une machine. Il permet de lister les préfixes IPv4 et IPv6 des interfaces, ainsi que leurs masques sous forme de notation CIDR (par exemple, /24).

Le programme accepte deux options principales :

**Afficher une interface réseau spécifique**

```bash
./ifshow -i ethX
```
ethX : Nom de l'interface (par exemple, eth0).

Affiche les préfixes IPv4 et IPv6 ainsi que leurs masques pour l'interface spécifiée.

**Afficher toutes les interfaces réseau**

```bash
./ifshow -a
```
Liste toutes les interfaces réseau disponibles avec leurs adresses IPv4/IPv6 et masques.

Compilé pour système 32 bits


# ifnetshow - Programme Serveur/Client en C

Ce projet permet d'exposer les adresses IP d'un serveur via un programme client. Le serveur fournit les informations réseau disponibles (interfaces et leurs adresses IP associées), tandis que le client peut interroger le serveur pour récupérer ces informations.

## Fonctionnalités

- **Serveur (`ifnetshow_server`)** :
  - Récupère les interfaces réseau (IPv4 et IPv6) du système.
  - Traite les commandes du client pour afficher soit toutes les interfaces, soit une interface spécifique.
  - Envoie les informations réseau au client.

```bash
./ifnetshow_server &
```


- **Client (`ifnetshow_client`)** :
  - Envoie une commande au serveur pour obtenir les informations réseau.
  - Affiche les informations réseau reçues dans le terminal du client.

```bash
./ifnetshow_client -n 127.0.0.1 -a 
./ifnetshow_server -n 127.0.0.1 -i eth0

```


---

## Prérequis

Pour compiler et exécuter ce programme, vous aurez besoin de :
- Un système Unix/Linux.
- Un compilateur C (comme gcc)
- Les bibliothèques standards C disponibles sur votre système.

---

# Compilation

Vous pouvez récupérer les binaires (testé sur et entre Rocky9, Vyos 1.4.X, Alpine Linux, Ubuntu, Tinycore).
Utilisez les commandes suivantes pour compiler le serveur et le client :

```bash
gcc -m32 -static -o ifshow ifshow.c
gcc -m32 -static -o ifnetshow_server ifnetshow_server.c
gcc -m32 -static -o ifnetshow_client ifnetshow_client.c
```
