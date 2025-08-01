# lab2_parte2_redes

## como correr el algoritmo de correccion, cliente y servidor:

Primero se entra a la carpeta
```
cd correction
```

Compilar el servidor y luego el cliente:
```
gcc servidor.c codigo_hamming.c -o servidor
```
```
gcc cliente.c codigo_hamming.c -o cliente
```
Luego:
```
./servidor
```
Y por ultimo: 
```
./cliente
```

Archivos: 
Correccion de errores: 
cliente: es el emisor
servidor: es el receptor
codigo de hamming: codigo de hamming
codigo de hamming.h: Declaraciones de funciones y estructura

correccion.c que es la version sin sockets, la entregada en avances pero con mejoras. Este realmente no se usa, solo lo deje como referencia para realizar lo demas y que tuviera una bonita estructura. 