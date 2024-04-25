#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "Compra.hpp"
#include "Bucket.hpp"
#include "Diretorio.hpp"

int main(int argc, char* argv[]) {

    int pg; // Profundidade global
    std::size_t chave; // Operando das operações do arquivo de entrada.
    std::string linha; // Linha do arquivo de entrada.
    std::string token; // "Pedaço" de uma linha do arquivo de entrada.
    std::size_t numLinha = 0; // Número da linha sendo processada do arquivo de entrada.

    // Abre o arquivo de entrada
    std::ifstream arquivoInput("in.txt");
    // Abre o arquivo de saída.
    std::ofstream arquivoOutput("out.txt");

    try {

        if (!arquivoInput) {
            
            throw std::runtime_error("Erro ao abrir o arquivo de entrada!");
        
        }

        if (!arquivoOutput) {

            throw std::runtime_error("Erro ao abrir o arquivo de saída! Tente novamente.");

        }

        // Lê a profundidade global inicial do arquivo de entrada.
        if (std::getline(arquivoInput, linha)) {

            numLinha++;
            std::istringstream issLinha(linha);
            std::getline(issLinha, token, '/');

            // Checa se a profundidade global foi definida no arquivo.
            if (token != "PG") {

                throw std::runtime_error("A profundidade global não está definida corretamente na primeira linha do arquivo de entrada!");

            }

            std::getline(issLinha, token);
            pg = std::stoi(token);

            if (pg < 0) {

                throw std::runtime_error("A profundidade global está definida como um inteiro negativo no arquivo de entrada!");

            }

        } else {

            throw std::runtime_error("O arquivo de entrada está vazio!");

        }

        // Cria o diretório
        sgbd::Diretorio diretorio(pg);

        // Insere a primeira linha do arquivo de saída.
        arquivoOutput << linha << std::endl;

        // Varre o arquivo de input e processa os comandos.
        while (std::getline(arquivoInput, linha)) {

            numLinha++;
            std::istringstream issLinha(linha);
            std::getline(issLinha, token, ':');

            if (token == "INC") {

                std::getline(issLinha, token);
                chave = std::stoi(token);
                arquivoOutput << diretorio.operacao(sgbd::INC, chave);

            } else if (token == "REM") {

                std::getline(issLinha, token);
                chave = std::stoi(token);
                arquivoOutput << diretorio.operacao(sgbd::REM, chave);

            } else if (token == "BUS=") {

                std::getline(issLinha, token);
                chave = std::stoi(token);
                arquivoOutput << diretorio.operacao(sgbd::BUS, chave);

            } else {

                throw std::runtime_error(std::string("Erro de sintaxe na linha ") + std::to_string(numLinha) + " do arquivo de entrada!");

            }

            arquivoOutput << std::endl;

        }

        // Insere última linha do arquivo de saída no formato "P:/<profundidade global final>".
        arquivoOutput << "PG:/" << diretorio.getPG();

    } catch (std::runtime_error& erro) {

        std::cout << erro.what() << std::endl;

    }

    // Fecha arquivos.
    arquivoInput.close();
    arquivoOutput.close();

    return EXIT_SUCCESS;

}
