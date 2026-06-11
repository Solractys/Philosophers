# Philosophers — Plano de desenvolvimento

> Projeto da 42 que implementa o problema dos filósofos jantando usando threads POSIX e mutexes.
> Cada tarefa inclui uma explicação do **o que fazer** e do **por quê** aquela etapa existe.
>
> **Modelo de dados real (em `philo/philo.h`):**
> - `t_rule` — parâmetros da simulação: `number_of_philosophers`, `time_to_die`, `time_to_eat`,
>   `time_to_sleep`, `number_of_meals` (sentinela `-1` quando o 5º argumento é omitido) e
>   `t_philo *philos`.
> - `t_philo` — contexto de cada filósofo: `id`, `pthread_t thread`, `last_meal`,
>   `t_fork *left_fork`, `t_fork *right_fork` e `t_rule *rules` (ponteiro de volta).
> - `t_fork` — um garfo: `pthread_mutex_t mutex` + `int id`.
>
> Não há array central de `pthread_t` nem de `pthread_mutex_t`: cada thread vive dentro do seu
> `t_philo`. Existem **exatamente `n` garfos**: cada filósofo aloca o seu `left_fork`
> individualmente, e `right_fork` é apenas um **ponteiro-alias** para o `left_fork` do vizinho
> (`philos[(i+1)%n].left_fork`) — não é uma alocação nova. Por isso só os `left_fork` são
> destruídos/liberados na limpeza; liberar `right_fork` seria double-free.

---

## Bugs conhecidos — corrigir antes de avançar

> Levantados a partir do código atual. Vários **bloqueiam fases posteriores**, então devem ser
> resolvidos antes de seguir. As referências `arquivo:linha` apontam para o ponto exato.

- [x] **B1 — `validade_times` rejeita `n = 1`** · `philo/init_philo.c:58`

  A condição `number_of_philosophers <= 1` trata 1 filósofo como entrada inválida, então
  `./philo 1 800 200 200` sai como "Invalid arguments". Isso **contradiz diretamente** a Fase 3.1,
  a 5.2 e a primeira linha da tabela de testes (5.3) — o subject exige que `n = 1` rode e o
  filósofo morra. **Correção:** aceitar `n == 1` (usar `< 1` em vez de `<= 1`).

- [x] **B2 — `n >= 200` rejeita o próprio 200** · `philo/init_philo.c:59`

  O teto de 200 não é exigido pelo subject (é autoimposto) e ainda corta o valor 200 em si.
  **Correção:** remover o limite ou, se quiser mantê-lo, usar `> 200`.

- [x] **B3 — Parsing não detecta lixo numérico nem overflow** · `philo/utils/ft_atoi.c`, `philo/init_philo.c:55`

  `ft_atoi("abc")` devolve `0` e `ft_atoi("12x")` devolve `12` — ambos passam silenciosamente.
  `validade_times` só rejeita valores `< 0`, então `time_to_* == 0` é aceito e `number_of_meals`
  não é validado. O "Por quê" da Fase 1.1 promete rejeitar não-numéricos, mas isso ainda não
  acontece. **Correção:** validar que cada argumento é composto só por dígitos (com sinal `+`
  opcional) antes de converter, rejeitar overflow de `int`, e validar `number_of_meals >= 0`.

- [x] **B4 — Cleanup quebra em inicialização parcial** · `philo/init_philo.c`, `philo/utils/free_rules.c`

  No loop de `init_philosophers` os campos de garfo **não eram inicializados** — só recebiam valor
  durante a alocação. Se essa falhasse num `malloc`/`mutex_init` intermediário, os filósofos
  seguintes ficavam com `left_fork` apontando para lixo. Aí `destroy_forks` — que só verificava
  `philos[0].left_fork` — percorria todos e chamava `pthread_mutex_destroy`/`free` em ponteiro
  inválido → segfault.
  **Resolvido:** `init_philosophers` zera `left_fork = NULL` e `right_fork = NULL` no loop;
  `init_left_forks` libera o bloco e o deixa `NULL` se o `mutex_init` falhar; e `destroy_forks`
  agora testa cada `left_fork` individualmente, pulando os `NULL`. Verificado com Valgrind
  (`--leak-check=full`) sem leaks para `n = 1`, `2`, `5` e `200`.

