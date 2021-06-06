#ifndef MENSAGEM_H
#define MENSAGEM_H


struct {
    unsigned char tipo;
    unsigned char origem;
    unsigned char tamanho;
    unsigned char repeticoes;
} typedef mensagem_t;

// Por conveniência, não é necessário
void escreve_mensagem (mensagem_t* m,
		       unsigned char tipo,
		       unsigned char origem,
		       unsigned char tamanho,
		       unsigned char repeticoes){
	m->tipo = tipo;
	m->origem = origem;
	m->tamanho = tamanho;
	m->repeticoes = repeticoes;
}

#define TAMANHO_DA_MENSAGEM 4

#endif // MENSAGEM_H
