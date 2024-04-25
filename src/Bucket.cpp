#include "Bucket.hpp"

sgbd::Bucket::Bucket () { }

sgbd::Bucket::Bucket (std::size_t ponteiro) {

    this->ponteiro = ponteiro;

    std::ifstream arq(std::to_string(ponteiro) + ".txt");

    if (arq) {

        this->dirty = false;

        //Ler as entradas do arquivo para as entradas do bucket (MAX 3 p/ arquivo)
        std::string linha;
        while (std::getline(arq, linha)) {
            
            EntradaBucket tmp;

            std::istringstream iss(linha);
            std::string token;

            std::getline(iss, token, ',');
            tmp.id = std::stoi(token);

            std::getline(iss, token);
            tmp.chave = std::stoi(token);

            this->entradas.push_back(tmp);

        }
    }
    else {
        //Se o arquivo não existir...

        this->dirty = true;

    }

    arq.close();

}

sgbd::Bucket::~Bucket () {

    if (this->dirty) {

        this->escreverArquivo();

    }

}

std::size_t sgbd::Bucket::getPonteiro () const {

    return this->ponteiro;

}

const std::vector<sgbd::EntradaBucket>& sgbd::Bucket::getEntradas () const {

    return this->entradas;

}

sgbd::EntradaBucket sgbd::Bucket::removeEntrada (std::size_t i) {

    sgbd::EntradaBucket entrada = this->entradas[i];
    this->entradas.erase(this->entradas.begin() + i);
    dirty = true;
    return entrada;

}

bool sgbd::Bucket::inserir (sgbd::EntradaBucket entrada) {

    if (!(this->entradas.size() == 3)) {

        this->entradas.push_back(entrada);
        this->dirty = true;
        return true;

    }

    return false;

}

std::size_t sgbd::Bucket::remover (int chave) {

    std::size_t quantTuplasRem = 0;

    for (std::size_t i = 0; i < this->entradas.size(); i++) {

        if (this->entradas[i].chave == chave) {

            this->entradas.erase(this->entradas.begin() + i);
            dirty = true;
            quantTuplasRem++;

        }

    }

    return quantTuplasRem;

}

std::size_t sgbd::Bucket::buscar (int chave) {

    std::size_t quantTuplasBus = 0;

    for (std::size_t i = 0; i < this->entradas.size(); i++) {

        if (this->entradas[i].chave == chave) {

            quantTuplasBus++;

        }

    }

    return quantTuplasBus;

}

void sgbd::Bucket::escreverArquivo () {

    if (this->dirty) {
        std::ofstream arq((std::to_string(this->ponteiro) + ".txt"), std::ios::out);

        for (int i = 0; i < this->entradas.size(); i++) {
            //Escrever linha a linha em formato .csv
            arq << this->entradas[i].id << "," << this->entradas[i].chave << "\n";
        }

        arq.close();
    }
}