all : lista
	gcc -o shell shell.c list.c -Wall

lista: list.c 
	gcc -c -o list list.c -Wall
