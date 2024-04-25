#include "Bucket.hpp"

sgbd::Bucket::Bucket () { }

sgbd::Bucket::Bucket (std::size_t ponteiro) {

    // TO DO

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

    // TO DO

}
