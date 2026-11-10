# Simple HTTP Server in C

Um servidor web HTTP simples, escrito em C puro, projetado para fins de estudo de baixo nível sobre **Sockets**, **Sistemas Unix**, **Manipulação de Arquivos** e **Redes**.

## 🚀 Funcionalidades

- **Sockets TCP/IP**: Utiliza a API de sockets BSD para escutar conexões na porta `8080`.
- **Reutilização de Porta**: Configurado com `SO_REUSEADDR` para evitar o erro *"Address already in use"* ao reiniciar o servidor rapidamente.
- **Servimento de Arquivos Estáticos**: Mapeia rotas para uma pasta local (`www/`) e serve arquivos automaticamente.
- **Detecção de Tipo de Conteúdo (`Content-Type`)**: Suporte básico para arquivos `.html`, `.css`, `.png` e `.jpg`.
- **Proteção contra Directory Traversal**: Bloqueia tentativas de acesso malicioso usando caminhos com `..`.
- **Tratamento de Erros Robusto**: Retorna respostas HTTP `404 Not Found` caso o arquivo não exista ou ocorra alguma falha de leitura.

---

## 🛠️ Requisitos

Para compilar e rodar este projeto, você precisará de:
- Um sistema operacional compatível com POSIX (Linux, macOS, etc.)
- Um compilador C (como `gcc`)
- A ferramenta `make`

---

## ⚙️ Como Compilar e Executar

1. **Clone ou abra a pasta do projeto** onde estão os arquivos fonte e o `Makefile`.
2. **Crie uma pasta chamada `www`** no mesmo diretório para colocar suas páginas web:
   ```bash
       mkdir www
   ```
3. Adicione um arquivo de teste (por exemplo, index.html) dentro da pasta www/:
    ```HTML
        <h1>Olá do meu servidor em C!</h1>
    ```
4. Compile o projeto usando o make:
    ```bash
    make
    ```
    Execute o servidor:
    ```bash
    ./server
    ```
O servidor estará rodando em: http://localhost:8080

## 📂 Estrutura do Código

    - content_type(): Analisa a extensão do arquivo solicitado para definir o cabeçalho HTTP correto.

    - main():

        - Inicializa o socket do servidor (socket, bind, listen).

        - Entra num loop infinito aceitando conexões de clientes (accept).

        - Lê a requisição HTTP (read), extrai o método e o caminho (sscanf).

        - Valida o caminho contra invasões de diretório (strstr).

        - Verifica a existência e as permissões do arquivo (stat, open).

        - Envia os cabeçalhos HTTP e o conteúdo do arquivo via write.

## 📄 Licença

Este projeto está sob a licença MIT. Sinta-se livre para usá-lo, modificá-lo e estudá-lo.
