#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

// Estrutura para representar uma entrada na tabela de Compras
struct Purchase {
    int pedido; //Primary Key
    double valor;
    int ano;
};

// Estrutura para representar um bucket
struct Bucket {
    vector<Purchase> entries;
};

// Estrutura para representar o diret�rio do �ndice hash extens�vel
struct Directory {
    int global_depth;
    unordered_map<int, Bucket> buckets;
};

// Fun��o para inicializar o �ndice hash extens�vel com a profundidade global inicial
Directory initializeIndex(int global_depth) {
    Directory index;
    index.global_depth = global_depth;
    // Inicializar os buckets vazios
    /* '1 << global_depth' � uma opera��o de deslocamento de bits que produzir� o n�mero de buckets 
    no diret�rio, que � igual a 2 elevado � profundidade global.*/
    for (int i = 0; i < (1 << global_depth); ++i) {
        Bucket bucket;
        index.buckets[i] = bucket;
    }
    return index;
}

// Fun��o para calcular o hash da chave de busca
int hashFunction(int key, int global_depth) {
    
    /* '(1 << global_depth) - 1': isso calcula um valor que consiste em uma sequ�ncia de bits com todos 
    os bits definidos at� a posi��o global_depth - 1, resultando em uma m�scara que cobre todos os bits 
    significativos at� a profundidade global do �ndice hash.*/
    
    /* 'key & ((1 << global_depth) - 1)' aplica a m�scara � chave de busca usando o operador bitwise AND 
    (&). Isso significa que apenas os bits na posi��o coberta pela m�scara ser�o preservados, 
    enquanto os outros bits ser�o zerados. Essa opera��o tem o efeito de "cortar" os bits irrelevantes 
    na chave de busca, deixando apenas os bits que contribuem para o �ndice do bucket.*/

    /*O resultado final � o indice calculado do bucket*/
    
    return key & ((1 << global_depth) - 1);
}

// Fun��o para realizar a busca por igualdade
vector<Purchase> searchEqual(Directory& index, int year) {
    vector<Purchase> result;
    int hash_value = hashFunction(year, index.global_depth);
    Bucket& bucket = index.buckets[hash_value];
    for (const auto& entry : bucket.entries) {
        if (entry.ano == year) {
            result.push_back(entry);
        }
    }
    return result;
}

// Fun��o para contar o n�mero de zeros � esquerda em um n�mero inteiro -> TEMPOR�RIO
int countLeadingZeros(int value) {
    int count = 0;
    while ((value & (1 << (sizeof(value) * 8 - 1))) == 0) {
        count++;
        value <<= 1;
    }
    return count;
}

// Fun��o para realizar a inclus�o de uma entrada -> TEMPOR�RIO
void insertEntry(Directory& index, Purchase entry) {
    int hash_value = hashFunction(entry.ano, index.global_depth);
    Bucket& bucket = index.buckets[hash_value];
    bucket.entries.push_back(entry);
    // Verificar se o bucket excede o limite m�ximo de entradas
    if (bucket.entries.size() > 3) {
        // Dividir o bucket
        int local_depth = 0; // Profundidade local do novo bucket
        // Verificar se a profundidade local do bucket � igual � profundidade global
        if (index.global_depth == bucket.entries.size()) {
            // Duplicar a capacidade do diret�rio
            index.global_depth++;
            // Redimensionar o diret�rio e redistribuir os buckets
            int old_index = hash_value;
            int new_index = hash_value | (1 << (index.global_depth - 1));
            // Copiar metade das entradas do bucket antigo para o novo bucket
            Bucket& new_bucket = index.buckets[new_index];
            new_bucket.entries.reserve(bucket.entries.size() / 2);
            for (auto it = bucket.entries.begin() + bucket.entries.size() / 2; it != bucket.entries.end(); ++it) {
                new_bucket.entries.push_back(*it);
            }
            bucket.entries.erase(bucket.entries.begin() + bucket.entries.size() / 2, bucket.entries.end());
            // Atualizar a profundidade local do novo bucket
            local_depth++;
        }
        else {
            // Aumentar a profundidade local do bucket
            local_depth = index.global_depth - countLeadingZeros(hash_value ^ (1 << (index.global_depth - 1)));
            // Dividir o bucket em dois e redistribuir as entradas
            Bucket new_bucket;
            auto it = bucket.entries.begin() + bucket.entries.size() / 2;
            while (it != bucket.entries.end()) {
                int new_hash = hashFunction(it->ano, index.global_depth);
                if (new_hash != hash_value) {
                    new_bucket.entries.push_back(*it);
                    it = bucket.entries.erase(it);
                }
                else {
                    ++it;
                }
            }
            // Atualizar a profundidade local do novo bucket
            local_depth++;
            // Atualizar o diret�rio para incluir o novo bucket
            index.buckets[hash_value | (1 << (local_depth - 1))] = new_bucket;
        }
    }
}

// Fun��o para carregar as compras do arquivo CSV para o �ndice hash extens�vel -> TEMPOR�RIO
void loadPurchasesFromCSV(Directory& index, const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Erro ao abrir o arquivo CSV: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string token;
        Purchase purchase;
        getline(iss, token, ',');
        purchase.pedido = stoi(token);
        getline(iss, token, ',');
        purchase.valor = stod(token);
        getline(iss, token);
        purchase.ano = stoi(token);

        insertEntry(index, purchase); // Adiciona a compra ao �ndice hash
    }

    file.close();
}

int main() {
    // Abrir arquivo de entrada
    ifstream inputFile("in.txt");
    if (!inputFile) {
        cerr << "Erro ao abrir o arquivo de entrada!" << endl;
        return 1;
    }

    // Ler a profundidade global inicial do arquivo de entrada
    string line;
    int global_depth;
    getline(inputFile, line);
    istringstream iss(line);
    string token;
    getline(iss, token, '/');
    getline(iss, token);
    global_depth = stoi(token);

    // Inicializar o �ndice hash extens�vel com a profundidade global inicial. (Diret�rio Vazio)
    Directory index = initializeIndex(global_depth);

    //Ler o arquivo compras.csv e carrega para o diret�rio
    loadPurchasesFromCSV(index, "compras.csv");

    // Processar as opera��es de busca por igualdade e escrever os resultados no arquivo de sa�da
    ofstream outputFile("out.txt");
    if (!outputFile) {
        cerr << "Erro ao abrir o arquivo de sa�da!" << endl;
        return 1;
    }

    while (getline(inputFile, line)) {
        // Processar opera��o do arquivo de entrada
        if (line.substr(0, 4) == "BUS=") {
            int year = stoi(line.substr(5));
            vector<Purchase> result = searchEqual(index, year);
            outputFile << "BUS:" << year << "/" << result.size() << endl;
        }

        //Implementar INC e REM
    }

    // Fechar arquivos
    inputFile.close();
    outputFile.close();

    return 0;
}
