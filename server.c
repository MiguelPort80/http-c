#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

char *content_type(char *filename){
	char *dot = strchr(filename, '.');
	if(dot == NULL) return "text/plain";
	if(strcmp(dot, ".html") == 0) return "text/html";
	if(strcmp(dot, ".css") == 0) return "text/css";
	if(strcmp(dot, ".png") == 0) return "text/png";
	if(strcmp(dot, ".jpg") == 0) return "text/jpeg";
	return "text/plain";
}

/* file descriptor é um processo de indexação de processos a partir de índices que mostram as condições de inicialização de determinado arquivo.

Em sistemas operacionais mais fiéis ao Unix, os descritores usam uma tabela própria para a descrição do arquivo mantido anteriormente pelo SO graças à tabela de arquivos, também conhecida como filetable.

Esse recurso registra e centraliza diferentes informações em relação ao arquivo, programa etc. Ou seja, você abre o programa e essa tabela de arquivos mantém informações sobre a leitura, a gravação, os acréscimos feitos e outros diferentes modos de descrição. */

int main(void){
	//int socket(int domain, int type, int protocol);
	//protocolo vai ser ipv4(AF_INET), a stream no caso do ipv4 vai ser tcp, vai ser o protocolo padrão
	//server vai ser um file descriptor do socket
	int server = socket(AF_INET, SOCK_STREAM, 0);

	if(server < 0){
		perror("socket");
		exit(1);
	}

	struct sockaddr_in address;
	//função que preenche os bytes de uma área da memória
	//void *memset(void s[.n], int c, size_t n);
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	//o endereço vai ser 0.0.0.0
	address.sin_addr.s_addr = INADDR_ANY;
	//a porta vai ser 8080
	address.sin_port = htons(8080);

	int yes = 1;
	//permite que o servidor reutilize a mesma porta de rede imediatamente após ser fechado, evitando erros. Porque após o fechamento do servidor o SO segura a porta durante um tempo e se tentarmos conectar novamente dará um erro de a porta já estar sendo usada.
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	//a função bind() dá um nome a um socket. Quando um socket é criado ele existe num namespace porém sem nenhum endereço; bind faz justamente o processo de atribuir um endereço a esse socket.
	//int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	if(bind(server, (struct sockaddr*)&address, sizeof(address)) < 0 ){
		perror("bind");
		exit(1);
	}
	//listen marca o socket como um socket passivo, ou seja, ele vai receber requisições usando accept()
	//int listen(int sockfd, int backlog);
	if(listen(server, 10) < 0){
		perror("listen");
		exit(1);
	}

	printf("Listening in port 8080\n");
	while(1){
		//accept é uma syscall que extrai a primeira conexão na pilha de conexões pendentes e retorna um novo file descriptor que se refere a aquele socket
		int client = accept(server,NULL,NULL);
		if(client < 0){
			perror("accept");
			exit(1);
		}	
		printf("client fd %d\n", client);
		char request[4096];
		//função le um numero de bytes de um file descriptor(ou stream) e coloca dentro de um buffer
		//ssize_t read(int fd, void buf[.count], size_t count);
		ssize_t n = read(client, request, sizeof(request) - 1);
		if(n <= 0){
			close(client);
			continue;
		}

		request[n] = '\0';
		char method[8], path[256];
		if(sscanf(request, "%7s %255s",method, path) != 2){
			close(client);
			continue;
		}	
		printf("%s %s \n",method, path);

		if(strstr(path, "..") != NULL){
			char *response = 
			   "HTTP/1.0 404 NOT FOUND \r\n"
			   "Content-Type: text/html\r\n"
			   "\r\n"
			   "<h1>404, btw </h1>\n";	   

			write(client, response, strlen(response));
			close(client);
			continue;
		}

		char filename[512];
		//formata uma string e guarda ela
		snprintf(
			filename,
			sizeof(filename),
			"www%s",
			//strcmp compara duas strings, nesse caso comparamos path com / se forem iguais ele retorna zero. Se o usuario acessar / ele substitui por index. Se usuario pediu qualquer outra coisa ele mantém o path digitado
			strcmp(path,"/") == 0 ? "/index.html" : path
		);

		struct stat info;
		int file = -1;

		//verifica se o arquivo solicitado pelo cliente existe, é um arquivo válido e pode ser aberto pela leitura; respectivamente
		//stat busca informações sobre um arquivo especificado em filename e guarda dados na struct info.
		//S_ISREG() é um macro que verifica se o arquivo é um arquivo válido e não uma pasta por exemplo.
		//open vai ver se é possível abrir o arquivo
		if(stat(filename, &info) < 0 || !S_ISREG(info.st_mode) || (file = open(filename,O_RDONLY)) < 0 ){
			char *response = 
			   "HTTP/1.0 404 NOT FOUND \r\n"
			   "Content-Type: text/html\r\n"
			   "\r\n"
			   "<h1>404, btw </h1>\n";	   

			write(client, response, strlen(response));
			close(client);
			continue;
		}
		char header[256];

		int length = snprintf(header, sizeof(header), 
		   "HTTP/1.0 200 OK \r\n"
	   	   "Content-Type: %s\r\n"
		   "Content-Length: %ld\r\n"
		   "\r\n",
		   content_type(filename), (long)info.st_size
	    );	   
		
		//escreve uma quantidade de bytes em um arquivo ou stream
		//ssize_t write(int fd, const void buf[.count], size_t count);
		write(client, header, length);

		char buffer[4096];
		ssize_t count;

		while((count = read(file, buffer, sizeof(buffer))) > 0 ){
			write(client, buffer, count);
		}


		close(file);
		close(client);
	}

	return 0;
}
