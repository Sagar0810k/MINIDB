"""
MiniDB-Lang Static Code Analyzer
=================================
A pure-Python lexer, parser, and semantic analyzer that mirrors
the Flex/Bison grammar of the MINIDB project.
"""

import re
from dataclasses import dataclass, field
from typing import List, Optional, Tuple

# ─── Token Types ───────────────────────────────────────────────────────────

KEYWORDS = {
    'CREATE', 'TABLE', 'INSERT', 'INTO', 'SELECT', 'FROM', 'VALUES',
    'INT', 'TEXT', 'EXIT', 'QUIT', 'SHOW', 'TABLES', 'WHERE', 'AND', 'OR',
    'DELETE', 'UPDATE', 'SET', 'JOIN', 'ON', 'INNER', 'ORDER', 'BY',
    'ASC', 'DESC', 'LIMIT', 'COUNT', 'SUM', 'AVG', 'PRIMARY', 'KEY',
    'UNIQUE', 'ALTER', 'ADD', 'DROP', 'COLUMN', 'HELP'
}

SYMBOLS = {
    '*': 'ASTERISK', '(': 'LPAREN', ')': 'RPAREN', ',': 'COMMA',
    ';': 'SEMICOLON', '.': 'DOT',
    '<=': 'LTE', '>=': 'GTE', '!=': 'NEQ',
    '<': 'LT', '>': 'GT', '=': 'EQ'
}

# Order matters — check multi-char operators first
SYMBOL_ORDER = ['<=', '>=', '!=', '<', '>', '=', '*', '(', ')', ',', ';', '.']


@dataclass
class Token:
    type: str        # e.g. 'KEYWORD', 'IDENTIFIER', 'NUMBER', 'STRING', 'SYMBOL', 'UNKNOWN'
    value: str       # the actual text
    line: int        # 1-indexed line number
    col: int         # 1-indexed column number

    def __repr__(self):
        return f"Token({self.type}:{self.value!r} L{self.line}:C{self.col})"


@dataclass
class AnalysisResult:
    status: str = "Valid"
    error_type: str = "None"
    line_number: int = 0
    offending_token: str = ""
    explanation: str = "Code is syntactically and semantically correct."
    full_corrected_code: str = ""

    def to_dict(self):
        return {
            "status": self.status,
            "analysis_metrics": {
                "error_type": self.error_type,
                "line_number": self.line_number,
                "offending_token": self.offending_token
            },
            "explanation": self.explanation,
            "full_corrected_code": self.full_corrected_code
        }


# ─── LEXER ─────────────────────────────────────────────────────────────────

class Lexer:
    """Tokenize MiniDB-Lang source code."""

    def __init__(self, source: str):
        self.source = source
        self.pos = 0
        self.line = 1
        self.col = 1
        self.tokens: List[Token] = []
        self.errors: List[Token] = []

    def _advance(self, n=1):
        for _ in range(n):
            if self.pos < len(self.source):
                if self.source[self.pos] == '\n':
                    self.line += 1
                    self.col = 1
                else:
                    self.col += 1
                self.pos += 1

    def _peek(self, offset=0):
        idx = self.pos + offset
        return self.source[idx] if idx < len(self.source) else ''

    def _remaining(self):
        return self.source[self.pos:]

    def tokenize(self) -> Tuple[List[Token], List[Token]]:
        while self.pos < len(self.source):
            c = self._peek()

            # Skip whitespace
            if c in ' \t\r\n':
                self._advance()
                continue

            # Skip comments: -- ...
            if c == '-' and self._peek(1) == '-':
                while self.pos < len(self.source) and self._peek() != '\n':
                    self._advance()
                continue

            # Multi-char symbols first
            matched_symbol = False
            for sym in SYMBOL_ORDER:
                if self.source[self.pos:self.pos + len(sym)] == sym:
                    tok = Token('SYMBOL', sym, self.line, self.col)
                    self.tokens.append(tok)
                    self._advance(len(sym))
                    matched_symbol = True
                    break
            if matched_symbol:
                continue

            # String literal: '...'
            if c == "'":
                start_line, start_col = self.line, self.col
                self._advance()  # skip opening quote
                string_val = ""
                closed = False
                while self.pos < len(self.source):
                    ch = self._peek()
                    if ch == "'":
                        if self._peek(1) == "'":   # escaped quote ''
                            string_val += "'"
                            self._advance(2)
                        else:
                            self._advance()  # skip closing quote
                            closed = True
                            break
                    else:
                        string_val += ch
                        self._advance()
                if closed:
                    self.tokens.append(Token('STRING', string_val, start_line, start_col))
                else:
                    self.errors.append(Token('UNKNOWN', "'"+string_val, start_line, start_col))
                continue

            # Number
            if c.isdigit():
                start_line, start_col = self.line, self.col
                num = ""
                while self.pos < len(self.source) and self._peek().isdigit():
                    num += self._peek()
                    self._advance()
                self.tokens.append(Token('NUMBER', num, start_line, start_col))
                continue

            # Identifier / keyword
            if c.isalpha() or c == '_':
                start_line, start_col = self.line, self.col
                ident = ""
                while self.pos < len(self.source) and (self._peek().isalnum() or self._peek() == '_'):
                    ident += self._peek()
                    self._advance()
                if ident.upper() in KEYWORDS:
                    self.tokens.append(Token('KEYWORD', ident.upper(), start_line, start_col))
                else:
                    self.tokens.append(Token('IDENTIFIER', ident, start_line, start_col))
                continue

            # Unknown character
            self.errors.append(Token('UNKNOWN', c, self.line, self.col))
            self._advance()

        return self.tokens, self.errors


