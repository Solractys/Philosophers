# Philosophers — Guia de Construção Passo a Passo

> Um walkthrough didático para montar o projeto do zero, na ordem em que ele
> realmente nasce, explicando *o porquê* de cada decisão — não só o *como*.
>
> Este guia segue a implementação real deste repositório (`philo/`). A ideia é
> que você consiga reconstruir o projeto inteiro acompanhando o meu raciocínio, e
> não copiando código pronto. Sempre que eu introduzir uma função, ela vai ser
> usada naquele mesmo momento — nada de código morto esperando para ser
> ligado depois.

---

## Como usar este guia

- Leia na ordem. Cada parte depende da anterior.
- Quando aparecer pare e pense, pare mesmo e tente responder antes de
  continuar. É assim que o conteúdo gruda.
- Quando aparecer teste agora, compile e rode. Programação concorrente é
  cruel com quem só lê: você precisa *ver* acontecendo.
- As caixas para aprofundar trazem links de documentação e material extra.

Pré-requisitos: você já fez `libft` (ou pelo menos sabe escrever um `ft_atoi`),
manja de ponteiros, `malloc/free`, structs e Makefile. Threads e mutexes não
são pré-requisito — é exatamente isso que vamos aprender aqui.

---

## Índice

0. [Entendendo o problema antes de escrever uma linha](#parte-0--entendendo-o-problema)
1. [O esqueleto: `main` e validação de argumentos](#parte-1--o-esqueleto-main--validação)
2. [As estruturas de dados (`philo.h`)](#parte-2--as-estruturas-de-dados)
3. [Inicialização: tempo, filósofos e garfos](#parte-3--inicialização)
4. [A primeira thread: dando vida à mesa](#parte-4--a-primeira-thread)
5. [Imprimindo sem bagunça: o `print_lock`](#parte-5--imprimindo-sem-bagunça)
6. [Pegando garfos e o fantasma do *deadlock*](#parte-6--pegando-garfos-e-o-deadlock)
7. [Comendo de verdade: `ft_eat` e o `ft_usleep` preciso](#parte-7--comendo-de-verdade)
8. [O monitor: detectando a morte](#parte-8--o-monitor)
9. [Parada por refeições: `ft_check_full`](#parte-9--parada-por-refeições)
10. [O caso especial: um único filósofo](#parte-10--o-caso-especial-um-único-filósofo)
11. [O Makefile completo](#parte-11--o-makefile-completo)
12. [Testando como um avaliador](#parte-12--testando-como-um-avaliador)
13. [Armadilhas comuns e checklist final](#parte-13--armadilhas-comuns-e-checklist)

---

## Parte 0 — Entendendo o problema

Antes de qualquer código: **o que estamos resolvendo?

### A história

Há `N` filósofos numa mesa redonda. Entre cada par de vizinhos há um garfo.
Logo, com `N` filósofos, há `N` garfos. Cada filósofo faz, em loop, três coisas:

```
pensar  →  pegar 2 garfos  →  comer  →  largar garfos  →  dormir  →  pensar ...
```

O problema: para comer, um filósofo precisa de dois garfos (o da esquerda
e o da direita). Mas o garfo da sua direita é o garfo da esquerda do vizinho.
Ou seja, vizinhos disputam o mesmo garfo.

Regras do subject que viram requisitos de código:

- Cada filósofo é uma thread.
- Cada garfo é protegido por um mutex (só um filósofo segura um garfo por vez).
- Se um filósofo passar `time_to_die` milissegundos sem começar a comer, ele
  morre e a simulação para.
- Os eventos têm que ser impressos num formato exato:
  `timestamp_em_ms  id_do_filosofo  ação`.
- Não pode haver *data race* (dois threads mexendo no mesmo dado ao mesmo tempo)
  nem *deadlock* (todo mundo travado esperando um pelo outro).

### Os três conceitos que você precisa carregar no bolso

1. **Thread**: uma linha de execução independente dentro do mesmo processo.
   Todas as threads compartilham a mesma memória (o mesmo `malloc`). Isso é
   poderoso e perigoso ao mesmo tempo: poderoso porque elas conversam fácil;
   perigoso porque podem pisar uma na outra.

2. **Data race / condição de corrida**: quando duas threads acessam a mesma
   variável "ao mesmo tempo" e pelo menos uma escreve. O resultado vira loteria.
   Ex.: duas threads fazendo `contador++` ao mesmo tempo podem perder um
   incremento, porque `++` não é uma operação atômica (é ler → somar → escrever).

3. **Mutex** (*mutual exclusion*): um cadeado. `lock` pega o cadeado (se outra
   thread já pegou, você *espera*); `unlock` devolve. Tudo que estiver entre
   `lock` e `unlock` só roda em uma thread por vez. É a nossa ferramenta contra
   data races.

E o vilão da concorrência:

4. **Deadlock**: todo mundo travado para sempre. O exemplo clássico é *este*
   projeto: se todos os filósofos pegarem o garfo da esquerda ao mesmo tempo,
   ninguém consegue pegar o da direita (cada direita está na mão do vizinho).
   Resultado: a mesa congela. Vamos cair nessa armadilha de propósito na Parte 6
   para você sentir na pele — e depois consertar.

### A arquitetura que vamos construir

- **Uma thread por filósofo**, rodando a rotina comer/dormir/pensar.
- **Uma thread "monitor"** (no nosso caso, a própria thread principal depois de
  criar as outras) que fica vigiando se alguém morreu de fome ou se todos já
  comeram o suficiente.
- **Mutexes**: um por garfo, mais um para a impressão (`print_lock`) e um para
  proteger o estado compartilhado (`state_lock`).

> **Pare e pense:** por que precisamos de uma thread *separada* só para vigiar
> a morte? Por que o próprio filósofo não checa se ele mesmo morreu?
>
> <details><summary>resposta</summary>
> Porque enquanto o filósofo está "comendo" ou "dormindo", ele está dentro de um
> <code>usleep</code> — ele não está executando código para perceber que o tempo
> de morrer estourou. Alguém de fora precisa cronometrar. Esse alguém é o monitor.
> </details>

> **Para aprofundar:**
> - [Dining philosophers problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem) — a teoria do problema e as soluções clássicas.
> - [POSIX Threads Programming — LLNL](https://hpc-tutorials.llnl.gov/posix/) — o melhor tutorial gratuito de pthreads.
> - [Oceano — Philosophers in C (PT-BR)](https://www.youtube.com/watch?v=zOpzGHwJ3MU) — explicação em português.
> - *The Little Book of Semaphores*, de Allen Downey (PDF grátis) — capítulo "Dining Philosophers".

---

## Parte 1 — O esqueleto: `main` e validação

**Nunca confie no input**. Antes de criar threads, garfos
ou qualquer coisa cara, a gente valida os argumentos. Se a entrada é lixo, a
gente avisa e sai. Simples assim.

### 1.1 — A `main`

O programa recebe 4 ou 5 argumentos (além do nome):

```
./philo número_de_filósofos time_to_die time_to_eat time_to_sleep [num_refeições]
```

Então `argc` tem que ser 5 (sem o opcional) ou 6 (com ele). Começamos por aí:

```c
/* main.c */
#include "philo.h"

int	main(int ac, char **av)
{
	t_rule	*rules;

	if (ac < 5 || ac > 6)
	{
		print_error("Error: Invalid number of arguments\n");
		return (1);
	}
	rules = init_rules(ac, av);
	if (!rules)
	{
		print_error("Error: Invalid arguments\n");
		return (1);
	}
	ft_init_simulation(rules);
	free_rules(rules);
	return (0);
}
```

Repare na estrutura: `main` é um roteiro. Ela conta a história do programa em
quatro frases — valida quantidade de args, monta as regras, roda a simulação,
limpa tudo. Cada uma dessas vira uma função. Vamos preenchê-las uma a uma.

Agora, `init_rules` e `ft_init_simulation` ainda não existem. Tudo bem: vamos
construir de baixo para cima, mas mantendo a `main` como nosso mapa.

### 1.2 — `print_error`: por que não usar `printf`?

```c
/* utils/print_error.c */
#include "../philo.h"

void	print_error(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		write(2, &str[i], 1);
		i++;
	}
}
```

Duas decisões aqui:

- Escrevemos em **fd 2** (`stderr`), não no 1 (`stdout`). Erros vão para a saída
  de erro — assim, se alguém fizer `./philo ... > log.txt`, a mensagem de erro
  não suja o arquivo de log.
- Usamos `write` em vez de `printf`. `printf` é pesado e bufferizado; `write` é
  uma syscall direta. Para uma mensagenzinha de erro, `write` é mais honesto.

> **🧠 Pare e pense:** dá pra escrever a string inteira de uma vez com
> `write(2, str, ft_strlen(str))`. Por que o código escreve **caractere por
> caractere**? Resposta curta: funciona dos dois jeitos; o autor escolheu não
> depender do `strlen` aqui. Os dois são aceitáveis na norma.

### 1.3 — Um Makefile mínimo (para conseguirmos compilar *já*)

Não dá pra "desenvolver na hora" sem compilar a cada passo. Então criamos um
Makefile enxuto agora e vamos adicionando arquivos em `SRC_FILES` conforme
eles nascem:

```make
NAME      = philo
CCFLAGS   = cc -Wall -Wextra -Werror -g3

SRC_FILES = main.c \
            parser.c \
            utils/print_error.c \
            utils/utils.c
# (vamos acrescentar mais arquivos aqui ao longo do guia)

OBJ_DIR   = obj
SRCS      = $(addprefix ./, $(SRC_FILES))
OBJS      = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

all: $(NAME)

$(OBJ_DIR)/%.o: ./%.c
	@mkdir -p $(dir $@)
	$(CCFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CCFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

As flags `-Wall -Wextra -Werror` são obrigatórias no 42 (todo warning vira erro).
A `-g3` adiciona símbolos de debug — essencial para usar `valgrind` e `gdb`
depois. Vamos ver a versão "bonita" e completa do Makefile na Parte 11.

### 1.4 — O parser

Agora o coração da validação. Queremos transformar `char *` em número, mas
rejeitando qualquer coisa estranha: letras, números negativos, números
gigantes que estourariam o `long`, strings vazias.

Primeiro, um `ft_strlen` (vamos precisar dele já já):

```c
/* utils/utils.c */
#include "../philo.h"

size_t	ft_strlen(char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}
```

Agora o `ft_atol` — converte string em `long`. Por que `long` e não `int`? Porque
`time_to_die` em ms pode ser grande, e queremos margem para detectar overflow.

```c
/* parser.c */
#include "philo.h"

long	ft_atol(char *str)
{
	int		i;
	int		sign;
	long	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}
```

É o `atoi` clássico: pula espaços e os caracteres de controle `\t \n \v \f \r`
(códigos 9 a 13), lê um sinal opcional, depois acumula dígitos.

Mas o `ft_atol` sozinho não valida — ele aceitaria `"12abc"` (lê 12 e para). Por
isso temos um validador separado, `check_arg`, que diz se a string é um
número limpo:

```c
int	check_arg(char *s)
{
	int	i;

	i = 0;
	if (ft_strlen(s) >= 11)
		return (1);
	if (s[i] == '+')
		i++;
	if (!s[i])
		return (1);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (1);
		i++;
	}
	return (0);
}
```

Decisões importantes aqui:

- `return (1)` significa "tem erro", `return (0)` significa "ok". É uma
  convenção comum em C: 0 = sucesso. Mantenha isso na cabeça, vai se repetir.
- `ft_strlen(s) >= 11`: um `int` vai no máximo até ~2,1 bilhões (10 dígitos).
  Qualquer string com 11+ caracteres de dígito com certeza estoura. É um
  jeito barato de barrar overflow sem fazer matemática. (Aceitamos o `+` inicial,
  por isso o limite é generoso.)
- `if (!s[i]) return (1)`: depois de pular um `+`, se a string acabou, era só
  um `"+"` — inválido.
- Aceitamos `+` inicial, mas não aceitamos `-`: o loop de dígitos vai falhar
  no `-`. Filósofos negativos não existem.

Aplicamos `check_arg` a todos os argumentos numéricos:

```c
int	parse_arguments(int ac, char **args)
{
	int	i;

	i = 1;
	while (i < ac)
	{
		if (args[i][0] == 0 || check_arg(args[i]))
			return (1);
		i++;
	}
	return (0);
}
```

`args[i][0] == 0` pega o caso da string vazia (`./philo "" 200 200 200`).

E, por fim, depois de converter os números, validamos os valores:

```c
int	validade_times(t_rule *rules)
{
	if (rules->time_to_die <= 0 || rules->time_to_eat <= 0
		|| rules->time_to_sleep <= 0)
		return (0);
	else if (rules->number_of_philosophers < 1
		|| rules->number_of_philosophers > 200)
		return (0);
	return (1);
}
```

(Note: aqui `return (1)` é "ok" — convenção invertida em relação ao `check_arg`.
Não é o ideal misturar as duas convenções no mesmo projeto, mas é assim que está,
e funciona porque cada chamador interpreta o retorno corretamente. Fica como
*nota de estilo* para você não se confundir.)

Por que limitar a 200 filósofos? Porque cada filósofo é uma thread + um
mutex. Com milhares de threads, sua máquina derrete e o `time_to_die` perde a
precisão (o SO não consegue acordar todas as threads na hora certa). O subject
sugere esse teto.

> **Pare e pense:** por que `time_to_eat` não pode ser `0`? Porque "comer em
> 0 ms" não faz sentido físico e quebraria a cronometragem (a refeição teria que
> atualizar `last_meal` e imediatamente acabar). Valores `<= 0` em qualquer tempo
> são rejeitados.

`validade_times` usa `rules`, que ainda não existe — então agora somos
forçados a criar a struct. Hora da Parte 2.

> **Para aprofundar:**
> - `man atoi`, `man strtol` — veja como a libc faz e por que `strtol` reporta overflow.
> - [Integer overflow (Wikipedia)](https://en.wikipedia.org/wiki/Integer_overflow) — por que limitar o tamanho da string importa.

---

## Parte 2 — As estruturas de dados

Em C, structs costumam crescer junto com o projeto. Vou mostrá-las já no
formato final dos campos que cada uma precisa, mas explicando *por que* cada
campo existe — e só incluindo o que vamos realmente usar.

### 2.1 — `t_rule`: o estado global da simulação

Tudo que é compartilhado por todos os filósofos mora aqui: os parâmetros, os
cadeados globais e o vetor de filósofos.

```c
/* philo.h (trecho) */
typedef struct s_philo	t_philo;   /* "promessa": esse tipo existe, defino já já */

typedef struct s_rule
{
	long				number_of_philosophers;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	long				number_of_meals;   /* -1 = sem limite */
	long				time_start;        /* marco zero, em ms */
	int					dead;              /* flag: alguém morreu? */
	pthread_mutex_t		print_lock;        /* cadeado da impressão */
	pthread_mutex_t		state_lock;        /* cadeado do estado compartilhado */
	t_philo				*philos;           /* vetor de filósofos */
}	t_rule;
```

Por que `typedef struct s_philo t_philo;` antes de definir `s_philo`? Porque
temos uma referência circular: `t_rule` aponta para `t_philo*`, e cada `t_philo`
vai apontar de volta para `t_rule*`. O compilador precisa saber que "`t_philo`
é um tipo" antes de usá-lo. Essa linha é uma forward declaration (declaração
adiantada).

> **Pare e pense:** por que `number_of_meals = -1` para "sem limite"? Porque
> nenhum contador real de refeições é negativo. Usar `-1` como sentinela nos
> deixa diferenciar "o usuário não passou o 5º argumento" de "o usuário passou 0".

### 2.2 — `t_philo`: um lugar na mesa

```c
typedef struct s_philo
{
	long				id;          /* 0, 1, 2 ... (interno, 0-indexado) */
	pthread_t			thread;      /* a thread deste filósofo */
	long				last_meal;   /* timestamp da última refeição (ms) */
	int					how_much_eat;/* quantas vezes já comeu */
	pthread_mutex_t		*left_fork;  /* garfo da esquerda (é dono) */
	pthread_mutex_t		*right_fork; /* garfo da direita (emprestado do vizinho) */
	t_rule				*rules;      /* ponteiro de volta ao estado global */
}	t_philo;
```

Cada campo tem um motivo concreto:

- **`id`**: identifica o filósofo. É 0-indexado internamente; na impressão
  mostramos `id + 1`, porque humanos contam a partir de 1.
- **`thread`**: o handle da thread, preenchido por `pthread_create`.
- **`last_meal`**: o monitor compara `agora - last_meal` com `time_to_die`. É o
  dado mais sensível do projeto — vamos protegê-lo com mutex.
- **`how_much_eat`**: para a condição de parada opcional (todos comeram N vezes).
- **`left_fork` / `right_fork`**: ponteiros para mutexes. Por que ponteiros e não
  o mutex em si? Porque um garfo é compartilhado entre dois vizinhos. O garfo
  físico é um só; os dois filósofos precisam apontar para o mesmo mutex.
- **`rules`**: a thread recebe só `void *` do `pthread_create`. Para a thread
  conseguir enxergar o estado global (os tempos, os cadeados), cada filósofo
  carrega um ponteiro de volta para `rules`.

> **Nota de limpeza:** se você olhar versões antigas, pode ver um campo
> `int is_dead` dentro de `t_philo`. Neste projeto ele não é usado — a morte
> é controlada por `rules->dead`. Campo que não é lido por ninguém é peso morto:
> não o inclua. (Mantenha suas structs enxutas; o avaliador percebe.)

> **Para aprofundar:**
> - `man pthread_mutex_t`, `man pthread_t` — os tipos opacos do pthreads.
> - [Forward declaration (C)](https://en.wikipedia.org/wiki/Forward_declaration) — por que a "promessa" de tipo funciona.

---

## Parte 3 — Inicialização

Agora preenchemos `init_rules` — a função que a `main` chama para montar tudo.
A ordem importa: precisamos do tempo antes de carimbar `time_start`, e dos
filósofos antes dos garfos.

### 3.1 — A função do tempo

Tudo em Philosophers é medido em milissegundos. Criamos um relógio:

```c
/* simulation.c (vamos começar este arquivo aqui) */
#include "philo.h"

long	ft_get_time(void)
{
	struct timeval	clock;

	gettimeofday(&clock, NULL);
	return ((clock.tv_sec * 1000) + (clock.tv_usec / 1000));
}
```

`gettimeofday` te dá segundos (`tv_sec`) e microssegundos (`tv_usec`) desde
01/01/1970. Convertendo tudo para ms: segundos × 1000 + microssegundos ÷ 1000.

Esse número absoluto é gigante e não interessa por si só. O que interessa é a
**diferença**: `ft_get_time() - time_start` nos dá "quantos ms desde que a
simulação começou". É por isso que guardamos `time_start`.

> **Pare e pense:** por que ms e não µs? Porque o subject pede timestamps em
> ms e porque o SO não garante precisão sub-milissegundo no agendamento de
> threads. Trabalhar em ms é preciso o suficiente e simples.

### 3.2 — Distribuindo os números pela mesa

```c
/* init_philo.c (novo arquivo) */
#include "philo.h"

void	put_number_on_table(t_rule **rules, int ac, char **av)
{
	(*rules)->number_of_philosophers = ft_atol(av[1]);
	(*rules)->time_to_die = ft_atol(av[2]);
	(*rules)->time_to_eat = ft_atol(av[3]);
	(*rules)->time_to_sleep = ft_atol(av[4]);
	(*rules)->time_start = ft_get_time();
	(*rules)->number_of_meals = -1;
	if (ac == 6)
		(*rules)->number_of_meals = ft_atol(av[5]);
}
```

Simples: converte cada argumento e guarda. O `number_of_meals` recebe o default
`-1` e só vira o valor real se o 6º argumento existir.

(O `time_start` é carimbado aqui, mas vamos re-carimbá-lo na hora exata em
que as threads começam — Parte 4. Tudo bem: o que vale é o último carimbo, feito
imediatamente antes do `pthread_create`.)

### 3.3 — Criando os filósofos

```c
int	init_philosophers(t_rule *rules)
{
	int	i;

	i = 0;
	rules->philos = malloc(sizeof(t_philo) * rules->number_of_philosophers);
	if (!rules->philos)
		return (0);
	while (i < rules->number_of_philosophers)
	{
		rules->philos[i].id = i;
		rules->philos[i].last_meal = 0;
		rules->philos[i].left_fork = NULL;
		rules->philos[i].right_fork = NULL;
		rules->philos[i].how_much_eat = 0;
		rules->philos[i].rules = rules;
		i++;
	}
	if (!init_left_forks(rules))
		return (0);
	link_forks(rules);
	return (1);
}
```

Alocamos o vetor de filósofos e inicializamos todo campo (nunca deixe lixo de
memória num campo — `malloc` não zera nada). Note que cada filósofo recebe
`rules->philos[i].rules = rules` — é o ponteiro de volta que comentamos.

Os garfos começam `NULL` de propósito: se a alocação de garfos falhar no meio do
caminho, o `free_rules` (logo abaixo) saberá quais já foram criados olhando para
o `NULL`.

### 3.4 — Os garfos (a parte mais elegante)

Cada filósofo é dono de um garfo: o da sua esquerda. Alocamos e inicializamos
um mutex para cada:

```c
int	init_left_forks(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		rules->philos[i].left_fork = malloc(sizeof(pthread_mutex_t));
		if (!rules->philos[i].left_fork)
			return (0);
		if (pthread_mutex_init(rules->philos[i].left_fork, NULL) != 0)
		{
			pthread_mutex_destroy(rules->philos[i].left_fork);
			free(rules->philos[i].left_fork);
			return (0);
		}
		i++;
	}
	return (1);
}
```

`pthread_mutex_init` prepara o cadeado. Ele pode falhar (retorna != 0), e um bom
projeto trata isso: destruímos e liberamos o que acabamos de criar antes de
retornar erro.

Agora a sacada: cada filósofo só tem o garfo da esquerda. De onde vem o da
direita? É o garfo da esquerda do vizinho. Em vez de alocar garfos duplicados,
fazemos o `right_fork` apontar para o `left_fork` do próximo:

```c
void	link_forks(t_rule *rules)
{
	int		i;
	long	n;

	i = 0;
	n = rules->number_of_philosophers;
	while (i < n)
	{
		rules->philos[i].right_fork = rules->philos[(i + 1) % n].left_fork;
		i++;
	}
}
```

O `% n` (módulo) faz a mesa ser redonda: o garfo direito do último filósofo é
o garfo esquerdo do primeiro. Assim, garfos adjacentes são literalmente o mesmo
mutex — exatamente como o garfo físico compartilhado entre dois vizinhos.

```
   filósofo 0        filósofo 1        filósofo 2
   left=mutex0       left=mutex1       left=mutex2
   right=mutex1 ────▶ (mesmo)          right=mutex0  ◀──── volta (mesa redonda)
                right=mutex2 ────▶ (mesmo)
```

> **Pare e pense:** se cada filósofo alocasse dois garfos próprios, o que
> aconteceria? Você teria dois mutexes diferentes para o "mesmo" garfo físico.
> Dois vizinhos "comeriam" ao mesmo tempo com o mesmo garfo — sem exclusão mútua
> nenhuma. O ponto inteiro do projeto desabaria.

### 3.5 — Montando tudo: `init_rules`

```c
t_rule	*init_rules(int ac, char **av)
{
	t_rule	*rules;

	if (parse_arguments(ac, av))
		return (NULL);
	rules = malloc(sizeof(t_rule));
	if (!rules)
		return (NULL);
	rules->philos = NULL;
	rules->dead = 0;
	if (pthread_mutex_init(&rules->print_lock, NULL) != 0
		|| pthread_mutex_init(&rules->state_lock, NULL) != 0)
	{
		free(rules);
		return (NULL);
	}
	put_number_on_table(&rules, ac, av);
	if (!validade_times(rules) || !init_philosophers(rules))
	{
		free_rules(rules);
		return (NULL);
	}
	return (rules);
}
```

Sequência de cuidado:

1. Valida o formato dos argumentos (`parse_arguments`). Lixo? Sai.
2. Aloca `rules`. Zera `philos` (para o cleanup saber que ainda não há vetor) e
   `dead`.
3. Cria os dois mutexes globais (`print_lock`, `state_lock`).
4. Distribui os números.
5. Valida os valores e cria filósofos/garfos. Qualquer falha → `free_rules`
   e `NULL`.

Cada `if` que pode falhar tem um caminho de saída limpo. Isso é o que separa um
projeto que passa de um que vaza memória ou crasha.

### 3.6 — A faxina: `free_rules`

A gente cria, a gente destrói. Para cada `malloc`, um `free`; para cada
`mutex_init`, um `mutex_destroy`. Escrevemos a faxina agora, junto com a
criação, para nunca esquecer.

```c
/* utils/free_rules.c */
#include "../philo.h"

void	destroy_forks(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		if (rules->philos[i].left_fork != NULL)
		{
			pthread_mutex_destroy(rules->philos[i].left_fork);
			free(rules->philos[i].left_fork);
		}
		i++;
	}
}

void	free_philosophers(t_rule *rules)
{
	if (!rules)
		return ;
	if (rules->philos)
	{
		destroy_forks(rules);
		free(rules->philos);
	}
}

void	free_rules(t_rule *rules)
{
	if (!rules)
		return ;
	if (rules->philos)
		free_philosophers(rules);
	pthread_mutex_destroy(&rules->print_lock);
	pthread_mutex_destroy(&rules->state_lock);
	free(rules);
}
```

Veja como o `if (left_fork != NULL)` salva o dia: se a alocação de garfos parou
na metade, só destruímos os que existem. Por isso inicializamos com `NULL` lá
atrás. Cuidado e descuido se pagam aqui.

> **Teste agora.** A `main` ainda chama `ft_init_simulation`, que não existe.
> Para testar só a inicialização, comente a linha `ft_init_simulation(rules);`
> temporariamente, adicione os novos arquivos ao `SRC_FILES` (`init_philo.c`,
> `simulation.c`, `utils/free_rules.c`) e rode:
>
> ```
> make && valgrind --leak-check=full ./philo 5 800 200 200
> ```
>
> Deve sair **"All heap blocks were freed -- no leaks are possible"**. Você ainda
> não vê filósofos comendo — mas já provou que aloca e libera tudo certinho. Esse
> é o tipo de vitória pequena que mantém você são num projeto de concorrência.

> **Para aprofundar:**
> - `man gettimeofday` — e por que `clock_gettime(CLOCK_MONOTONIC)` seria ainda melhor (relógio que não anda para trás).
> - `man pthread_mutex_init` / `man pthread_mutex_destroy` — o ciclo de vida de um mutex.

---

## Parte 4 — A primeira thread

Hora de dar vida à mesa. `ft_init_simulation` carimba o marco zero, cria uma
thread por filósofo, e depois assume o papel de monitor.

### 4.1 — Criando as threads

```c
/* simulation.c */
void	ft_init_simulation(t_rule *rules)
{
	int	i;

	i = 0;
	rules->time_start = ft_get_time();
	while (i < rules->number_of_philosophers)
	{
		rules->philos[i].last_meal = rules->time_start;
		pthread_create(&rules->philos[i].thread, NULL, ft_routine,
			&rules->philos[i]);
		i++;
	}
	ft_monitor(rules);          /* a thread principal vira o monitor */
	ft_join_threads(rules);
}
```

Pontos cruciais:

- **`rules->time_start = ft_get_time();`** logo antes do loop: aqui está o marco
  zero "de verdade". Todos os timestamps impressos serão relativos a este
  instante.
- **`last_meal = time_start`**: no nascimento, considera-se que o filósofo
  "acabou de comer". Senão o monitor acharia que ele já está faminto há um tempão
  e o mataria no instante 0.
- **`pthread_create(&thread, NULL, ft_routine, &philos[i])`**: cria a thread.
  - 1º arg: onde guardar o handle.
  - 2º: atributos (NULL = padrão).
  - 3º: a função que a thread vai rodar (`ft_routine`).
  - 4º: o argumento passado para essa função. Passamos `&philos[i]` — o
    endereço daquele filósofo. Por isso cada filósofo carrega o ponteiro
    `rules`: é a única forma de a thread reencontrar o estado global.

> **Pare e pense:** por que passar `&philos[i]` e não `i`? Se passássemos um
> índice, a thread teria que voltar até o vetor global — mas ela não tem acesso a
> ele sem um ponteiro. Passando o endereço do filósofo, a thread recebe tudo de
> que precisa em um pacote só. Cuidado clássico: nunca passe o endereço de uma
> variável de loop que muda (`&i`); passe o endereço de um elemento estável do
> vetor, como aqui.

### 4.2 — Esperando todo mundo terminar

```c
void	ft_join_threads(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_join(rules->philos[i].thread, NULL);
		i++;
	}
}
```

`pthread_join` bloqueia até a thread terminar. É o oposto do `create`. Sem o
`join`, a `main` poderia chamar `free_rules` enquanto as threads ainda rodam —
acessando memória já liberada (crash garantido). O `join` garante: "só sigo
adiante quando todos pararam".

### 4.3 — A rotina (versão esqueleto)

Vamos começar com a rotina mais simples possível, só para ver as threads
rodarem. Ela ainda não come nem dorme de verdade:

```c
void	*ft_routine(void *philo)
{
	t_philo	*tmp;

	tmp = (t_philo *)philo;
	printf("filosofo %ld nasceu\n", tmp->id + 1);
	return (NULL);
}
```

`ft_routine` recebe `void *` (assinatura exigida pelo `pthread_create`) e a
primeira coisa que faz é converter de volta para `t_philo *`. Esse vai-e-volta de
ponteiro é o idioma padrão de threads em C.

> **Teste agora.** Descomente o `ft_init_simulation(rules)` na `main`, adicione
> `ft_routine` e `ft_init_simulation`/`ft_join_threads`/`ft_get_time` (todos em
> `simulation.c`) ao build, compile e rode `./philo 5 800 200 200`. Você verá 5
> "nasceu". A ordem pode variar a cada execução — isso é concorrência: o SO
> decide quem roda primeiro. Acostume-se.

Esqueleto funcionando, vamos preencher a rotina de verdade — mas antes precisamos
resolver como imprimir sem que as threads embaralhem a saída.

> **Para aprofundar:**
> - `man pthread_create`, `man pthread_join` — leia com calma; são as duas funções centrais.
> - [Unix threads in C — playlist (CodeVault)](https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2) — visual e prática.

---

## Parte 5 — Imprimindo sem bagunça

Cinco threads chamando `printf` ao mesmo tempo é receita de desastre: uma linha
começa, outra interrompe no meio, e você recebe `1 2 is ea1 3 is eating\nting`.
A saída precisa ser **atômica por linha**. Solução: um mutex só para impressão.

```c
/* philo_routine.c (novo arquivo) */
#include "./philo.h"

void	ft_print_mutex(char *str, long id, t_rule *rules)
{
	pthread_mutex_lock(&rules->print_lock);
	if (!ft_is_dead(rules))
		printf("%ld %ld %s\n", ft_get_time() - rules->time_start, id + 1, str);
	pthread_mutex_unlock(&rules->print_lock);
}
```

Três coisas acontecendo:

1. **O cadeado `print_lock`** garante que só uma thread imprime por vez. A linha
   sai inteira ou não sai.
2. **O formato exato do subject**: `timestamp id ação`. O timestamp é
   `ft_get_time() - time_start` (ms relativos ao início). O `id + 1` converte do
   0-indexado interno para o 1-indexado humano.
3. **`if (!ft_is_dead(rules))`**: não imprimimos nada **depois** que alguém
   morreu. Isso evita o bug clássico em que um filósofo imprime "is eating" *uma
   linha depois* da mensagem "died" — proibido pelo subject. (Vamos definir
   `ft_is_dead` na Parte 8; por ora, saiba que ele lê com segurança a flag
   `rules->dead`.)

> **🧠 Pare e pense:** por que o `ft_is_dead` está **dentro** do `print_lock`, e
> não antes dele? Porque queremos que a decisão "morreu?" e o ato de imprimir
> sejam indivisíveis. Se checássemos fora, o estado poderia mudar entre o check e
> o `printf`. Manter os dois sob o mesmo cadeado fecha essa fresta.

> **Sobre buffering (um detalhe que vai te morder):** `printf` escreve num buffer.
> Quando a saída é o terminal, ela é *line-buffered* (some a cada `\n`, então você
> vê tudo ao vivo). Mas quando você redireciona para um arquivo
> (`./philo ... > log.txt`), vira *fully-buffered*: nada aparece até o buffer
> encher ou o programa terminar normalmente. Se você **matar** o processo
> (`Ctrl+C`), perde o que estava no buffer. Não é bug no seu código — é como o C
> funciona. Para depurar, prefira rodar no terminal ou usar uma condição de
> parada (refeições) para o programa sair sozinho.

---

## Parte 6 — Pegando garfos e o *deadlock*

Agora o filósofo precisa **pegar dois garfos** antes de comer. Vamos começar
pela versão ingênua, ver ela travar, e consertar. É o jeito de aprender que gruda.

### 6.1 — A versão ingênua (que trava!)

Imagine que todo filósofo faz:

```c
/* NÃO use esta versão — é para você ver o problema */
pthread_mutex_lock(philo->left_fork);
pthread_mutex_lock(philo->right_fork);
```

Cenário: todos os filósofos acordam juntos, e cada um pega seu garfo da
**esquerda** ao mesmo tempo. Agora todos tentam pegar o da direita... mas todo
garfo da direita está na mão esquerda do vizinho. **Ninguém solta, ninguém come,
a mesa congela para sempre.** Isso é *deadlock* — e ele acontece pelas quatro
condições de Coffman, sendo a culpada principal a **espera circular**: 0 espera
1, que espera 2, ..., que espera 0.

### 6.2 — Quebrando a simetria

A solução clássica e simples: **nem todo mundo pega na mesma ordem**. Se metade
da mesa pega esquerda-depois-direita e a outra metade pega direita-depois-esquerda,
a espera circular não se fecha. Fazemos isso pela paridade do `id`:

```c
int	ft_get_fork(t_rule *rules, long id)
{
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	first = rules->philos[id].left_fork;
	second = rules->philos[id].right_fork;
	if (id % 2 == 0)
	{
		first = rules->philos[id].right_fork;
		second = rules->philos[id].left_fork;
	}
	pthread_mutex_lock(first);
	if (ft_is_dead(rules))
	{
		pthread_mutex_unlock(first);
		return (1);
	}
	pthread_mutex_lock(second);
	ft_print_mutex("has taken a fork", id, rules);
	return (0);
}
```

O que muda:

- Filósofos de `id` **par** pegam **direita primeiro**; os de `id` **ímpar** pegam
  **esquerda primeiro**. Essa assimetria quebra a espera circular.
- Depois de pegar o **primeiro** garfo, checamos `ft_is_dead`. Se a simulação já
  acabou enquanto esperávamos no `lock`, **soltamos o garfo** e retornamos `1`
  (sinal de "aborta, não vou comer"). Sem isso, uma thread poderia ficar segurando
  um garfo após a morte e travar o `join`.
- Só imprimimos "has taken a fork" depois de garantir os **dois**. (Tem
  implementações que imprimem a cada garfo; aqui o autor imprime uma vez por já
  ter os dois — observe que cada filósofo precisa de dois para comer.)

### 6.3 — Largando os garfos

```c
int	ft_put_fork(t_rule *rules, long id)
{
	pthread_mutex_unlock(rules->philos[id].left_fork);
	pthread_mutex_unlock(rules->philos[id].right_fork);
	return (0);
}
```

`unlock` pode ser na ordem que quiser — soltar não causa deadlock. O importante é
soltar **os dois**, sempre, depois de comer.

### 6.4 — O reforço: escalonar a largada

Além da paridade na ordem dos garfos, damos uma desencontrada no **início**: os
filósofos de id par esperam 1 ms antes de começar. Isso evita que todos disputem
os garfos no mesmo microinstante e melhora muito a fluidez (vamos colocar isso na
rotina, Parte 7):

```c
if (tmp->id % 2 == 0)
	usleep(1000);
```

> **🧠 Pare e pense:** a paridade na *ordem dos garfos* (6.2) já evita deadlock
> sozinha. Então por que o `usleep` de escalonamento? Porque sem deadlock o
> programa ainda pode ficar **injusto** — alguns filósofos comem muito, outros
> quase nunca, e algum acaba morrendo de fome em casos apertados (`4 410 200 200`).
> O escalonamento espalha a disputa no tempo e dá a todos uma chance justa. Um
> resolve *correção* (não travar); o outro resolve *desempenho/justiça*.

> **📚 Para aprofundar:**
> - [Coffman conditions / deadlock (Wikipedia)](https://en.wikipedia.org/wiki/Deadlock) — as 4 condições e como quebrá-las.
> - [Dining philosophers — resource hierarchy solution](https://en.wikipedia.org/wiki/Dining_philosophers_problem#Resource_hierarchy_solution) — a base teórica do nosso "par/ímpar".

---

## Parte 7 — Comendo de verdade

Com garfos resolvidos, montamos o ato de comer e o `sleep` preciso.

### 7.1 — `ft_eat`

```c
void	ft_eat(t_rule *rules, long id)
{
	if (ft_get_fork(rules, id))
		return ;
	pthread_mutex_lock(&rules->state_lock);
	rules->philos[id].last_meal = ft_get_time();
	pthread_mutex_unlock(&rules->state_lock);
	ft_print_mutex("is eating", id, rules);
	ft_usleep(rules, rules->time_to_eat);
	pthread_mutex_lock(&rules->state_lock);
	rules->philos[id].how_much_eat++;
	pthread_mutex_unlock(&rules->state_lock);
	ft_put_fork(rules, id);
}
```

A coreografia:

1. Pega os garfos. Se `ft_get_fork` retornou 1 (morreu durante a espera),
   abortamos sem comer.
2. **Atualiza `last_meal` sob `state_lock`.** Esse é o dado que o monitor lê para
   decidir se você morreu. Como duas threads (esta e o monitor) tocam nele,
   protegemos com mutex. Atualizamos `last_meal` **antes** de "is eating" porque o
   relógio da fome zera quando você *começa* a refeição.
3. Imprime "is eating".
4. `ft_usleep(time_to_eat)`: passa o tempo da refeição.
5. Incrementa `how_much_eat` (também sob `state_lock`) — comeu mais uma vez.
6. Larga os garfos.

> **🧠 Pare e pense:** por que `last_meal` e `how_much_eat` precisam de
> `state_lock`, mas a variável local `id` não? Porque `last_meal`/`how_much_eat`
> são lidos pelo **monitor** em paralelo. `id` só existe nesta thread. **Regra
> mental:** todo dado que é tocado por mais de uma thread, com pelo menos uma
> escrita, precisa de proteção.

### 7.2 — O `ft_usleep` preciso

Por que não usar `usleep(time_to_eat * 1000)` direto? Dois motivos:

1. **Imprecisão**: `usleep` garante dormir *no mínimo* aquele tempo, mas pode
   dormir mais (o SO acorda quando puder). Para `time_to_die` apertado, esse
   "a mais" pode matar um filósofo injustamente.
2. **Resposta à morte**: se você dorme 200 ms de uma vez e alguém morre no ms 50,
   você só vai perceber no ms 200. Queremos reagir rápido.

A solução é dormir em **fatias pequenas**, checando o relógio e a morte a cada
fatia:

```c
void	ft_usleep(t_rule *rules, long time)
{
	long	start;

	start = ft_get_time();
	while (ft_get_time() - start < time)
	{
		if (ft_is_dead(rules))
			break ;
		usleep(200);
	}
}
```

Dormimos de 200 µs em 200 µs até completar o tempo pedido — mas saímos na hora se
a simulação acabou. É um *sleep* que "presta atenção no mundo".

### 7.3 — A rotina completa

Agora preenchemos `ft_routine` de verdade (substituindo o esqueleto da Parte 4):

```c
/* simulation.c */
void	*ft_routine(void *philo)
{
	t_philo	*tmp;

	tmp = (t_philo *)philo;
	if (tmp->rules->number_of_philosophers == 1)
		return (ft_lonely(tmp->rules, tmp->id));   /* caso especial, Parte 10 */
	if (tmp->id % 2 == 0)
		usleep(1000);                              /* escalonamento, Parte 6.4 */
	while (!ft_is_dead(tmp->rules))
	{
		ft_eat(tmp->rules, tmp->id);
		ft_print_mutex("is sleeping", tmp->id, tmp->rules);
		ft_usleep(tmp->rules, tmp->rules->time_to_sleep);
		ft_print_mutex("is thinking", tmp->id, tmp->rules);
	}
	return (NULL);
}
```

O loop é a vida do filósofo: **comer → dormir → pensar**, repetindo até alguém
morrer ou todos ficarem satisfeitos (a flag `dead` será ligada nos dois casos —
ver Partes 8 e 9). A condição `while (!ft_is_dead(...))` é o que faz a thread
terminar sozinha quando a simulação acaba — e é por isso que o `pthread_join`
consegue retornar.

> **🧠 Pare e pense:** repare que não há `ft_print_mutex("is thinking")` *antes*
> de comer no início. O primeiro evento de cada filósofo é pegar garfo / comer.
> "Pensar" aqui aparece depois de dormir, fechando o ciclo. Algumas
> implementações imprimem "is thinking" logo no começo — as duas leituras do
> subject são aceitas, desde que o ciclo faça sentido.

> **▶ Teste agora.** Adicione `philo_routine.c` ao build e rode:
> ```
> ./philo 4 410 200 200 5
> ```
> Você deve ver algo assim no começo (os ids podem variar):
> ```
> 0 2 has taken a fork
> 0 2 is eating
> 0 4 has taken a fork
> 0 4 is eating
> 200 2 is sleeping
> 200 4 is sleeping
> 200 1 has taken a fork
> 200 1 is eating
> ```
> Repare: filósofos **2 e 4** comem primeiro (ids pares pegam o garfo de um jeito),
> depois **1 e 3**. A mesa se organiza em dois turnos — é a quebra de simetria
> funcionando diante dos seus olhos.

> **📚 Para aprofundar:**
> - `man usleep`, `man nanosleep` — granularidade e por que dormir em fatias.

---

## Parte 8 — O monitor

Falta o vigia. Lembra: enquanto o filósofo dorme/come dentro de um `usleep`, ele
não checa nada. O monitor é a thread (aqui, a principal) que cronometra a fome de
todo mundo.

### 8.1 — Lendo e escrevendo a flag de morte com segurança

A flag `rules->dead` é lida por **todas** as threads (no `while` da rotina, no
print, no `ft_usleep`) e escrita pelo monitor. Logo: acesso protegido.

```c
/* monitor.c (novo arquivo) */
#include "philo.h"

int	ft_is_dead(t_rule *rules)
{
	int	ret;

	pthread_mutex_lock(&rules->state_lock);
	ret = rules->dead;
	pthread_mutex_unlock(&rules->state_lock);
	return (ret);
}

void	ft_set_dead(t_rule *rules)
{
	pthread_mutex_lock(&rules->state_lock);
	rules->dead = 1;
	pthread_mutex_unlock(&rules->state_lock);
}
```

Pode parecer exagero travar um mutex só para ler um `int`. Mas sem isso você tem
um *data race* clássico: o compilador pode manter `dead` em um registrador, e uma
thread nunca "ver" a mudança feita por outra. O mutex força a sincronização da
memória entre threads. (Esses são os data races que o `helgrind`/`drd` pegam — ver
Parte 12.)

### 8.2 — A mensagem de morte

```c
void	ft_print_death(t_rule *rules, long id)
{
	pthread_mutex_lock(&rules->print_lock);
	printf("%ld %ld died\n", ft_get_time() - rules->time_start, id + 1);
	pthread_mutex_unlock(&rules->print_lock);
}
```

Note: aqui **não** checamos `ft_is_dead` antes de imprimir (diferente do
`ft_print_mutex`). Faz sentido — esta *é* a mensagem que anuncia a morte; ela tem
prioridade. Mas ela ainda pega o `print_lock`, para não colidir com uma linha que
outra thread esteja imprimindo no mesmo instante.

### 8.3 — Checando a inanição

```c
int	ft_check_starvation(t_rule *rules)
{
	int		i;
	long	last;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_mutex_lock(&rules->state_lock);
		last = rules->philos[i].last_meal;
		if (ft_get_time() - last > rules->time_to_die)
		{
			rules->dead = 1;
			pthread_mutex_unlock(&rules->state_lock);
			ft_print_death(rules, i);
			return (1);
		}
		pthread_mutex_unlock(&rules->state_lock);
		i++;
	}
	return (0);
}
```

Para cada filósofo: lemos `last_meal` (sob `state_lock`, porque o `ft_eat` o
escreve em paralelo) e checamos se `agora - last_meal > time_to_die`. Se sim:

1. Marcamos `dead = 1` (ainda dentro do lock — já estamos com ele na mão).
2. **Soltamos o `state_lock` antes de imprimir.** Por quê? Para não segurar dois
   cadeados aninhados (`state_lock` + `print_lock` lá dentro), o que poderia
   gerar... deadlock. Soltamos um antes de pegar o outro.
3. Imprimimos a morte e retornamos 1.

A ordem aqui é cirúrgica: primeiro ligamos a flag (para que todas as threads
parem de imprimir suas ações), só **depois** imprimimos "died". Assim "died" é
sempre a última linha. Exatamente o que o subject exige.

### 8.4 — O loop do monitor

```c
/* utils/monitor_utils.c */
#include "../philo.h"

void	ft_monitor(t_rule *rules)
{
	while (1)
	{
		if (ft_check_starvation(rules))
			return ;
		if (ft_check_full(rules))    /* condição de parada opcional, Parte 9 */
			return ;
		usleep(500);
	}
}
```

O monitor roda em loop: checa fome, checa saciedade, dorme 500 µs, repete. O
`usleep(500)` evita que ele consuma 100% de CPU girando à toa — ele acorda
centenas de vezes por ms, mais que suficiente para precisão de ms.

Lembre que `ft_init_simulation` (Parte 4) chama `ft_monitor(rules)` **na thread
principal**, depois de criar os filósofos. Quando o monitor retorna (alguém
morreu ou todos comeram), a thread principal segue para o `ft_join_threads`. As
threads dos filósofos, vendo `ft_is_dead() == 1`, saem dos seus loops e o `join`
completa. Tudo se fecha lindamente.

> **▶ Teste agora.** Com `monitor.c` e `utils/monitor_utils.c` no build:
> ```
> ./philo 4 100 200 200
> ```
> Com `time_to_die = 100` mas `time_to_eat = 200`, é **impossível** comer a tempo:
> ```
> 0 2 has taken a fork
> 0 2 is eating
> 1 4 has taken a fork
> 1 4 is eating
> 101 1 died
> ```
> Alguém morre por volta do ms 100, e **nada** é impresso depois de "died". 

> **📚 Para aprofundar:**
> - [Memory visibility / why locking a read matters (helgrind manual)](https://valgrind.org/docs/manual/hg-manual.html) — por que ler sem mutex é data race.

---

## Parte 9 — Parada por refeições

O 5º argumento (opcional) diz quantas vezes **cada** filósofo deve comer. Quando
todos atingirem essa meta, a simulação para — sem ninguém morrer.

```c
/* monitor.c */
int	ft_check_full(t_rule *rules)
{
	int	i;
	int	full;

	if (rules->number_of_meals < 0)
		return (0);
	i = 0;
	full = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_mutex_lock(&rules->state_lock);
		if (rules->philos[i].how_much_eat >= rules->number_of_meals)
			full++;
		pthread_mutex_unlock(&rules->state_lock);
		i++;
	}
	if (full < rules->number_of_philosophers)
		return (0);
	ft_set_dead(rules);
	return (1);
}
```

A lógica:

- **`if (number_of_meals < 0) return (0)`**: lembra o sentinela `-1`? Se o usuário
  não passou o 5º argumento, essa condição de parada simplesmente não existe.
  Saímos na hora.
- Contamos quantos filósofos já bateram a meta (`how_much_eat >= number_of_meals`).
- Só se **todos** bateram, ligamos `dead` (reaproveitando a mesma flag que encerra
  tudo) e retornamos 1.

> **🧠 Pare e pense:** reusar a flag `dead` para "todos comeram" é elegante ou
> confuso? É eficiente — uma única flag encerra a simulação por qualquer motivo, e
> as threads já sabem reagir a ela. Mas o nome `dead` fica semanticamente estranho
> (ninguém morreu). Um refinamento seria uma flag `simulation_over`. Funciona como
> está; é uma escolha de clareza.

> **▶ Teste agora.**
> ```
> ./philo 5 800 200 200 3
> ```
> Cada um dos 5 filósofos come 3 vezes e o programa **termina sozinho**, sem
> "died". Confira o final da saída: as últimas ações são refeições/sono, e então
> silêncio.

---

## Parte 10 — O caso especial: um único filósofo

`./philo 1 800 200 200` é o teste favorito dos avaliadores. Pense: um filósofo,
**um** garfo. Ele nunca terá o segundo. Logo, ele **não pode comer** e tem que
**morrer** após `time_to_die`.

Se você jogasse esse caso na rotina normal, `ft_get_fork` tentaria travar o mesmo
mutex duas vezes (left e right apontam para o mesmo garfo, pois `(0+1) % 1 == 0`)
— a thread travaria nela mesma, *deadlock* de um só. Por isso tratamos à parte:

```c
/* simulation.c */
void	*ft_lonely(t_rule *rules, long id)
{
	ft_print_mutex("has taken a fork", id, rules);
	while (!ft_is_dead(rules))
		usleep(200);
	return (NULL);
}
```

O filósofo solitário pega **um** garfo (e anuncia), e então só espera. Ele nunca
come, então o monitor vai detectar a inanição e matá-lo após `time_to_die` ms. A
rotina já desviava para cá lá na Parte 7:

```c
if (tmp->rules->number_of_philosophers == 1)
	return (ft_lonely(tmp->rules, tmp->id));
```

> **▶ Teste agora.**
> ```
> ./philo 1 800 200 200
> ```
> Saída esperada:
> ```
> 1 1 has taken a fork
> 801 1 died
> ```
> Ele pega o garfo no ms ~1 e morre por volta do ms 800. Exatamente o comportamento
> exigido.

> **🧠 Pare e pense:** por que `usleep(200)` no loop, e não um `usleep(time_to_die)`
> único? Mesma lição do `ft_usleep`: dormir em fatias pequenas deixa a thread
> reagir rápido quando o monitor liga a flag `dead`. Um sono longo atrasaria o
> encerramento.

---

## Parte 11 — O Makefile completo

Ao longo do guia fomos adicionando arquivos. Eis a versão final, polida, com
diretório de objetos separado e um banner ASCII (charme opcional, mas divertido):

```make
NAME		= philo
CCFLAGS		= cc -Wall -Wextra -Werror -g3

SRC_DIR		= ./
OBJ_DIR		= obj

SRC_FILES	= main.c \
						parser.c \
						init_philo.c \
						utils/utils.c \
						utils/print_error.c \
						utils/monitor_utils.c \
						simulation.c \
						philo_routine.c \
						monitor.c \
						utils/free_rules.c

SRCS		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS		= $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CCFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CCFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf philo
	rm -rf $(OBJ_DIR)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
```

Pontos do Makefile que valem entender:

- **`$(SRC_FILES:.c=.o)`**: substituição de sufixo. Transforma `main.c` em
  `main.o`. É como o make sabe quais objetos gerar.
- **A regra padrão `$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c`**: ensina o make a compilar
  *qualquer* `.c` em `.o`. O `@mkdir -p $(dir $@)` cria subpastas (como
  `obj/utils/`) antes de compilar.
- **`.PHONY`**: avisa o make que `all`, `clean` etc. são *comandos*, não arquivos.
  Sem isso, se existisse um arquivo chamado `clean`, o make acharia que não há
  nada a fazer.
- **`-g3`** continua aqui: você vai querer `valgrind` com símbolos.

> **🧠 Pare e pense:** por que separar `.o` em `obj/` em vez de deixar tudo na raiz?
> Organização e builds incrementais limpos. E o `fclean` apaga a pasta inteira de
> uma vez. Pequenos hábitos que mantêm o projeto arrumado.

> **📚 Para aprofundar:**
> - [GNU Make manual — pattern rules](https://www.gnu.org/software/make/manual/html_node/Pattern-Rules.html)
> - `man make`

---

## Parte 12 — Testando como um avaliador

Compilar e "parecer funcionar" não basta em concorrência. Bugs de thread são
*intermitentes* — passam 9 vezes e quebram na 10ª. Teste de verdade:

### 12.1 — Os cenários canônicos

```bash
# Deve morrer (um filósofo, sem segundo garfo)
./philo 1 800 200 200          # -> "died" por volta de 800ms

# Ninguém deve morrer (roda "para sempre")
./philo 5 800 200 200
./philo 4 410 200 200          # caso apertado: a quebra de simetria precisa salvar

# Deve parar quando todos comem N vezes (sem mortes)
./philo 5 800 200 200 7

# Caso de morte óbvia (impossível comer a tempo)
./philo 4 310 200 100          # alguém morre

# Argumentos inválidos (deve recusar com mensagem em stderr)
./philo 4 200 200              # poucos argumentos
./philo 0 200 200 200          # zero filósofos
./philo 4 abc 200 200          # não-número
./philo 4 -5 200 200           # negativo
```

### 12.2 — Caçando *data races* com Helgrind / DRD

Esta é a ferramenta que separa "funciona na minha máquina" de "está correto":

```bash
valgrind --tool=helgrind ./philo 4 410 200 200 3
valgrind --tool=drd      ./philo 4 410 200 200 3
```

Elas detectam acessos a memória compartilhada sem proteção de mutex. Se aparecer
"Possible data race", você tem um campo desprotegido — provavelmente um acesso a
`last_meal`, `dead` ou `how_much_eat` sem o `state_lock`. (É por isso que
blindamos *todos* os acessos a esses campos. Use a condição de refeições para o
programa terminar, senão o valgrind roda eternamente.)

### 12.3 — Vazamentos de memória

```bash
valgrind --leak-check=full --show-leak-kinds=all ./philo 5 800 200 200 5
```

Tem que dar **zero** bytes vazados e **zero** mutexes não destruídos. Se vazar,
revise o par criar/destruir da Parte 3.6.

### 12.4 — A morte tem que ser pontual

O subject permite no máximo ~10 ms de atraso entre a morte real e a impressão de
"died", e **nenhuma** ação pode ser impressa depois de "died". Cheque os
timestamps: em `./philo 1 800 200 200`, o "died" deve sair perto de 800, não de
850.

> **📚 Para aprofundar:**
> - [Helgrind manual](https://valgrind.org/docs/manual/hg-manual.html) e [DRD manual](https://valgrind.org/docs/manual/drd-manual.html)
> - [Lazy Philosophers Tester](https://github.com/MichelleJiam/LazyPhilosophersTester) — tester automatizado da comunidade 42.

---

## Parte 13 — Armadilhas comuns e checklist

### Armadilhas que pegam quase todo mundo

1. **Acessar `last_meal`/`dead`/`how_much_eat` sem mutex.** Funciona "quase
   sempre" — e quebra na avaliação. Toda leitura/escrita de dado compartilhado
   passa pelo `state_lock`.
2. **Imprimir depois de "died".** Por isso o `ft_print_mutex` checa `ft_is_dead`
   *dentro* do `print_lock`.
3. **`usleep` longo único.** Mata a precisão e a reatividade. Sempre durma em
   fatias (`ft_usleep`).
4. **Esquecer o caso de 1 filósofo.** Trava num self-deadlock se cair na rotina
   normal. Desvie para `ft_lonely`.
5. **Não desencontrar o início.** Sem o `usleep(1000)` nos pares (ou alguma
   estratégia equivalente), casos apertados ficam injustos e alguém morre sem
   precisar.
6. **Segurar dois mutexes aninhados.** No `ft_check_starvation`, soltamos o
   `state_lock` *antes* de pegar o `print_lock`. Aninhar cadeados é como deadlocks
   nascem.
7. **`pthread_create` sem o `join` correspondente.** A `main` liberaria memória
   com threads ainda rodando. Sempre crie e depois junte.
8. **Campos mortos na struct.** Um campo que ninguém lê (como o antigo
   `is_dead` em `t_philo`) é confusão. Remova.

### Checklist final

- [ ] Compila com `-Wall -Wextra -Werror`, zero warnings.
- [ ] `./philo 1 800 200 200` → morre por volta de 800 ms.
- [ ] `./philo 5 800 200 200` → roda sem mortes (deixe rodar um tempão).
- [ ] `./philo 4 410 200 200` → ninguém morre (o caso apertado).
- [ ] `./philo 5 800 200 200 7` → para quando todos comem 7×, sem mortes.
- [ ] Nenhuma ação impressa depois de "died".
- [ ] `valgrind --leak-check=full` → sem vazamentos, mutexes destruídos.
- [ ] `valgrind --tool=helgrind` → sem data races.
- [ ] Argumentos inválidos recusados com mensagem clara em `stderr`.
- [ ] Cada `malloc` tem seu `free`; cada `mutex_init` tem seu `mutex_destroy`.

### A ordem em que tudo foi montado (resumo)

```
0. Entender o problema (threads, mutex, deadlock)
1. main + print_error + parser + Makefile mínimo   → valida input
2. structs t_rule / t_philo                          → modela o mundo
3. init (tempo, filósofos, garfos) + free_rules      → monta e desmonta a mesa
4. pthread_create / join + rotina esqueleto          → threads ganham vida
5. print_lock (ft_print_mutex)                       → saída sem bagunça
6. ft_get_fork / ft_put_fork + quebra de simetria    → garfos sem deadlock
7. ft_eat + ft_usleep + rotina completa              → o ciclo comer/dormir/pensar
8. monitor (is_dead/set_dead/starvation/death)       → detecta a morte
9. ft_check_full                                     → parada por refeições
10. ft_lonely                                        → o caso de 1 filósofo
11. Makefile completo                                → empacota tudo
12. Testes (helgrind, valgrind, cenários)            → prova que está correto
```

---

## Materiais para continuar estudando

- **POSIX Threads — LLNL HPC Tutorials:** https://hpc-tutorials.llnl.gov/posix/
  — referência completa e didática de pthreads.
- **The Little Book of Semaphores (Allen Downey):** PDF grátis; o capítulo de
  Dining Philosophers cobre as soluções clássicas (hierarquia de recursos,
  garçom/arbitrário, etc.).
- **Operating Systems: Three Easy Pieces (OSTEP):** https://pages.cs.wisc.edu/~remzi/OSTEP/
  — capítulos de Concurrency, gratuitos. Leitura de ouro para entender o *porquê*.
- **CodeVault — Unix threads in C (playlist):**
  https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2
- **Oceano — Philosophers em C (PT-BR):** https://www.youtube.com/watch?v=zOpzGHwJ3MU
- **Repositórios de referência da comunidade 42:**
  - https://github.com/DeRuina/philosophers
  - https://github.com/zelhajou/ft_unix_philosophers
- **Manuais (use sempre!):** `man pthread_create`, `man pthread_join`,
  `man pthread_mutex_init`, `man pthread_mutex_lock`, `man gettimeofday`,
  `man usleep`.

> Última dica, no melhor espírito 42: **não copie este código.** Leia, entenda o
> *porquê* de cada parte, feche o guia e reescreva do zero com suas próprias mãos.
> O que você reconstrói sozinha, ninguém te tira. Bons estudos! 🍝
