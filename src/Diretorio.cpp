#include "Diretorio.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Bucket.hpp"
#include "Compra.hpp"

sgbd::Diretorio::Diretorio (std::size_t pg, std::size_t pl, std::string csv) : arquivoCSV(csv) {

    if (pl > pg) {

        throw std::invalid_argument("O valor de PL inicial do diretório é maior que o valor de PG inicial!");

    }

    this->pg = pg;
    this->referencias = std::vector<sgbd::EntradaDiretorio>(std::pow(2, pg), {pl, 0});
    for (std::size_t i = 0; i < this->referencias.size(); i++) {

        this->referencias[i].ponteiro = i & ((std::size_t) (1 << pl) - 1);

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

    for (std::size_t i = 0; i < tamanhoOriginal; i++) {

        this->referencias.push_back(referencias[i]);

    }

    this->pg++;

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
        bool duplicouDir = false; // Váriavel de controle que identifica se o diretório foi duplicado.

        // Abre o arquivo CSV para varredura.
        std::ifstream arquivo(this->arquivoCSV);
        if (!arquivo) {

            throw std::runtime_error(std::string("Não foi possível abrir o arquivo CSV: ") + this->arquivoCSV);

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

        if (novasEntradasBucket.size() > 3) {

            throw std::runtime_error(std::string("Há mais de 3 registros com ano=") + std::to_string(novasEntradasBucket[0].chave) + " no arquivo CSV!");

        }

        // Traz o bucket para a memória principal.
        sgbd::Bucket* bucket = new Bucket(this->referencias[indiceBucket].ponteiro);

        for (std::size_t i = 1; i <= novasEntradasBucket.size(); i++) { // O iterador começa do 1 porque ele pode ser decrementado e o std::size_t não representa números negativos.

            if (!bucket->inserir(novasEntradasBucket[i - 1])) {

                // Duplica o diretório antes de dividir o bucket se necessário.
                if (this->referencias[indiceBucket].pl == this->getPG()) {

                    this->duplicar();
                    duplicouDir = true;

                }

                // Dividir o bucket

                const std::size_t ponteiroAtual = this->referencias[indiceBucket].ponteiro;

                // Novo ponteiro que apontará para a imagem dividida do bucket.
                const std::size_t ponteiroNovo = ponteiroAtual + std::pow(2, this->referencias[indiceBucket].pl);

                // Entradas que serão movidas para a imagem dividida do bucket.
                std::vector<sgbd::EntradaBucket> entradasRedistribuicao;

                // Incrementa PLs de indices que apontam para o ponteiro atual e troca seus ponteiros se necessário.
                for (std::size_t indDir = 0; indDir < this->referencias.size(); indDir++) {

                    if (this->referencias[indDir].ponteiro == ponteiroAtual) {

                        this->referencias[indDir].pl++;

                        if ((indDir & ((1 << this->referencias[indDir].pl) - 1)) != ponteiroAtual) {

                            this->referencias[indDir].ponteiro = ponteiroNovo;
                            
                        }

                    }

                }

                // Guarda as entradas que serão redistribuídas para a imagem dividida.
                for (std::size_t j = 1; j <= bucket->getEntradas().size(); j++) { // O iterador começa do 1 porque ele pode ser decrementado e o std::size_t não representa números negativos.

                    // Checa se a entrada deve ser movida para o bucket que o ponteiro novo aponta.
                    if ((this->hash(bucket->getEntradas()[j - 1].chave) & ((1 << this->referencias[indiceBucket].pl) - 1)) == ponteiroNovo) {

                        entradasRedistribuicao.push_back(bucket->removeEntrada(j - 1));
                        j--;  // Volta o iterador para não pular 1 elemento do vetor "entradasRedistribuicao", já que o tamanho máximo do iterador diminuiu com a diminuição do vetor em si.

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

                // Se necessário, muda o bucket no qual a chave deve ser inserida.
                if (this->hash(chave) != indiceBucket) {

                    delete bucket;
                    indiceBucket = this->hash(chave);
                    bucket = new Bucket(this->referencias[indiceBucket].ponteiro);

                }

                // Volta o iterador para tentar a inserção novamente.
                i--;

            }

        }

        // Formato "INC:x/<profundidade global>,<profundidade local>"
        info = std::string("INC:") + std::to_string(chave) + '/' + std::to_string(this->getPG()) + ',' + std::to_string(this->referencias[indiceBucket].pl);

        if (duplicouDir) {

            // Caso o diretório tenha sido duplicado, adiciona uma linha no formato "DUP_DIR:/<profundidade global>,<profundidade local>"
            info = info + "\nDUP_DIR:/" + std::to_string(this->getPG()) + ',' + std::to_string(this->referencias[indiceBucket].pl);

        }

        // Tira o bucket da memória principal.
        delete bucket;

    } else {

        std::size_t quantTuplas; // Quantidade de tuplas removidas ou selecionadas

        // Traz o bucket para a memória principal.
        sgbd::Bucket* bucket = new Bucket(this->referencias[indiceBucket].ponteiro);

        if (operacao == sgbd::Operacao::REM) {

            quantTuplas = bucket->remover(chave);

            // TO DO: Dividir o diretório se necessário.

            // Formato "REM:x/<qtd de tuplas removidas>,<profundidade global>,<profundidade local>"
            info = std::string("REM:") + std::to_string(chave) + '/' + std::to_string(quantTuplas) + ',' + std::to_string(this->getPG()) + ',' + std::to_string(this->referencias[indiceBucket].pl);

        } else {

            quantTuplas = bucket->buscar(chave);

            // Formato "BUS:x/<quantidade de tuplas selecionadas>"
            info = std::string("BUS:") + std::to_string(chave) + '/' + std::to_string(quantTuplas);

        }

        // Tira o bucket da memória principal.
        delete bucket;

    }

    return info;
    
}