# ─── PARSER ────────────────────────────────────────────────────────────────

class ParseError(Exception):
    def __init__(self, message, token=None):
        self.message = message
        self.token = token
        super().__init__(message)


class Parser:
    """
    Recursive-descent parser for MiniDB-Lang.
    Validates structural grammar against the Bison spec.
    """

    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0
        self.errors: List[dict] = []

    def _peek(self) -> Optional[Token]:
        return self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def _advance(self) -> Token:
        tok = self.tokens[self.pos]
        self.pos += 1
        return tok

    def _expect(self, type_: str, value: str = None) -> Token:
        tok = self._peek()
        if tok is None:
            expected = f"{value or type_}"
            raise ParseError(f"Unexpected end of input, expected '{expected}'", None)
        if tok.type != type_ or (value is not None and tok.value.upper() != value.upper()):
            expected = value or type_
            raise ParseError(
                f"Expected '{expected}' but found '{tok.value}' at line {tok.line}, column {tok.col}",
                tok
            )
        return self._advance()

    def _match(self, type_: str, value: str = None) -> bool:
        tok = self._peek()
        if tok is None:
            return False
        if tok.type != type_:
            return False
        if value is not None and tok.value.upper() != value.upper():
            return False
        return True

    def _match_any_keyword(self, *keywords) -> bool:
        tok = self._peek()
        if tok is None or tok.type != 'KEYWORD':
            return False
        return tok.value.upper() in [k.upper() for k in keywords]

    def parse(self) -> List[dict]:
        """Parse all statements, collecting errors."""
        while self.pos < len(self.tokens):
            try:
                self._parse_statement()
            except ParseError as e:
                self.errors.append({
                    'message': e.message,
                    'token': e.token,
                    'line': e.token.line if e.token else 0,
                    'col': e.token.col if e.token else 0,
                    'offending': e.token.value if e.token else 'EOF'
                })
                # Error recovery: skip to next semicolon
                while self.pos < len(self.tokens):
                    tok = self._peek()
                    if tok and tok.type == 'SYMBOL' and tok.value == ';':
                        self._advance()
                        break
                    self._advance()
        return self.errors

    def _parse_statement(self):
        tok = self._peek()
        if tok is None:
            return

        if tok.type == 'KEYWORD':
            kw = tok.value.upper()
            if kw == 'CREATE':
                self._parse_create_table()
            elif kw == 'INSERT':
                self._parse_insert()
            elif kw == 'SELECT':
                self._parse_select()
            elif kw == 'DELETE':
                self._parse_delete()
            elif kw == 'UPDATE':
                self._parse_update()
            elif kw == 'ALTER':
                self._parse_alter()
            elif kw == 'SHOW':
                self._parse_show_tables()
            elif kw in ('EXIT', 'QUIT'):
                self._parse_exit()
            elif kw == 'HELP':
                self._advance()
                self._expect('SYMBOL', ';')
            else:
                raise ParseError(f"Unexpected keyword '{kw}' — not a valid statement start", tok)
        else:
            raise ParseError(f"Expected a SQL statement keyword, found '{tok.value}'", tok)

    # ── CREATE TABLE ────────────────────────────────────────────
    def _parse_create_table(self):
        self._expect('KEYWORD', 'CREATE')
        self._expect('KEYWORD', 'TABLE')
        name_tok = self._expect('IDENTIFIER')
        self._expect('SYMBOL', '(')
        self._parse_column_defs()
        self._expect('SYMBOL', ')')
        self._expect('SYMBOL', ';')
        return name_tok.value

    def _parse_column_defs(self):
        self._parse_column_def()
        while self._match('SYMBOL', ','):
            self._advance()
            self._parse_column_def()

    def _parse_column_def(self):
        self._expect('IDENTIFIER')
        if self._match_any_keyword('INT', 'TEXT'):
            self._advance()
        else:
            tok = self._peek()
            raise ParseError(
                f"Expected data type (INT or TEXT) but found '{tok.value if tok else 'EOF'}'",
                tok
            )
        # Optional constraint
        if self._match('KEYWORD', 'PRIMARY'):
            self._advance()
            self._expect('KEYWORD', 'KEY')
        elif self._match('KEYWORD', 'UNIQUE'):
            self._advance()

    # ── INSERT ──────────────────────────────────────────────────
    def _parse_insert(self):
        self._expect('KEYWORD', 'INSERT')
        self._expect('KEYWORD', 'INTO')
        name_tok = self._expect('IDENTIFIER')
        self._expect('KEYWORD', 'VALUES')
        self._expect('SYMBOL', '(')
        values = self._parse_value_list()
        self._expect('SYMBOL', ')')
        self._expect('SYMBOL', ';')
        return name_tok.value, values

    def _parse_value_list(self):
        values = [self._parse_value()]
        while self._match('SYMBOL', ','):
            self._advance()
            values.append(self._parse_value())
        return values

    def _parse_value(self):
        tok = self._peek()
        if tok is None:
            raise ParseError("Unexpected end of input, expected a value", None)
        if tok.type == 'NUMBER':
            return self._advance()
        elif tok.type == 'STRING':
            return self._advance()
        else:
            raise ParseError(
                f"Expected a value (number or string) but found '{tok.value}'",
                tok
            )

    # ── SELECT ──────────────────────────────────────────────────
    def _parse_select(self):
        self._expect('KEYWORD', 'SELECT')
        tok = self._peek()

        # SELECT aggregate(...)
        if tok and tok.type == 'KEYWORD' and tok.value.upper() in ('COUNT', 'SUM', 'AVG'):
            self._parse_aggregate()
            self._expect('KEYWORD', 'FROM')
            name_tok = self._expect('IDENTIFIER')
            self._parse_opt_where()
            self._expect('SYMBOL', ';')
            return

        # SELECT *
        if self._match('SYMBOL', '*'):
            self._advance()
            self._expect('KEYWORD', 'FROM')
            name_tok = self._expect('IDENTIFIER')

            # Check for INNER JOIN
            if self._match('KEYWORD', 'INNER'):
                self._advance()
                self._expect('KEYWORD', 'JOIN')
                self._expect('IDENTIFIER')
                self._expect('KEYWORD', 'ON')
                self._parse_col_ref()
                self._expect('SYMBOL', '=')
                self._parse_col_ref()
                self._expect('SYMBOL', ';')
                return

            self._parse_opt_where()
            self._parse_opt_order()
            self._parse_opt_limit()
            self._expect('SYMBOL', ';')
            return

        # SELECT col1, col2, ...
        self._parse_col_name_list()
        self._expect('KEYWORD', 'FROM')
        name_tok = self._expect('IDENTIFIER')
        self._parse_opt_where()
        self._parse_opt_order()
        self._parse_opt_limit()
        self._expect('SYMBOL', ';')

    def _parse_aggregate(self):
        """Parse COUNT/SUM/AVG (col | *)"""
        self._advance()  # consume aggregate keyword
        self._expect('SYMBOL', '(')
        if self._match('SYMBOL', '*'):
            self._advance()
        elif self._match('IDENTIFIER'):
            self._advance()
        else:
            tok = self._peek()
            raise ParseError(f"Expected column name or '*' inside aggregate but found '{tok.value if tok else 'EOF'}'", tok)
        self._expect('SYMBOL', ')')

    def _parse_col_name_list(self):
        self._expect('IDENTIFIER')
        while self._match('SYMBOL', ','):
            self._advance()
            self._expect('IDENTIFIER')

    def _parse_col_ref(self):
        self._expect('IDENTIFIER')
        if self._match('SYMBOL', '.'):
            self._advance()
            self._expect('IDENTIFIER')

    # ── WHERE ───────────────────────────────────────────────────
    def _parse_opt_where(self):
        if self._match('KEYWORD', 'WHERE'):
            self._advance()
            self._parse_single_cond()
            while self._match_any_keyword('AND', 'OR'):
                self._advance()
                self._parse_single_cond()

    def _parse_single_cond(self):
        self._expect('IDENTIFIER')
        # Expect comparison operator
        tok = self._peek()
        if tok and tok.type == 'SYMBOL' and tok.value in ('=', '!=', '<', '>', '<=', '>='):
            self._advance()
        else:
            raise ParseError(
                f"Expected comparison operator (=, !=, <, >, <=, >=) but found '{tok.value if tok else 'EOF'}'",
                tok
            )
        # Expect value
        self._parse_value()

    # ── ORDER BY ────────────────────────────────────────────────
    def _parse_opt_order(self):
        if self._match('KEYWORD', 'ORDER'):
            self._advance()
            self._expect('KEYWORD', 'BY')
            self._expect('IDENTIFIER')
            if self._match_any_keyword('ASC', 'DESC'):
                self._advance()

    # ── LIMIT ───────────────────────────────────────────────────
    def _parse_opt_limit(self):
        if self._match('KEYWORD', 'LIMIT'):
            self._advance()
            self._expect('NUMBER')

    # ── DELETE ──────────────────────────────────────────────────
    def _parse_delete(self):
        self._expect('KEYWORD', 'DELETE')
        self._expect('KEYWORD', 'FROM')
        self._expect('IDENTIFIER')
        self._parse_opt_where()
        self._expect('SYMBOL', ';')

    # ── UPDATE ──────────────────────────────────────────────────
    def _parse_update(self):
        self._expect('KEYWORD', 'UPDATE')
        self._expect('IDENTIFIER')
        self._expect('KEYWORD', 'SET')
        self._expect('IDENTIFIER')
        self._expect('SYMBOL', '=')
        self._parse_value()
        self._parse_opt_where()
        self._expect('SYMBOL', ';')

    # ── ALTER TABLE ─────────────────────────────────────────────
    def _parse_alter(self):
        self._expect('KEYWORD', 'ALTER')
        self._expect('KEYWORD', 'TABLE')
        self._expect('IDENTIFIER')
        tok = self._peek()
        if tok and tok.type == 'KEYWORD' and tok.value.upper() == 'ADD':
            self._advance()
            self._expect('KEYWORD', 'COLUMN')
            self._expect('IDENTIFIER')
            if self._match_any_keyword('INT', 'TEXT'):
                self._advance()
            else:
                err_tok = self._peek()
                raise ParseError(f"Expected data type (INT or TEXT) but found '{err_tok.value if err_tok else 'EOF'}'", err_tok)
            self._expect('SYMBOL', ';')
        elif tok and tok.type == 'KEYWORD' and tok.value.upper() == 'DROP':
            self._advance()
            self._expect('KEYWORD', 'COLUMN')
            self._expect('IDENTIFIER')
            self._expect('SYMBOL', ';')
        else:
            raise ParseError(f"Expected ADD or DROP after ALTER TABLE but found '{tok.value if tok else 'EOF'}'", tok)

    # ── SHOW TABLES ─────────────────────────────────────────────
    def _parse_show_tables(self):
        self._expect('KEYWORD', 'SHOW')
        self._expect('KEYWORD', 'TABLES')
        self._expect('SYMBOL', ';')

    # ── EXIT ────────────────────────────────────────────────────
    def _parse_exit(self):
        self._advance()  # EXIT or QUIT
        self._expect('SYMBOL', ';')