- [ ] **B5 — `last_meal` inicializado com `0`** · `philo/init_philo.c:44` *(latente)*

  Hoje não causa dano porque o monitor ainda não existe, mas vira bug assim que a Fase 4 entrar:
  o monitor calcularia `agora - 0` (valor enorme) e declararia morte no primeiro tick. `last_meal`
  deve valer o `start_time` da simulação. **Correção:** semear `last_meal = start_time` no
  lançamento das threads (Fase 4.2), já que `start_time` só é conhecido lá. É o ponto de
  sincronização entre as Fases 1.2 e 4.2.

---

## Fase 0 — Inicialização (concluída)

- [x] **Structs principais: `t_rule`, `t_philo`, `t_fork`**

  Definição das três structs descritas no topo deste documento: `t_rule` centraliza os parâmetros,
  `t_philo` carrega o contexto de cada filósofo (incluindo a própria `pthread_t`) e `t_fork`
  representa um garfo (mutex + id).

  **Por quê:** centralizar os parâmetros em `t_rule` evita variáveis globais (proibidas pela Norma)
  e dá a cada thread um único ponteiro de contexto. Embutir a thread no `t_philo` dispensa um array
  paralelo de `pthread_t`; e tornar o garfo um struct (em vez de um `pthread_mutex_t` cru) permite
  carregar um `id` útil em debug junto do mutex.

  > **A adicionar nas próximas fases:** `t_rule` ainda vai precisar de `start_time`, da flag `stop`
  > (com mutex próprio) e de um mutex de print; `t_philo` vai precisar de `meals_eaten` e de um
  > mutex para proteger `last_meal`/`meals_eaten`.

---

## Fase 1 — Parsing & Setup

- [x] **1.1 — Parsing e validação de argumentos** — *implementado, mas ver B1–B3*

  Ler `argc` e `argv`, rejeitar entradas inválidas (negativos, não-numéricos, args faltando) e
  popular os campos de tempo em `t_rule`. O quinto argumento — `number_of_times_each_philosopher_must_eat`
  — é opcional; quando ausente, fica como `-1` e a simulação roda até alguém morrer.

  **Por quê:** o subject exige que o programa encerre silenciosamente (sem crash nem comportamento
  indefinido) para qualquer entrada inválida. Um parsing robusto aqui evita UB em todas as fases
  seguintes. Além disso, converter os valores uma única vez e armazená-los na struct garante que
  todas as threads leiam os mesmos dados sem conversões repetidas.

  > **Estado atual:** o esqueleto existe (`init_rules`, `ft_atoi`, `validade_times`, `print_error`),
  > mas a validação ainda tem os bugs **B1** (`n = 1` rejeitado), **B2** (`200` rejeitado) e
  > **B3** (não-numéricos/overflow aceitos). Não considere a fase fechada até resolvê-los.

- [ ] **1.2 — Inicialização dos filósofos (`t_philo`)** — *implementação inicial em `init_philo.c`*

  Alocar e preencher o array `t_philo[n]` (`init_philosophers`). Cada filósofo precisa do seu `id`,
  ponteiro para `t_rule`, `last_meal` e `meals_eaten` zerado.

  **Por quê:** cada filósofo é uma thread independente que precisa de contexto próprio para
  operar sem acessar dados de outro filósofo. O `last_meal` precisa valer o tempo de início —
  não zero — porque o monitor começa a checar logo após as threads serem criadas; um zero faria
  o monitor declarar morte antes do primeiro ciclo (exatamente o **B5**).

  > **Estado atual:** `init_philosophers` já preenche `id`, `last_meal = 0`, `rules` e zera
  > `left_fork`/`right_fork` (`NULL`) — este último resolve o **B4**. **Pendências:** (1) o campo
  > `meals_eaten` ainda não existe no struct; (2) como `start_time` só é conhecido no lançamento
  > das threads, a inicialização real de `last_meal` acontece na **Fase 4.2**, não aqui.

