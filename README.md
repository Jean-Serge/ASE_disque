ASE_disque
==========
Jean-Serge Monbailly
Arthur Dewarumez

Fichiers présent :
==================
README.md     : le fichier README
drive.[c|h]   : le bibliothèque de gestion de disque
dmps.c        : les sources du programme d'affichage de secteur du disque
frmt.c        : les sources du programme de formattage du disque.
Makefile      : le makefile


Pour compiler :
===============

	Pour compiler l'ensemble du projet
		make all

	Pour compiler le programme de création de disque :
		make create-disk

	Pour compiler le programme d'affichage de secteur du disque :
		make dmps

	Pour compiler le programme de formattage de disque :
		make frmt

	Pour compiler le programme de test des fonctions de gestion du MBR :
		make test_mbr

	Par nettoyer le projet
		make clean

Structure du MBR
================
Secteur 0 cylindre 0

0           2           3           8          12          17          22          27          32          37          42
+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
|           |           |           |           |           |           |           |           |           |           |
|   Magic   |  nb  vol  |   Vol 1   |   Vol 2   |   Vol 3   |   Vol 4   |   Vol 5   |   Vol 6   |   Vol 7   |   Vol 8   |
|           |           |           |           |           |           |           |           |           |           |
+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+

Magic :
-------
Pour vérifier que la MBR à bien été initialisé

0           1           2
+-----------+-----------+
|           |           |
|   0xB0    |   0xB0    |
|           |           |
+-----------+-----------+

Description d'un volume :
-------------------------

0           1           2           3           4           5
+-----------+-----------+-----------+-----------+-----------+
| cylindre  |  secteur  |                       |           |
|    de     |    de     |   nombre de secteurs  |    type   |
|   début   |   début   |                       |           |
+-----------+-----------+-----------+-----------+-----------+

TODO
+ Ajouter dans le programme de test la mise en place de valeur par défaut.
+ Dans le fichier filesystem.c, écrire fonction pour lire écrire
un int* sur un disque.
+ Modifier les fonctions de filesystem.c utilisant write/read_struct sur un
int * pour utiliser les fonctions appropriées (delete_inode et fbloc...).
