# TCP Calculator

## 📖 Descrição
Aplicação cliente-servidor TCP em C.  
O cliente envia operações matemáticas e o servidor executa, retornando o resultado.  
Suporta dois formatos:
- **Obrigatório**: `OP NUM NUM` (ex.: `ADD 2 3`)
- **Bônus**: `NUM <op> NUM` (ex.: `2 + 3`)

### Operações suportadas
- `ADD` ou `+` → soma
- `SUB` ou `-` → subtração
- `MUL` ou `*` → multiplicação
- `DIV` ou `/` → divisão

---

## ⚙️ Como compilar
No terminal, dentro da pasta do projeto:

```bash
make