- [x] **1.3 — Inicialização dos garfos (mutex por garfo)** — *em `init_philo.c` (`init_left_forks` + `link_forks`)*

  Modelo adotado: **existem exatamente `n` garfos** e cada filósofo aloca o próprio garfo esquerdo.
  A inicialização acontece em **dois passos**, e a ordem importa:
  1. `init_left_forks` — para cada `i`: `philos[i].left_fork = malloc(sizeof(t_fork))`,
     `pthread_mutex_init(&left_fork->mutex, NULL)` e `left_fork->id = i`. Se o `mutex_init` falhar,
     libera o bloco e o deixa `NULL` (cobre o **B4**).
  2. `link_forks` — atribui `philos[i].right_fork = philos[(i + 1) % n].left_fork`. **Sem `malloc`
     e sem `mutex_init`**: o garfo direito é só um *alias* para o garfo esquerdo do vizinho.

  **Por quê:** cada garfo é um recurso compartilhado entre dois filósofos vizinhos; o mutex garante
  apenas um portador por vez. O índice `% n` fecha o círculo — o último filósofo divide um garfo
  com o primeiro. O link **precisa** ser um segundo passo separado: na iteração `i`, o `left_fork`
  do vizinho `i+1` ainda não foi alocado, então ligá-lo no mesmo loop leria ponteiro lixo.

  > **Decisão de arquitetura (fechada):** adotado o campo `right_fork` em `t_philo`, apontando para
  > o mesmo mutex do garfo esquerdo do vizinho — deixa `take_forks`/`put_forks` (Fase 3.1) direto,
  > sem recalcular `philos[(i+1)%n].left_fork`. **Armadilha central:** `right_fork` **não** é um
  > garfo novo. Dar `malloc`/`mutex_init` nele cria `2n` garfos sem compartilhamento (vizinhos
  > travando mutexes distintos), o que destrói a exclusão mútua e ainda vaza memória. É só ponteiro.

---

## Fase 2 — Logging & Tempo

- [ ] **2.1 — Função de tempo em milissegundos**

  Implementar `long long get_time_ms(void)` usando `gettimeofday`. Retorna
  `tv_sec * 1000 + tv_usec / 1000`. O timestamp exibido no log é sempre
  `get_time_ms() - rules->start_time` (campo `start_time` a ser adicionado em `t_rule`).

  **Por quê:** o subject exige timestamps em milissegundos relativos ao início da simulação.
  `gettimeofday` é a única syscall portável (sem POSIX extensions) que oferece resolução de
  microssegundos na maioria dos sistemas. Calcular o delta em relação a `start_time` deixa os
  logs legíveis (começam em 0 ms) e facilita debugar timing de morte.

- [ ] **2.2 — Sistema de logging thread-safe (`print_status`)**

  Criar um mutex de print em `t_rule`. Implementar `print_status(t_philo *p, char *msg)` que:
  (1) trava o mutex de print, (2) checa a flag `stop` **dentro** do lock, (3) só imprime se a
  simulação ainda roda, (4) destrava o mutex.

  Mensagens obrigatórias pelo subject:
  ```
  timestamp_ms X has taken a fork
  timestamp_ms X is eating
  timestamp_ms X is sleeping
  timestamp_ms X is thinking
  timestamp_ms X died
  ```

  **Por quê:** sem o mutex de print, threads concorrentes escrevem no stdout ao mesmo tempo,
  gerando saída intercalada ilegível (e reprovação no avaliador). Checar `stop` dentro do lock
  evita que uma mensagem seja impressa após a morte já ter sido anunciada — o subject exige que
  nenhuma mensagem apareça depois do `died`.

  > **Armadilha 1 — o `died` é a exceção.** A própria mensagem `died` precisa sair **mesmo com
  > `stop` já marcado**. O monitor não a imprime via o gate acima: ele trava o mutex de print,
  > imprime `died` direto e marca `stop` no mesmo lock (ver Fase 4.1). Por isso o `print_status`
  > dos filósofos precisa checar `stop` *dentro* do lock de print — assim, depois do `died`,
  > nenhum filósofo consegue imprimir.
  >
  > **Armadilha 2 — sincronizar `stop`.** `stop` é lida/escrita por várias threads. Um `int`
  > acessado sem sincronização é data race e o Helgrind (5.4) vai acusar. Proteja `stop` com um
  > mutex dedicado (com getter/setter, ex.: `simulation_stopped(rules)`) ou use um tipo atômico,
  > e nunca a leia/escreva direto.