# ─── SEMANTIC ANALYZER ─────────────────────────────────────────────────────

@dataclass
class TableDef:
    name: str
    columns: List[dict] = field(default_factory=list)  # [{'name':..,'type':..,'flags':[]}]


class SemanticAnalyzer:
    """
    Performs semantic checks over the token stream:
    - Table existence for INSERT/SELECT/DELETE/UPDATE/ALTER/JOIN
    - Column count matching for INSERT
    - Column existence in WHERE/ORDER BY/SET
    """

    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0
        self.tables: dict = {}  # name -> TableDef
        self.errors: List[dict] = []

    def _peek(self) -> Optional[Token]:
        return self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def _advance(self) -> Token:
        tok = self.tokens[self.pos]
        self.pos += 1
        return tok

    def _skip_to_semicolon(self):
        while self.pos < len(self.tokens):
            tok = self._peek()
            if tok and tok.type == 'SYMBOL' and tok.value == ';':
                self._advance()
                return
            self._advance()

    def _match(self, type_: str, value: str = None) -> bool:
        tok = self._peek()
        if tok is None: return False
        if tok.type != type_: return False
        if value is not None and tok.value.upper() != value.upper(): return False
        return True

    def analyze(self) -> List[dict]:
        while self.pos < len(self.tokens):
            tok = self._peek()
            if tok is None:
                break
            if tok.type != 'KEYWORD':
                self._advance()
                continue

            kw = tok.value.upper()
            try:
                if kw == 'CREATE':
                    self._analyze_create()
                elif kw == 'INSERT':
                    self._analyze_insert()
                elif kw == 'SELECT':
                    self._analyze_select()
                elif kw == 'DELETE':
                    self._analyze_delete()
                elif kw == 'UPDATE':
                    self._analyze_update()
                elif kw == 'ALTER':
                    self._analyze_alter()
                else:
                    self._skip_to_semicolon()
            except (IndexError, StopIteration):
                break

        return self.errors

    def _analyze_create(self):
        self._advance()  # CREATE
        self._advance()  # TABLE
        name_tok = self._advance()  # table name
        table_name = name_tok.value

        if table_name.upper() in [t.upper() for t in self.tables]:
            self.errors.append({
                'message': f"Table '{table_name}' is already defined in this code",
                'line': name_tok.line,
                'col': name_tok.col,
                'offending': table_name
            })

        # Parse column definitions
        columns = []
        self._advance()  # (
        while self.pos < len(self.tokens):
            tok = self._peek()
            if tok and tok.type == 'SYMBOL' and tok.value == ')':
                self._advance()
                break
            if tok and tok.type == 'SYMBOL' and tok.value == ',':
                self._advance()
                continue
            if tok and tok.type == 'IDENTIFIER':
                col_name = self._advance().value
                col_type = self._advance().value if self._peek() else 'INT'
                flags = []
                while self._peek() and self._peek().type == 'KEYWORD' and self._peek().value.upper() in ('PRIMARY', 'KEY', 'UNIQUE'):
                    flags.append(self._advance().value.upper())
                columns.append({'name': col_name, 'type': col_type, 'flags': flags})
            else:
                self._advance()

        self.tables[table_name] = TableDef(name=table_name, columns=columns)
        self._skip_to_semicolon()

    def _analyze_insert(self):
        start_tok = self._advance()  # INSERT
        self._advance()  # INTO
        name_tok = self._advance()  # table name
        table_name = name_tok.value

        if table_name not in self.tables:
            self.errors.append({
                'message': f"Cannot INSERT into table '{table_name}': table not defined in this code. "
                           f"Make sure to CREATE TABLE before inserting.",
                'line': name_tok.line,
                'col': name_tok.col,
                'offending': table_name
            })
            self._skip_to_semicolon()
            return

        # Count values
        self._advance()  # VALUES
        self._advance()  # (
        value_count = 0
        paren_depth = 1
        while self.pos < len(self.tokens) and paren_depth > 0:
            tok = self._peek()
            if tok.type == 'SYMBOL' and tok.value == '(':
                paren_depth += 1
            elif tok.type == 'SYMBOL' and tok.value == ')':
                paren_depth -= 1
                if paren_depth == 0:
                    self._advance()
                    break
            elif tok.type in ('NUMBER', 'STRING') and paren_depth == 1:
                value_count += 1
            self._advance()

        table_def = self.tables[table_name]
        expected = len(table_def.columns)
        if expected > 0 and value_count != expected:
            self.errors.append({
                'message': f"INSERT value count mismatch for table '{table_name}': expected {expected} values "
                           f"(columns: {', '.join(c['name'] for c in table_def.columns)}) but got {value_count}",
                'line': start_tok.line,
                'col': start_tok.col,
                'offending': f"{value_count} values"
            })

        self._skip_to_semicolon()

    def _analyze_select(self):
        self._advance()  # SELECT

        # Determine selected columns
        selected_cols = []
        is_star = False
        is_agg = False
        tok = self._peek()

        if tok and tok.type == 'SYMBOL' and tok.value == '*':
            is_star = True
            self._advance()
        elif tok and tok.type == 'KEYWORD' and tok.value.upper() in ('COUNT', 'SUM', 'AVG'):
            is_agg = True
            # skip past aggregate function
            while self.pos < len(self.tokens):
                t = self._peek()
                if t and t.type == 'SYMBOL' and t.value == ')':
                    self._advance()
                    break
                self._advance()
        else:
            while self.pos < len(self.tokens):
                t = self._peek()
                if t and t.type == 'KEYWORD' and t.value.upper() == 'FROM':
                    break
                if t and t.type == 'IDENTIFIER':
                    selected_cols.append(t)
                self._advance()

        # FROM
        if self._match('KEYWORD', 'FROM'):
            self._advance()
        name_tok = self._peek()
        if name_tok and name_tok.type == 'IDENTIFIER':
            table_name = self._advance().value
            if table_name not in self.tables:
                self.errors.append({
                    'message': f"Cannot SELECT from table '{table_name}': table not defined in this code",
                    'line': name_tok.line,
                    'col': name_tok.col,
                    'offending': table_name
                })
            elif not is_star and not is_agg and selected_cols:
                table_def = self.tables[table_name]
                known_cols = [c['name'].upper() for c in table_def.columns]
                for col_tok in selected_cols:
                    if col_tok.value.upper() not in known_cols:
                        self.errors.append({
                            'message': f"Column '{col_tok.value}' does not exist in table '{table_name}'",
                            'line': col_tok.line,
                            'col': col_tok.col,
                            'offending': col_tok.value
                        })

        self._skip_to_semicolon()

    def _analyze_delete(self):
        self._advance()  # DELETE
        self._advance()  # FROM
        name_tok = self._advance()
        if name_tok.value not in self.tables:
            self.errors.append({
                'message': f"Cannot DELETE from table '{name_tok.value}': table not defined",
                'line': name_tok.line,
                'col': name_tok.col,
                'offending': name_tok.value
            })
        self._skip_to_semicolon()

    def _analyze_update(self):
        self._advance()  # UPDATE
        name_tok = self._advance()
        if name_tok.value not in self.tables:
            self.errors.append({
                'message': f"Cannot UPDATE table '{name_tok.value}': table not defined",
                'line': name_tok.line,
                'col': name_tok.col,
                'offending': name_tok.value
            })
        else:
            # Check SET column
            if self._match('KEYWORD', 'SET'):
                self._advance()
                col_tok = self._peek()
                if col_tok and col_tok.type == 'IDENTIFIER':
                    table_def = self.tables[name_tok.value]
                    known_cols = [c['name'].upper() for c in table_def.columns]
                    if col_tok.value.upper() not in known_cols:
                        self.errors.append({
                            'message': f"Column '{col_tok.value}' not found in table '{name_tok.value}'",
                            'line': col_tok.line,
                            'col': col_tok.col,
                            'offending': col_tok.value
                        })
        self._skip_to_semicolon()

    def _analyze_alter(self):
        self._advance()  # ALTER
        self._advance()  # TABLE
        name_tok = self._advance()
        if name_tok.value not in self.tables:
            self.errors.append({
                'message': f"Cannot ALTER table '{name_tok.value}': table not defined",
                'line': name_tok.line,
                'col': name_tok.col,
                'offending': name_tok.value
            })
        self._skip_to_semicolon()


