#pragma once

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include "status.h"

typedef struct hash_map hash_map;

/**
 * @brief Inicializa e aloca uma nova instância de hash_map.
 * * Reserva memória para a estrutura de controle e para a tabela interna de buckets.
 * As funções de callback (hash e cmp_key) são essenciais para o funcionamento 
 * da estrutura e não devem ser nulas.
 * * @param initial_size Capacidade inicial do mapa (quantidade de buckets).
 * @param hash         Ponteiro para a função de hashing das chaves.
 * @param cmp_key      Ponteiro para a função de comparação de igualdade entre chaves.
 * @param free_key     Opcional: Função para desalocação de chaves (pode ser NULL).
 * @param free_value   Opcional: Função para desalocação de valores (pode ser NULL).
 * * @return 
 * - `hash_map*`: Endereço da nova instância alocada.
 * - `NULL`: Caso ocorra falha de alocação ou se `hash`/`cmp_key` forem nulos.
 * * @note Se 'free_key' ou 'free_value' forem fornecidos, o mapa assumirá a 
 * propriedade da memória dos elementos inseridos, liberando-os automaticamente 
 * em operações de remoção ou limpeza.
 */
hash_map* new_hash_map(size_t value_size, size_t (*hash)(const void*), int (*cmp_key)(const void*, const void*), void (*free_key)(void*), void (*free_value)(void*));

/**
 * @brief Insere ou atualiza um par chave-valor no mapa.
 * * Se a chave já existir, o valor anterior é liberado (se o free_value não for nulo) 
 * e substituído. O mapa sofre redimensionamento automático (rehash) caso o 
 * fator de carga atinja 0.75.
 * * @param hm Instância do hash_map.
 * @param key Referência para a chave de busca.
 * @param value Referência para o dado a ser armazenado.
 * @return 
 * - `SUCCESS`: Operação concluída com sucesso. 
 * - `ALLOC_FAIL`: Erro de alocação de memória (nó ou rehash).
 * - `NULL_POINTER`: Se 'hm' ou 'key' forem nulos.
 */
status hash_map_insert(hash_map* hm, void* key, void* value);

/**
 * @brief Remove um par chave-valor com base na chave fornecida.
 * * Se encontradas, as referências de chave e valor são liberadas utilizando as 
 * funções de limpeza configuradas na criação do mapa.
 * * @param hm Instância do hash_map.
 * @param key Referência para a chave a ser removida.
 * @return
 * - `SUCCESS`: Remoção realizada com sucesso. 
 * - `NULL_POINTER`: Se 'hm' ou 'key' forem nulos.
 * - `UNDERFLOW`: O mapa não contém elementos.
 * - `NOT_FOUND`: A chave especificada não existe no mapa.
 */
status hash_map_remove(hash_map* hm, const void* key);

/**
 * @brief Recupera a referência do valor associado a uma chave.
 * * @param hm Instância do hash_map.
 * @param key Referência para a chave de busca.
 * @return void* Endereço do valor associado ou `NULL` se a chave não for encontrada 
 * ou se os parâmetros forem inválidos.
 */
void* hash_map_get(const hash_map* hm, void* key);

/**
 * @brief Retorna a quantidade de elementos armazenados do hash_map, utiliza assert para garantir que o mesmo não é NULL.
 * * @param hm Instância do hash_map.
 * @return size_t Quantidade de elementos armazenados no hash_map (count)
 */
size_t hash_map_count(const hash_map* mp);

/**
 * @brief Transforma todos os valores armazenados no hash_map em um vetor.
 * * @param hm Instância do hash_map.
 * @return 
 * - void* : Valores obtidos com sucesso. 
 * - NULL: Se 'hm' for nulo ou estiver vazio.
 */
void* hash_map_values(const hash_map* hm);

/**
 * @brief Remove e libera todos os elementos contidos no mapa.
 * * Mantém a estrutura do mapa intacta para novas inserções, mas redefine 
 * o tamanho interno (count) para zero.
 * * @param hm Instância do hash_map a ser limpa.
 * @return 
 * - `SUCCESS`: Mapa esvaziado com sucesso. 
 * - `NULL_POINTER`: Se 'hm' for nulo.
 */
status hash_map_clear(hash_map* hm);

/**
 * @brief Desaloca a estrutura do mapa e todos os seus recursos internos.
 * * Após esta chamada, a referência 'hm' torna-se inválida.
 * * @param hm Instância do hash_map a ser destruída.
 */
void hash_map_free(hash_map* hm);

#endif
