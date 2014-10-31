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

0           2           3           10          17          24          31          38          45          52         60
+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
|-----------|           |           |           |           |           |           |           |           |           |
|   Magic   |  nb  vol  |   Vol 1   |   Vol 2   |   Vol 3   |   Vol 4   |   Vol 5   |   Vol 6   |   Vol 7   |   Vol 8   |
|           |-----------|           |           |           |           |           |           |           |           |
+-----------+           +-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+

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

0           2           3           4           5           6           7
+------------+----------+-----------+-----------+-----------+-----------+
| cylindre |            |                       |                       |           |
|    de    |   secteur de début    |   nombre de secteurs  |    type   |
|  début   |           |                       |                       |           |
+-----------+-----------+-----------+-----------+-----------+-----------+-----------+