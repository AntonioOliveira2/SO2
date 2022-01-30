Código base do projeto de Sistemas Operativos
====================================

LEIC-A/LEIC-T/LETI, DEI/IST/ULisboa 2021-22
---------------------------------------------------------------

Consultar o enunciado publicado no site da disciplina

CHANGE LOG:
============

**Versão 4**
-----------------

Corrigido um erro no teste da função tfs_destroy_after_all_closed.

**Versão 3**
-----------------

Várias extensões para suportar o desenvolvimento do 2º exercício (consultar o enunciado respetivo).

**Versão 2**
-----------------

1. Removido um bloco “if” obsoleto na tfs_read (na v1, linhas 162-165 do operations.c): “if (file->of_offset + to_read >= BLOCK_SIZE) {...}”. Além de desnecessário, a condição continha um bug que poderia fazer com que uma leitura a um bloco cheio não devolvesse o resultado esperado.

2. O ciclo de “insert_delay” na função inode_create (na v1, linha 93 do state.c na v1) foi retificado para passar a ter um número de iterações mais realista.
O original (v1) era “if ((inumber * (int) sizeof(allocation_state_t)) == 0)”, passou para “if ((inumber * (int) sizeof(allocation_state_t) % BLOCK_SIZE) == 0)”.

3. Removido um comentário obsoleto (na v1, linhas 159-160 do state.c).


