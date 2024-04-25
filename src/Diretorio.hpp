#ifndef DIRETORIO_HPP_
#define DIRETORIO_HPP_

#include <vector>
#include <string>

namespace sgbd {

    enum Operacao { INC, REM, BUS };

    struct EntradaDiretorio {

        std::size_t pl;
        std::size_t ponteiro;

    };

    class Diretorio {

        private:


            std::size_t pg; // Profundidade global
            const std::string arquivoCSV;
            std::vector<EntradaDiretorio> referencias;

            // Duplica o espaço do diretório.
            void duplicar ();

        public:

            Diretorio (std::size_t pg = 1, std::string csv = "compras.csv");

            std::size_t getPG () const;

            sgbd::EntradaDiretorio operator[] (std::size_t i) const;

            // Função hash para calcular o índice do diretório.
            std::size_t hash (int chave) const;
            // Realiza uma das três operações implementadas (inclusão, remoção ou busca) e retorna uma std::string com informações da operação realizada.
            std::string operacao (sgbd::Operacao operacao, int chave);

    };

}

#endif