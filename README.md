# Projeto 1 de Programação Concorrente e Paralela

O projeto consistia em criar uma barreira borboleta entre 8 processos trabalhando como cliente e servidor, realizando redução de dados ao receberem números aleatórios dos clientes e somá-los, passando para a próxima camada. Esses 8 processos eram denominados workers e tinha 1 manager.

A ideia para implementar a parte de cliente-servidor foi por meio de bibliotecas em C como <netdb.h>, <sys/socket.h>, <arpa/inet.h>. O servidor espera o número que vai somar com o seu número e o cliente envia esse valor. Todos os números são aleatórios.

Vale ressaltar que um worker pode ser cliente e servidor, dependendo da iteração e que o manager sempre é servidor.

Foi usada a operação bitwise *&* para verificar qual worker era cliente e qual era servidor. Assumindo que os valores dos workers variam de 0 a 7, na primeira iteração, o cliente será os ímpares, ou seja, último bit == 1. Na segunda, haverá apenas os pares, sendo os clientes aqueles que possuem o penúltimo bit == 1, e assim por diante na barreira.

# Para rodar

## Primeira forma

Pode abrir vários terminais, primeiro rodando o manager:

    ./m

Depois, abrir um terminal para cada worker:

    ./w (número do worker)

Os números dos worker vão de 0 a 7, contabilizando 8 workers.

Pode rodar em terminais diferentes e fora de ordem, ou na ordem.

## Segunda forma

Pode abrir no mesmo terminal, executando da seguinte forma:

    ./m & ./w 0 & ./w 1 & ./w 2 & ./w 3 & ./w 4 & ./w 5 & ./w 6 & ./w 7

Pode modificar a ordem dos workers, mas o ideal é ter o manager primeiro, apesar de que no código, os clientes ficam o *loop* até conseguir a conexão quando o servidor é iniciado.

## Terceira forma

Fizemos um arquivo *bash* para facilitar a execução, pode modificá-lo  e mudar a ordem dos workers se desejado. Basta colocar no terminal:

    ./rodar.sh