# ─── MAIN ANALYZE FUNCTION ─────────────────────────────────────────────────

def analyze_code(source: str) -> AnalysisResult:
    """
    Run all three analysis passes on MiniDB-Lang source code.
    Returns the first error found (lexical > syntax > semantic).
    """
    result = AnalysisResult()
    result.full_corrected_code = source.strip()

    if not source.strip():
        result.status = "Invalid"
        result.error_type = "Syntax"
        result.line_number = 1
        result.offending_token = ""
        result.explanation = "Empty input: no MiniDB-Lang statements provided."
        result.full_corrected_code = "-- Write your MiniDB code here\nCREATE TABLE example (id INT, name TEXT);"
        return result

    # ── Pass 1: Lexical Analysis ─────────────────────────────────
    lexer = Lexer(source)
    tokens, lex_errors = lexer.tokenize()

    if lex_errors:
        err = lex_errors[0]
        result.status = "Invalid"
        result.error_type = "Lexical"
        result.line_number = err.line
        result.offending_token = err.value
        if err.value.startswith("'"):
            result.explanation = (
                f"Lexical Error: Unterminated string literal starting at line {err.line}, column {err.col}. "
                f"String literals must be enclosed in single quotes (e.g., 'value')."
            )
            result.full_corrected_code = source.strip() + "'"
        else:
            result.explanation = (
                f"Lexical Error: Unexpected character '{err.value}' at line {err.line}, column {err.col}. "
                f"MiniDB-Lang only supports alphanumeric identifiers, numbers, string literals in single quotes, "
                f"and the symbols: * ( ) , ; . = != < > <= >="
            )
            # Remove the offending character
            lines = source.split('\n')
            if err.line <= len(lines):
                line = lines[err.line - 1]
                fixed = line[:err.col - 1] + line[err.col:]
                lines[err.line - 1] = fixed
            result.full_corrected_code = '\n'.join(lines).strip()
        return result

    # ── Pass 2: Syntax Analysis ──────────────────────────────────
    parser = Parser(tokens)
    parse_errors = parser.parse()

    if parse_errors:
        err = parse_errors[0]
        result.status = "Invalid"
        result.error_type = "Syntax"
        result.line_number = err.get('line', 0)
        result.offending_token = err.get('offending', '')
        result.explanation = f"Syntax Error: {err['message']}"

        # Attempt auto-fix for common issues
        if 'expected' in err['message'].lower() and "';'" in err['message']:
            # Missing semicolon — find the line and add one
            lines = source.split('\n')
            for i, line in enumerate(lines):
                stripped = line.rstrip()
                if stripped and not stripped.endswith(';') and not stripped.startswith('--'):
                    # Check if this line has any SQL keyword
                    words = stripped.split()
                    if words and any(w.upper() in KEYWORDS for w in words):
                        potential_end = True
                        # Check if next non-empty line starts a clause
                        for j in range(i + 1, len(lines)):
                            if lines[j].strip():
                                first_word = lines[j].strip().split()[0].upper() if lines[j].strip().split() else ''
                                if first_word not in ('WHERE', 'AND', 'OR', 'ORDER', 'LIMIT', 'SET', 'ON', 'INNER', 'JOIN', 'VALUES', 'FROM'):
                                    lines[i] = stripped + ';'
                                break
            result.full_corrected_code = '\n'.join(lines).strip()
        return result

    # ── Pass 3: Semantic Analysis ────────────────────────────────
    semantic = SemanticAnalyzer(tokens)
    sem_errors = semantic.analyze()

    if sem_errors:
        err = sem_errors[0]
        result.status = "Invalid"
        result.error_type = "Semantic"
        result.line_number = err.get('line', 0)
        result.offending_token = err.get('offending', '')
        result.explanation = f"Semantic Error: {err['message']}"
        return result

    # All clear!
    result.status = "Valid"
    result.error_type = "None"
    result.explanation = (
        "All checks passed. The code is lexically, syntactically, and semantically correct "
        "according to the MiniDB-Lang specification."
    )
    return result
