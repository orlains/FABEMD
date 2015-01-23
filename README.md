FABEMD
======

Fast and Adaptive Bidimensional Empirical Mode Decomposition (FABEMD)
Using Order-Statistics Filter Based Envelope Estimation

##Compilation
	make

##Utilisation
###Test sur une image
	./bin/fabemd -i ./data/elaine.png -o 3 -n 1 -t 0.05 -w 3
	-i Image d'entrée
	-o Type de OSFW
	-n Nombre maximal d'itérations
	-w Taille de la fenêtre de recherche
	-t Seuil d'écart-type
	
###Test sur une image de synthèse
	./bin/fabemd -i ./data/elaine.png -o 3 -n 1 -t 0.05 -w 3 -s 1
	-s Activation du test sur données de synthèses
