ASE_disque
==========
Jean-Serge Monbailly
Arthur Dewarumez

Fichiers présent :
==================
README.md     : le fichier README


Pour compiler :
===============
	Pour compiler l'ensemble du projet :
		make all

	Par nettoyer le projet :
		make clean

Structure du MBR
================
Secteur 0 cylindre 0

0           3           4           11          18          25          32          39          46          53         59
+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
|-----------|           |           |           |           |           |           |           |           |           |
|   Magic   |  nb  vol  |   Vol 1   |   Vol 2   |   Vol 3   |   Vol 4   |   Vol 5   |   Vol 6   |   Vol 7   |   Vol 8   |
|           |-----------|           |           |           |           |           |           |           |           |
+-----------+           +-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+

Magic :
-------
Pour vérifier que la MBR à bien été initialisé

0           1           2           3
+-----------+-----------+-----------+
|           |           |           |
|     A     |     1     |     E     |
|           |           |           |
+-----------+-----------+-----------+

Description d'un volume :
-------------------------

0           1           2           3           4           5           6           7
+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
|                       |                       |                       |           |
|   cylindre de début   |   secteur de début    |   nombre de secteurs  |    type   |
|                       |                       |                       |           |
+-----------+-----------+-----------+-----------+-----------+-----------+-----------+