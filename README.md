Sickle [![Build Status](https://travis-ci.org/sup3asc2/sickle.svg?branch=master)](https://travis-ci.org/sup3asc2/sickle) [![Coverage Status](https://coveralls.io/repos/sup3asc2/sickle/badge.svg?branch=)](https://coveralls.io/r/sup3asc2/sickle?branch=) ![Version](https://img.shields.io/github/tag/sup3asc2/sickle.svg?style=flat)
==================================================
L'editeur de monde du prolétariat.
--------------------------------------------------
Pour pouvoir facilement compiler ce projet, il est recommandé d'utiliser l'IDE
Qt Creator. Sinon, il reste possible de générer un Makefile gràce a l'outil
`qmake` inclut dans la suite de programmes de la bibliothèque Qt.

# Documentation
La documentation utilisateur se trouve au format PDF a la racin du projet. Elle
explique rapidement comment utiliser l'éditeur.

La documentation technique se trouve dans le dossier docs (générée avec
Doxygen), en complément des informations présentes dans ce fichier.

# Technologie
Ce projet utilise OpenGL dans sa version 4.3. Il donc est recommandé d'avoir un
driver de carte graphique a jour, le programme ne fonctionnant simplement pas
avec une version inférieure.

# Rendu
L'affichage passe par un rendu `deferred`, qui consiste a enregistrer toutes
les informations sur une image dans une série de buffers. Ensuite l'image
finale est calculée dans une série de passe (une par source de lumière),
affichées a l'écran grace a un quad prenant tout l'écran.

# Sauvegardes
Les sauvegardes sont automatisées grace au système de meta-objet de Qt. Lors de
la sauvegarde d'un objet, le nom de sa classe est enregistrée, ainsi que toutes
ses propriétés et les objets qu'il contient. Lors du chargement du fichier, les
objets sont ré-instanciés gràce au nom de leur classe, et leurs propriétés
réstaurées.

# Interface
La gestion de l'interface repose aussi sur le système de méta-objet de Qt pour
lister les propriétés d'un objet, et les afficher avec un widget approprié
en fonction de leur type.
