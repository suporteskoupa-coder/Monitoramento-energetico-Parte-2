# Diário de Desenvolvimento

## Registros recuperados do repositório

Esta linha do tempo contém somente eventos sustentados por commits, dados ou relatórios. Ela
não substitui atas que não tenham sido registradas na época.

| Data | Evento verificável | Evidência | Situação resultante |
|---|---|---|---|
| 15/07/2026 | Criação inicial do repositório | primeiro commit do Git | início do projeto |
| 04–05/08/2026 | Execução da Campanha 01 | base bruta e relatório corrigido | campanha diagnóstica com quatro interrupções |
| 15/08/2026 | Feedback do orientador sobre regularização | relatório de feedback fornecido à equipe | priorização de fluxo único, documentação e teste-piloto |
| 18/08/2026 | Reorganização de firmware e dados | histórico recente do Git | candidato a firmware em `firmware/src/` e bases em `dados/` |
| 18/08/2026 | Consolidação da documentação científica | arquivos em `docs/` e README | limitações, método e decisões passam a estar explícitos |

## Pendências que devem aparecer nos próximos registros

- decisão do fluxo oficial de aquisição;
- confirmação do hardware real;
- decisão da regra de consolidação;
- execução do teste-piloto;
- disponibilização do alicate amperímetro;
- calibração e validação;
- campanha definitiva.

## Modelo para novos registros

Adicionar uma entrada por sessão relevante de trabalho:

```text
Data e horário:
Responsável(is):
Objetivo:
Atividade realizada:
Arquivos ou equipamentos envolvidos:
Resultado observável:
Dificuldades:
Decisões tomadas ou decisão pendente:
Próximo passo e responsável:
Commit ou evidência:
```

## Regras

1. descrever resultado observável, não apenas “atividade concluída”;
2. vincular código, dado, foto, relatório ou commit;
3. não registrar como validado algo que apenas foi criado;
4. separar fato, hipótese e interpretação;
5. manter decisões compartilhadas também em [`decisoes.md`](decisoes.md).
