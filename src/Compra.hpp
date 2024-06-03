#ifndef COMPRA_HPP_
#define COMPRA_HPP_

namespace sgbd {

    // Estrutura para representar uma entrada na tabela de Compras
    struct Compra {
        int pedido; // Chave primária
        double valor;
        int ano; // Chave do índice hash extensível.
    };

}

#endif