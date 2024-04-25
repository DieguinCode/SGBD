#include "Diretorio.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Bucket.hpp"
#include "Compra.hpp"

sgbd::Diretorio::Diretorio (std::size_t pg, std::string csv) : arquivoCSV(csv) {

    this->pg = pg;
    this->referencias = std::vector<sgbd::EntradaDiretorio>(std::pow(2, pg), {1, 0});
    for (std::size_t i = 0; i < this->referencias.size(); i++) {

        this->referencias[i].ponteiro = i;

    }

}

std::size_t sgbd::Diretorio::getPG () const {

    return this->pg;

}

sgbd::EntradaDiretorio sgbd::Diretorio::operator[] (std::size_t i) const {

    return this->referencias[i];

}

void sgbd::Diretorio::duplicar () {

    std::size_t tamanhoOriginal = this->referencias.size();
    std::size_t valorASerSomado = 1 << pg;

    for (std::size_t i = 0; i < tamanhoOriginal; i++) {

        this->referencias.push_back(referencias[i]);

    }

}

std::size_t sgbd::Diretorio::hash (int chave) const {

    return chave & ((1 << this->getPG()) - 1);

}

std::string sgbd::Diretorio::operacao (sgbd::Operacao operacao, int chave) {

    std::size_t indiceBucket;
    std::string info;

    indiceBucket = this->hash(chave);

    if (operacao == sgbd::Operacao::INC) {

        std::vector<sgbd::EntradaBucket> novasEntradasBucket;

        // Abre o arquivo CSV para varredura.
        std::ifstream arquivo(this->arquivoCSV);
        if (!arquivo) {

            throw std::invalid_argument(std::string("Erro ao abrir o arquivo CSV: ") + this->arquivoCSV + std::string("\n"));

        }

        // Varre o arquivo CSV e armazena entradas do bucket.
        std::string linha;
        while (std::getline(arquivo, linha)) {

            std::istringstream iss(linha);
            std::string token;
            sgbd::Compra compra;

            getline(iss, token, ',');
            compra.pedido = std::stoi(token);

            getline(iss, token, ',');
            compra.valor = std::stof(token);

            getline(iss, token);
            compra.ano = std::stoi(token);

            if (compra.ano == chave) novasEntradasBucket.push_back({chave, compra.pedido});

        }

        // Fecha o arquivo
        arquivo.close();

        // Traz o bucket para a memória principal.
        sgbd::Bucket* bucket = new Bucket(this->referencias[indiceBucket].ponteiro);

        for (std::size_t i = 1; i <= novasEntradasBucket.size(); i++) { // O iterador começa do 1 porque ele pode ser decrementado e o std::size_t não representa números negativos.

            if (!bucket->inserir(novasEntradasBucket[i - 1])) {

                // Duplica o diretório antes de dividir o bucket se necessário.
                if (this->referencias[indiceBucket].pl == this->getPG()) {

                    this->duplicar();

                }

                // Dividir o bucket

                const std::size_t ponteiroAtual = this->referencias[indiceBucket].ponteiro;

                // Novo ponteiro que apontará para a imagem dividida do bucket.
                const std::size_t ponteiroNovo = this->referencias[indiceBucket].ponteiro + std::pow(2, this->referencias[indiceBucket].pl);

                // Entradas que serão movidas para a imagem dividida do bucket.
                std::vector<sgbd::EntradaBucket> entradasRedistribuicao;

                // Incrementa PLs de indices que apontam para o ponteiro atual e troca seus ponteiros se necessário.
                for (std::size_t indDir; indDir < this->referencias.size(); indDir++) {

                    if (this->referencias[indDir].ponteiro == ponteiroAtual) {

                        this->referencias[indDir].pl++;

                        if (indDir & ((1 << this->referencias[indDir].pl) - 1) != ponteiroAtual) {

                            this->referencias[indDir].ponteiro = ponteiroNovo;
                            
                        }

                    }

                }

                // Guarda as entradas que serão redistribuídas para a imagem dividida.
                for (std::size_t j = 0; j < bucket->getEntradas().size(); j++) {

                    if (this->hash(bucket->getEntradas()[j].chave) & ((1 << this->referencias[indiceBucket].pl) - 1) == ponteiroNovo) {

                        entradasRedistribuicao.push_back(bucket->removeEntrada(j));

                    }

                }

                if (entradasRedistribuicao.size() > 0) {

                    // Tira o bucket da memória.
                    delete bucket;

                    // Traz a imagem dividida do bucket para a memória.
                    bucket = new Bucket(ponteiroNovo);

                    // Insere as entradas de redistribuição para dentro da imagem dividida.
                    for (std::size_t k = 0; k < entradasRedistribuicao.size(); k++) {

                        bucket->inserir(entradasRedistribuicao[k]);

                    }

                    // Tira a imagem dividida da memória.
                    delete bucket;

                    // Traz o bucket original de volta para a memória.
                    bucket = new Bucket(ponteiroAtual);

                }

                // Volta o iterador para tentar a inserção novamente.
                i--;

            }

        }

    } else {

        std::size_t quantTuplas; // Quantidade de tuplas removidas ou selecionadas

        // Traz o bucket para a memória principal.
        sgbd::Bucket* bucket = new Bucket(this->referencias[indiceBucket].ponteiro);

        if (operacao == sgbd::Operacao::REM) {

            quantTuplas = bucket->remover(chave);

            // TO DO: Dividir o diretório se necessário.

        } else {

            quantTuplas = bucket->buscar(chave);

            // Formato "BUS:x/<quantidade de tuplas selecionadas>"
            info = std::string("BUS:") + std::to_string(chave) + std::string("/") + std::to_string(quantTuplas);

        }

        // Tira o bucket da memória principal.
        delete bucket;

    }

    return info;
    
}