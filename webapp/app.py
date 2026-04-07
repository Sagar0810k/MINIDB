"""
MiniDB Studio — Flask Backend
==============================
REST API backend for the MiniDB web IDE + AI chatbot.
Uses the official OpenAI Python package to connect to an OpenAI-compatible API (OpenRouter in this case).
"""

from flask import Flask, render_template, request, jsonify
import subprocess
import re
import os
import json
from pathlib import Path
from openai import OpenAI

from analyzer import analyze_code, KEYWORDS

app = Flask(__name__)
app.secret_key = os.urandom(24)

MINIDB_ROOT = Path(__file__).resolve().parent.parent
MINIDB_EXE = MINIDB_ROOT / "build" / "minidb.exe"

# ─── OpenAI Compatible Config (Connecting to OpenRouter) ──────────────────────
API_KEY = "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"
BASE_URL = "https://openrouter.ai/api/v1"
MODEL_ID = "openrouter/free"

client = OpenAI(
    base_url=BASE_URL,
    api_key=API_KEY
)

MINIDB_SYSTEM_PROMPT = """You are **MiniDB Assistant**, an expert AI helping users write, debug, and understand MiniDB-Lang — a custom SQL-like language for the MiniDB database management system. You also answer any general/casual question the user asks.

## MiniDB-Lang Grammar Reference

### Supported Statements:
1. **CREATE TABLE** name (col1 TYPE [CONSTRAINT], ...);
   - Types: INT, TEXT
   - Constraints: PRIMARY KEY, UNIQUE
   
2. **INSERT INTO** name VALUES (val1, val2, ...);
   - Numbers: 42
   - Strings: 'hello'
   
3. **SELECT** * | col1, col2 | COUNT(*) | SUM(col) | AVG(col)
   **FROM** name
   [**WHERE** col op value [**AND**|**OR** col op value ...]]
   [**ORDER BY** col [**ASC**|**DESC**]]
   [**LIMIT** n];
   - Operators: =, !=, <, >, <=, >=
   
4. **SELECT * FROM** t1 **INNER JOIN** t2 **ON** t1.col = t2.col;

5. **DELETE FROM** name [**WHERE** ...];

6. **UPDATE** name **SET** col = value [**WHERE** ...];

7. **ALTER TABLE** name **ADD COLUMN** col TYPE;
   **ALTER TABLE** name **DROP COLUMN** col;

8. **SHOW TABLES;**

9. **EXIT;** or **QUIT;**

### Rules:
- All statements end with a semicolon (;)
- Keywords are CASE-INSENSITIVE
- String values use single quotes: 'value'
- Comments: -- single line comment
- Identifiers: letters, digits, underscores (must start with letter or underscore)

## Your Behavior:
1. When the user pastes MiniDB code with errors, identify the exact error, explain it clearly, and provide the corrected code.
2. When asked casual/general questions, answer them normally and helpfully.
3. Always format code examples in code blocks.
4. Be friendly, concise, and technically precise.
5. If the user asks something unrelated to MiniDB, still answer it well — you're a general-purpose assistant too.
"""

# ─── Routes ─────────────────────────────────────────────────────────────────

@app.route('/')
def index():
    return render_template('index.html', keywords=sorted(KEYWORDS))


@app.route('/api/run', methods=['POST'])
def run_code():
    """Execute MiniDB-Lang code via minidb.exe."""
    data = request.get_json()
    code = data.get('code', '').strip()

    if not code:
        return jsonify({'output': '⚠️ No code to execute.', 'success': False})

    if not MINIDB_EXE.exists():
        return jsonify({
            'output': '❌ minidb.exe not found! Run build.bat first.',
            'success': False
        })

    code_with_exit = code + "\nEXIT;\n"

    try:
        result = subprocess.run(
            [str(MINIDB_EXE)],
            input=code_with_exit,
            capture_output=True,
            text=True,
            timeout=10,
            cwd=str(MINIDB_ROOT)
        )

        output = result.stdout
        output = re.sub(r'MiniDB>\s*', '', output)
        for pat in [r'=+\s*\n', r'.*MiniDB v[\d.]+.*\n', r'.*Custom SQL.*\n',
                     r".*Type 'HELP.*\n"]:
            output = re.sub(pat, '', output)
        output = re.sub(r'Commands:\s*\n(?:\s{2,}.*\n)*', '', output)
        output = re.sub(r'Goodbye!\s*', '', output)
        output = re.sub(r'\n{3,}', '\n\n', output).strip()

        stderr_out = result.stderr.strip()
        if stderr_out:
            output += '\n\n⚠️ STDERR:\n' + stderr_out

        if not output:
            output = '✅ Code executed successfully (no output)'

        return jsonify({'output': output, 'success': True})

    except subprocess.TimeoutExpired:
        return jsonify({'output': '⏱️ Execution timed out (10s limit).', 'success': False})
    except Exception as e:
        return jsonify({'output': f'❌ ERROR: {str(e)}', 'success': False})


@app.route('/api/analyze', methods=['POST'])
def analyze():
    """Run static analysis on MiniDB-Lang code."""
    data = request.get_json()
    code = data.get('code', '').strip()

    if not code:
        return jsonify({'status': 'Invalid', 'analysis_metrics': {
            'error_type': 'Syntax', 'line_number': 1, 'offending_token': ''
        }, 'explanation': 'Empty input.', 'full_corrected_code': ''})

    result = analyze_code(code)
    return jsonify(result.to_dict())


@app.route('/api/chat', methods=['POST'])
def chat():
    """Chat with the AI assistant via OpenAI compatible streaming API."""
    data = request.get_json()
    message = data.get('message', '').strip()
    history = data.get('history', [])

    if not message:
        return jsonify({'response': '⚠️ Please enter a message.'})

    if not API_KEY:
        return jsonify({'response': '⚠️ API key not configured.'})

    # Build messages list
    messages = [{"role": "system", "content": MINIDB_SYSTEM_PROMPT}]

    for msg in history[-10:]:
        messages.append({
            "role": msg['role'],
            "content": msg['content']
        })

    # If message contains code keywords, augment with analyzer
    analysis_context = ""
    if any(kw in message.upper() for kw in ['CREATE', 'INSERT', 'SELECT', 'DELETE', 'UPDATE', 'ALTER', 'SHOW']):
        result = analyze_code(message)
        if result.status == "Invalid":
            analysis_context = (
                f"\n\n[Static Analyzer found issues — kindly include this info in your response]\n"
                f"Error Type: {result.error_type}\n"
                f"Line: {result.line_number}\n"
                f"Token: {result.offending_token}\n"
                f"Explanation: {result.explanation}\n"
                f"Suggested Fix:\n{result.full_corrected_code}\n"
            )

    messages.append({"role": "user", "content": message + analysis_context})

    def generate():
        try:
            # Call the OpenAI API (via client) with streaming enabled
            response = client.chat.completions.create(
                model=MODEL_ID,
                messages=messages,
                max_tokens=2048,
                temperature=0.7,
                stream=True,  # Enable streaming
                extra_headers={
                    "HTTP-Referer": "http://localhost:5000",
                    "X-Title": "MiniDB Studio"
                }
            )

            for chunk in response:
                content = chunk.choices[0].delta.content
                if content:
                    yield f"data: {json.dumps({'text': content})}\n\n"

        except Exception as e:
            yield f"data: {json.dumps({'error': str(e)})}\n\n"

    # Return the streaming response
    return app.response_class(generate(), mimetype='text/event-stream')


# ─── Entry Point ────────────────────────────────────────────────────────────

if __name__ == '__main__':
    app.run(debug=True, port=5000)
