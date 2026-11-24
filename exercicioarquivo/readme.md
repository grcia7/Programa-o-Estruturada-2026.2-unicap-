# Streaming de Vídeos - Arquivos Binários em C

Tipo de entrega: Individual (ou Dupla; apague a opção que não se aplica)

Participantes:

- Nome completo - RA
- Nome completo - RA (somente se for dupla)

Vídeo de demonstração: cole aqui o link do YouTube

## Descrição

Programa em C que manipula dois arquivos binários, cada um com um tipo de registro diferente, relacionados entre si.

videos.bin guarda registros do tipo Video:

- id (campo-chave)
- titulo
- genero
- ano
- duracao_min

usuarios.bin guarda registros do tipo Usuario:

- id (campo-chave)
- nome
- email
- qtd_favoritos
- favoritos[20], um vetor com os IDs dos vídeos favoritados

O vetor favoritos[] é o relacionamento entre os arquivos: cada posição contém o id de um registro de videos.bin.

## Funcionalidades

Para os dois tipos de registro (vídeo e usuário):

- Cadastrar um registro novo (grava no final do arquivo).
- Buscar um registro específico pelo ID.
- Listar todos os registros do arquivo.
- Atualizar um registro existente (altera o registro direto no arquivo, com fseek e fwrite).
- Remover um registro.

Para usuários, a atualização também permite adicionar e remover vídeos dos favoritos.

## Relatórios

1. Usuários e seus vídeos favoritos: para cada usuário, mostra os títulos dos vídeos favoritados (buscados em videos.bin, não os IDs).
2. Vídeos e os usuários que os favoritaram: para cada vídeo, mostra os nomes dos usuários que o favoritaram.

## Como funciona

- Remoção: os registros são copiados para temp.bin, exceto o que será removido. Depois os dois arquivos são fechados, o arquivo antigo é apagado com remove e temp.bin é renomeado com rename para o nome do antigo. Se algo falhar antes disso, o arquivo original não é apagado.
- Ao remover um vídeo, o ID dele é retirado dos favoritos de todos os usuários.
- Ao favoritar um vídeo, o programa confere se o vídeo existe e se ele já não está na lista.
- Os IDs são gerados automaticamente (maior ID do arquivo + 1).
- As funções de busca, remoção, gravação e geração de ID são genéricas: recebem o tamanho do registro e funcionam para os dois tipos, porque o id é o primeiro campo dos dois structs.
- O retorno de fopen, fwrite, fread, fseek, remove e rename é conferido. O retorno de fclose é conferido nos arquivos abertos para escrita. Depois de um fread incompleto, feof diferencia o fim do arquivo de um erro de leitura.
- A opção 13 do menu carrega dados de exemplo (6 vídeos e 3 usuários), mas só funciona se os arquivos estiverem vazios ou não existirem.

## Como compilar e executar

```
gcc -Wall -Wextra -std=c99 -o streaming main.c
./streaming
```

No Windows, o executável é streaming.exe. Os comandos completos estão no arquivo comandos.txt.

Os arquivos videos.bin e usuarios.bin são criados na pasta onde o programa é executado.

## Menu

```
1  - Cadastrar vídeo
2  - Buscar vídeo por ID
3  - Listar todos os vídeos
4  - Atualizar vídeo
5  - Remover vídeo
6  - Cadastrar usuário
7  - Buscar usuário por ID
8  - Listar todos os usuários
9  - Atualizar usuário (dados e favoritos)
10 - Remover usuário
11 - Relatório: usuários e seus vídeos favoritos
12 - Relatório: vídeos e os usuários que os favoritaram
13 - Carregar dados de exemplo
0  - Sair
```

## Observação

Os registros são gravados diretamente com fwrite a partir do struct. Por isso, os arquivos .bin só são garantidos na mesma plataforma e compilador em que foram gerados.