- [ ] **2.3 — `ft_usleep` com checagem de parada**

  Implementar um `usleep` próprio que dorme em fatias pequenas (~200 µs) e verifica a flag `stop`
  (via getter sincronizado) a cada iteração, retornando cedo se a simulação terminou.

  **Por quê:** o `usleep` padrão não pode ser interrompido. Se um filósofo está dormindo
  800 ms e a simulação encerrou em 100 ms, ele continuará ocupando CPU e postergando o join
  das threads. O `ft_usleep` garante que todas as threads acordem rapidamente após `stop = 1`,
  permitindo um encerramento limpo.

---

## Fase 3 — Rotina dos filósofos

- [ ] **3.1 — Pegar e largar garfos**

  Implementar `take_forks(t_philo *p)` e `put_forks(t_philo *p)`.

  Garfo esquerdo do filósofo `i`: `philos[i].left_fork`. Garfo direito: `philos[i].right_fork`
  (campo já populado por `link_forks` na Fase 1.3; equivale ao `left_fork` do vizinho,
  `philos[(i + 1) % n].left_fork`).

  Estratégia anti-deadlock: filósofos com `id` **par** pegam o garfo **direito** primeiro;
  filósofos com `id` **ímpar** pegam o **esquerdo** primeiro. (Lembre que `id` é 1-indexado —
  `init_philosophers` faz `id = i + 1`.) Isso quebra a simetria circular que causa deadlock.

  Caso especial para `n = 1`: só existe um garfo. Pegar o garfo esquerdo, printar "has taken a fork"
  e aguardar `time_to_die` ms. Nunca tentar um segundo garfo (ele não existe).

  **Por quê:** se todos os filósofos pegarem o garfo esquerdo ao mesmo tempo, nenhum consegue
  o direito — deadlock eterno. Inverter a ordem de um subconjunto quebra o ciclo de dependência.
  O caso `n = 1` é tratado à parte porque a lógica de dois garfos pressupõe dois filósofos
  distintos; com um único filósofo o vizinho é ele mesmo (`(0 + 1) % 1 == 0`), então o "garfo
  direito" é o mesmo mutex do esquerdo — travá-lo duas vezes na mesma thread é deadlock imediato.

- [ ] **3.2 — Fase de comer**

  Implementar `philo_eat(t_philo *p)`:
  1. `take_forks(p)`
  2. Travar o mutex de `last_meal` do filósofo, atualizar `p->last_meal = get_time_ms()`, destravar
  3. `print_status(p, "is eating")`
  4. `ft_usleep(p->rules->time_to_eat)`
  5. Incrementar `p->meals_eaten` (sob o mesmo mutex, pois o monitor o lê)
  6. `put_forks(p)`

  **Por quê:** `last_meal` precisa ser atualizado sob mutex porque o monitor lê esse valor
  concorrentemente. Atualizar antes do `ft_usleep` (e não depois) é crítico: se o filósofo
  demorar mais que `time_to_die` para comer (impossível pelo subject, mas defensivo), o
  monitor não declara morte prematura. `meals_eaten` é incrementado após o sleep para não
  contar uma refeição que foi interrompida por `stop` — e, como o monitor também o lê, deve ser
  protegido pelo mesmo mutex que cobre `last_meal`.

- [ ] **3.3 — Loop principal da thread**

  Implementar `void *philo_routine(void *arg)`:
  ```
  if (id % 2 == 0) ft_usleep(1);          // dessincroniza filósofos pares
  while (!simulation_stopped(rules))      // getter sincronizado, não leia stop direto
      philo_eat → philo_sleep → philo_think
  ```

  **Por quê:** o delay inicial de 1 ms nos filósofos pares evita que todos tentem pegar o
  garfo esquerdo no mesmo instante. Sem isso, mesmo com a estratégia de inversão, o burst
  inicial de criação de threads pode gerar contenção excessiva e timeouts espúrios nos primeiros
  ciclos. O loop checa `stop` antes de cada fase para encerrar o mais rápido possível
  após a parada da simulação.

---

## Fase 4 — Monitor de morte

