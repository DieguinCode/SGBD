#ifndef BUCKET_HPP_
#define BUCKET_HPP_

#include <vector>

namespace sgbd {

    struct EntradaBucket {

        int chave;
        int id;

    };

    class Bucket {

        private:

            std::size_t ponteiro; // Ponteiro para o bucket na memória externa
            std::vector<sgbd::EntradaBucket> entradas; // Entradas de dados
            bool dirty; // Varíavel de controle para alterações no bucket

        public:

            Bucket ();
            Bucket (std::size_t ponteiro);

            ~Bucket ();

            std::size_t getPonteiro () const;
            const std::vector<sgbd::EntradaBucket>& getEntradas() const;

            // Remove uma entrada do bucket e a retorna.
            sgbd::EntradaBucket removeEntrada (std::size_t i);
            // Insere uma entrada ao bucket. Retorna true se a inserção for bem sucedida e, em caso contrário, retorna false.
            bool inserir (sgbd::EntradaBucket entrada);
            // Remove todas as entradas do bucket com ocorrência da chave e retorna a quantidade de tuplas removidas.
            std::size_t remover (int chave);
            // Retorna a quantidade de entradas do bucket com ocorrência da chave.
            std::size_t buscar (int chave);
            // Escreve o bucket no seu arquivo de dados correspondente se houver alguma alteração (dirty == true).
            void escreverArquivo ();

    };

}

#endif