(c) Renaud PINON 2022

Ce projet appartient à Renaud PINON. Le code peut être utilisé sans restriction à condition que l'appartenance du projet soit conservée et clairement indiquée.


# WifiClock

Horloge connectée

# Description

Programme à envoyer à un ESP-32 afin de faire une horloge connectée sur Internet. Le module intéragit avec un ensemble de Leds à la suite les unes des autres formant les chiffres de l'horloge.

Les paramètres de l'horloge (fuseau horaire, couleur des LEDS, serveur de temps, SSID et mot de passe Wifi, etc...) sont accessibles sur une interface web. Pour cela, mettre l'horloge en mode Paramétrage et accéder au réseau Wifi créé par l'horloge. Puis accéder à l'adresse IP de l'horloge (192.168.x.1) depuis un navigateur pour obtenir la page de paramétrage.

# Status

En cours. Code écrit totalement mais matériel pas encore monté.


# TODO

- Schéma électronique de montage
- correction problème voltage insuffisant lorsque le montage est alimenté par un transformateur 220V AC => 5V DC (fonctionne avec un USB d'ordinateur).