- [ ] **4.1 — Thread supervisora**

  Implementar `void *monitor_routine(void *arg)` que, em loop, itera sobre todos os filósofos:
  - Lê `philo->last_meal` sob o mutex do filósofo.
  - Se `get_time_ms() - last_meal > time_to_die`: **imprime `"X died"` sob o mutex de print e
    marca `stop = 1` no mesmo lock**, então retorna.
  - Se `number_of_meals > 0` e todos têm `meals_eaten >= number_of_meals` (lido sob mutex):
    marca `stop = 1` e retorna.

  Dormir ~500–1000 µs entre checagens para não saturar CPU.

  **Por quê:** o monitor precisa ser uma thread separada porque os filósofos ficam bloqueados em
  mutexes e em `usleep` — eles não conseguem verificar a própria morte. Uma thread dedicada
  consegue checar todos os filósofos de forma independente. O sleep entre checagens é necessário
  para não transformar o monitor em busy-wait puro, que consumiria um core inteiro. O acesso a
  `last_meal` (e a `meals_eaten`) sob mutex evita data race com a thread do filósofo que escreve
  nos mesmos campos.

  > **Ordem do `died` (casa com a Armadilha 1 da 2.2):** imprima o `died` **antes** de liberar o
  > lock de print e **só então** (ou no mesmo lock) marque `stop`. Se marcar `stop` primeiro e
  > deixar o `print_status` gated cuidar da impressão, a mensagem obrigatória `died` é engolida.
  > Use `>` (e não `>=`) com cuidado: a diferença de 1 ms importa nos casos apertados (ver 5.3).

- [ ] **4.2 — Lançar threads e sincronizar**

  Sequência correta:
  1. Salvar `rules->start_time = get_time_ms()` e semear `last_meal = start_time` em todo filósofo
     (resolve o **B5** / completa a 1.2)
  2. `pthread_create` para cada filósofo
  3. `pthread_create` para o monitor
  4. `pthread_join` no monitor (bloqueia até a simulação acabar)
  5. `pthread_join` em todos os filósofos

  **Por quê:** `start_time` deve ser salvo imediatamente antes de criar as threads para que o
  timestamp 0 ms coincida com o início real da simulação — e é exatamente aqui que `last_meal`
  ganha seu valor inicial, pois antes disso `start_time` não existe. O monitor é criado por último
  para não começar a checar `last_meal` antes dos filósofos terem inicializado seus dados. O join
  do monitor primeiro é natural: ele é quem seta `stop = 1`, então aguardá-lo garante que a
  simulação realmente terminou antes de joinar os filósofos — que vão encerrar rapidamente
  graças ao `ft_usleep` reativo.

---

## Fase 5 — Limpeza & edge cases

- [ ] **5.1 — Cleanup completo** — *parcialmente em `philo/utils/free_rules.c`*

  Após todos os joins:
  - `pthread_mutex_destroy` no mutex de cada garfo (`philos[i].left_fork->mutex`) e `free` de cada `left_fork` — **nunca** o `right_fork` (é alias)
  - `pthread_mutex_destroy` no mutex de print
  - `pthread_mutex_destroy` no mutex de `stop`
  - `pthread_mutex_destroy` no mutex de `last_meal` de cada filósofo
  - `free(rules->philos)`
  - `free(rules)`

  **Por quê:** mutexes e memória alocada que não são explicitamente destruídos/liberados
  resultam em leaks detectados pelo Valgrind — reprovação automática na avaliação. Chamar
  `pthread_mutex_destroy` em um mutex ainda travado é UB; o cleanup só acontece após todos
  os joins, o que garante que nenhum mutex está em uso.

  > **Estado atual:** `destroy_forks`/`free_philosophers`/`free_rules` já liberam os garfos e o
  > `rules`, com o **B4 resolvido** — `destroy_forks` pula `left_fork` `NULL` e **só libera os
  > `left_fork`** (nunca `right_fork`, que é alias; liberá-lo seria double-free). Ainda falta
  > destruir os mutexes de print/`stop`/`last_meal` (que ainda não existem). Não há array central
  > `forks` — cada garfo é liberado individualmente.

- [ ] **5.2 — Tratamento do caso `n = 1`**

  Garantir que a rotina do filósofo único pega apenas um garfo, printa `"has taken a fork"`,
  aguarda `time_to_die` ms via `ft_usleep` e então o monitor declara a morte.

  **Por quê:** com um único filósofo, o garfo esquerdo e o direito seriam o mesmo mutex
  (`(0 + 1) % 1 == 0`). Tentar travá-lo duas vezes na mesma thread causa deadlock imediato. O
  subject exige que esse caso seja tratado corretamente: o filósofo morre de fome por nunca
  conseguir o segundo garfo (que não existe).

  > **Bloqueado pelo B1:** hoje o validador rejeita `n = 1` antes mesmo de chegar à rotina.
  > Corrija o **B1** primeiro, senão este caso é impossível de testar.

- [ ] **5.3 — Testes de referência**

  Executar os casos abaixo e verificar os resultados. (O subject traz apenas alguns exemplos
  soltos; esta tabela é um conjunto de referência.)

  | Comando | Resultado esperado |
  |---|---|
  | `./philo 1 800 200 200` | Morre em ~800 ms — *bloqueado pelo B1 até corrigir* |
  | `./philo 5 800 200 200` | Não morre nunca |
  | `./philo 5 800 200 200 7` | Para após 7 refeições cada |
  | `./philo 4 410 200 200` | Não morre nunca |
  | `./philo 4 310 200 100` | **Limite — verificar empiricamente:** `eat+sleep = 200+100 = 300 < 310`, então uma implementação eficiente pode manter todos vivos; o resultado depende do escalonamento |
  | `./philo 2 800 200 200` | Não morre nunca |

  **Por quê:** esses casos cobrem os cenários críticos: morte garantida, sobrevivência com timing
  justo, condição de parada por refeições, e os limites de timing onde um erro de 1–2 ms na
  implementação do sleep (ou um `>=` no lugar de `>`) já vira a diferença entre passar e falhar.

- [ ] **5.4 — Detecção de data races com Helgrind**

  Rodar `valgrind --tool=helgrind ./philo 4 410 200 200` e garantir que não há reportes de
  race condition. Para leaks: `valgrind --leak-check=full ./philo 5 800 200 200 3`.

  **Por quê:** o Helgrind detecta acessos concorrentes sem sincronização que o compilador e
  o AddressSanitizer não pegam. Data races têm comportamento não-determinístico — podem não
  manifestar falha em 99 de 100 execuções mas travar na avaliação. Qualquer reporte do Helgrind
  indica um mutex faltando ou mal posicionado.

  > **Atenção:** a flag `stop` (e qualquer estado compartilhado: `last_meal`, `meals_eaten`) tem
  > de estar sob mutex/atômico — um `int` lido/escrito por várias threads sem sincronização é
  > justamente o que o Helgrind acusa. Ver Armadilha 2 da Fase 2.2.

- [ ] **5.5 — Revisão Norminette**

  Rodar `norminette` em todos os arquivos `.c` e `.h` e corrigir todas as infrações.

  **Por quê:** reprovação automática na avaliação da 42 se houver erros de Norma — independente
  do funcionamento do programa.

  > **Já visível em `philo/philo.h`:** `} t_philo;` (linha 27) precisa de tab antes do nome do
  > tipo, e `typedef struct  s_rule` (linha 29) tem espaço duplo. A norminette pega ambos.

---

## Resumo das dependências entre fases

```
Bugs B1–B5 (código atual) ──► destravam testes das fases finais

Fase 0 (structs) ──► Fase 1 (parsing + init) ──► Fase 2 (tempo + log)
                                                         │
                                              ┌──────────┘
                                              ▼
                                      Fase 3 (rotina)
                                              │
                                              ▼
                                      Fase 4 (monitor) ──┐
                                              │           │ semeia last_meal (completa a 1.2)
                                              ▼           ▼
                                      Fase 5 (cleanup + testes)
```

Cada fase depende da anterior. Não pule para a rotina dos filósofos sem ter o logging pronto —
sem output visível, debugar timing de morte é praticamente impossível.

**Exceção ao fluxo linear:** a inicialização real de `last_meal` (1.2) só acontece no lançamento
das threads (4.2), porque depende de `start_time`. E os Bugs conhecidos **B1–B5** bloqueiam os
testes das fases finais — corrija cada um assim que tocar no código correspondente.
