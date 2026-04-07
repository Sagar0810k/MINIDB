/* ═══════════════════════════════════════════════════════════════
   MiniDB Studio — Frontend Logic (Terminal REPL Mode)
   ═══════════════════════════════════════════════════════════════ */

// ─── State ──────────────────────────────────────────────────────

let chatHistory = [];
let commandHistory = [];      // history of typed commands for up/down arrow
let historyIndex = -1;        // current position in command history
let lastCommand = '';         // last executed command for Analyze button
let isExecuting = false;      // prevent input while running

// ─── Example Code Snippets ──────────────────────────────────────

const EXAMPLES = {
    create: [
        "CREATE TABLE employees (id INT PRIMARY KEY, name TEXT, dept TEXT);",
        "INSERT INTO employees VALUES (1, 'Alice', 'Engineering');",
        "INSERT INTO employees VALUES (2, 'Bob', 'Marketing');",
        "INSERT INTO employees VALUES (3, 'Charlie', 'Engineering');",
        "SELECT * FROM employees WHERE dept = 'Engineering';"
    ],
    aggregates: [
        "CREATE TABLE scores (id INT, subject TEXT, marks INT);",
        "INSERT INTO scores VALUES (1, 'Math', 95);",
        "INSERT INTO scores VALUES (2, 'Science', 88);",
        "INSERT INTO scores VALUES (3, 'Math', 76);",
        "SELECT COUNT(*) FROM scores;",
        "SELECT AVG(marks) FROM scores;",
        "SELECT SUM(marks) FROM scores;"
    ],
    update: [
        "CREATE TABLE users (id INT PRIMARY KEY, name TEXT, age INT);",
        "INSERT INTO users VALUES (1, 'Alice', 25);",
        "INSERT INTO users VALUES (2, 'Bob', 30);",
        "UPDATE users SET age = 26 WHERE name = 'Alice';",
        "DELETE FROM users WHERE id = 2;",
        "SELECT * FROM users;"
    ],
    join: [
        "CREATE TABLE orders (id INT, product TEXT, customer_id INT);",
        "CREATE TABLE customers (id INT, name TEXT);",
        "INSERT INTO customers VALUES (1, 'Alice');",
        "INSERT INTO customers VALUES (2, 'Bob');",
        "INSERT INTO orders VALUES (101, 'Laptop', 1);",
        "INSERT INTO orders VALUES (102, 'Phone', 2);",
        "SELECT * FROM orders INNER JOIN customers ON orders.customer_id = customers.id;"
    ],
    order: [
        "CREATE TABLE products (id INT, name TEXT, price INT);",
        "INSERT INTO products VALUES (1, 'Keyboard', 50);",
        "INSERT INTO products VALUES (2, 'Mouse', 25);",
        "INSERT INTO products VALUES (3, 'Monitor', 300);",
        "INSERT INTO products VALUES (4, 'Headset', 80);",
        "SELECT * FROM products ORDER BY price DESC LIMIT 3;"
    ],
    alter: [
        "CREATE TABLE inventory (id INT, item TEXT);",
        "INSERT INTO inventory VALUES (1, 'Widget');",
        "ALTER TABLE inventory ADD COLUMN quantity INT;",
        "SHOW TABLES;",
        "SELECT * FROM inventory;"
    ],
    syntax_error: [
        "CREATE TABLE test (id INT, name TEXT)",
        "INSERT INTO test VALUES (1, 'Hello');"
    ],
    semantic_error: [
        "INSERT INTO nonexistent VALUES (1, 'data');",
        "SELECT * FROM missing_table;"
    ]
};

// ─── Tab Switching ──────────────────────────────────────────────

function switchTab(tab) {
    document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
    document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));

    document.getElementById(`tab-btn-${tab}`).classList.add('active');
    document.getElementById(`tab-${tab}`).classList.add('active');

    if (tab === 'chat') {
        setTimeout(() => document.getElementById('chatInput').focus(), 100);
    } else {
        setTimeout(() => document.getElementById('terminalInput').focus(), 100);
    }
}

// ─── Terminal REPL ──────────────────────────────────────────────

function getTerminalInput() {
    return document.getElementById('terminalInput');
}

function getTerminalHistory() {
    return document.getElementById('terminalHistory');
}

function scrollTerminalToBottom() {
    const body = document.getElementById('terminalBody');
    body.scrollTop = body.scrollHeight;
}

function appendToHistory(html) {
    const history = getTerminalHistory();
    const div = document.createElement('div');
    div.innerHTML = html;
    history.appendChild(div);
    scrollTerminalToBottom();
}

function appendCommand(cmd) {
    appendToHistory(
        `<div class="term-line term-command">` +
        `<span class="term-prompt">MiniDB&gt;</span> ` +
        `<span class="term-cmd-text">${escapeHtml(cmd)}</span>` +
        `</div>`
    );
}

function appendOutput(text) {
    if (!text || text.trim() === '') return;
    const escaped = escapeHtml(text);
    appendToHistory(
        `<div class="term-line term-output"><pre class="term-output-pre">${escaped}</pre></div>`
    );
}

function appendError(text) {
    appendToHistory(
        `<div class="term-line term-error">${escapeHtml(text)}</div>`
    );
}

function appendInfo(text) {
    appendToHistory(
        `<div class="term-line term-info">${text}</div>`
    );
}

// ─── Execute Single Statement ───────────────────────────────────

async function executeStatement(cmd) {
    if (!cmd.trim()) return;

    isExecuting = true;
    const input = getTerminalInput();
    input.disabled = true;
    input.placeholder = 'Executing...';

    appendCommand(cmd);

    // Store for Analyze button and command history
    lastCommand = cmd;
    commandHistory.push(cmd);
    historyIndex = commandHistory.length;

    try {
        const resp = await fetch('/api/run', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ code: cmd })
        });
        const data = await resp.json();

        if (data.output) {
            // Check if output contains ERROR
            if (data.output.includes('ERROR') || data.output.includes('❌')) {
                appendError(data.output);
            } else {
                appendOutput(data.output);
            }
        }
    } catch (err) {
        appendError('❌ Network error: ' + err.message);
    } finally {
        isExecuting = false;
        input.disabled = false;
        input.placeholder = 'Type a MiniDB command...';
        input.focus();
    }
}

// ─── Load Example (run all statements sequentially) ─────────────

async function loadExample(name) {
    const statements = EXAMPLES[name];
    if (!statements) return;

    switchTab('editor');
    appendInfo('<span class="term-info-label">📝 Running example:</span> ' + name);

    for (const stmt of statements) {
        await executeStatement(stmt);
    }
}

// ─── Analyze Last Command ───────────────────────────────────────

async function analyzeLastCommand() {
    if (!lastCommand) {
        appendInfo('⚠️ No command to analyze. Type a command first.');
        return;
    }

    const btn = document.getElementById('btnAnalyze');
    btn.disabled = true;

    appendInfo('🔍 Analyzing: <code>' + escapeHtml(lastCommand) + '</code>');

    try {
        const resp = await fetch('/api/analyze', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ code: lastCommand })
        });
        const data = await resp.json();
        showAnalysisInTerminal(data);
    } catch (err) {
        appendError('❌ Analysis error: ' + err.message);
    } finally {
        btn.disabled = false;
    }
}

function showAnalysisInTerminal(data) {
    const isValid = data.status === 'Valid';
    const errorType = data.analysis_metrics?.error_type || 'None';
    const line = data.analysis_metrics?.line_number || 0;

    let html = '<div class="term-analysis">';

    if (isValid) {
        html += '<div class="term-analysis-ok">✅ Code is valid — no errors found.</div>';
    } else {
        html += `<div class="term-analysis-err">`;
        html += `❌ <strong>${errorType} Error</strong>`;
        if (line > 0) html += ` at line ${line}`;
        html += `</div>`;

        if (data.explanation) {
            html += `<div class="term-analysis-desc">${escapeHtml(data.explanation)}</div>`;
        }

        if (data.full_corrected_code) {
            html += `<div class="term-analysis-fix">`;
            html += `<strong>🔧 Suggested fix:</strong><br>`;
            html += `<pre class="term-output-pre term-fix-code">${escapeHtml(data.full_corrected_code)}</pre>`;
            html += `</div>`;
        }
    }

    html += '</div>';
    appendToHistory(html);
}

// ─── Clear Terminal ─────────────────────────────────────────────

function clearTerminal() {
    const history = getTerminalHistory();
    history.innerHTML = `
        <div class="terminal-welcome">
            <span class="welcome-accent">MiniDB Studio v2.0</span> — Interactive Terminal<br>
            Type MiniDB-Lang commands below. Press <kbd>Enter</kbd> to execute.<br>
            Use sidebar examples to get started. Type <code>HELP;</code> for commands.
        </div>
    `;
    lastCommand = '';
    commandHistory = [];
    historyIndex = -1;
    getTerminalInput().focus();
}

// ─── Terminal Input Handler ─────────────────────────────────────

document.addEventListener('DOMContentLoaded', function () {
    const input = document.getElementById('terminalInput');
    if (!input) return;

    input.addEventListener('keydown', function (e) {
        // Enter to execute
        if (e.key === 'Enter' && !isExecuting) {
            e.preventDefault();
            const cmd = input.value.trim();
            if (cmd) {
                input.value = '';
                executeStatement(cmd);
            }
            return;
        }

        // Up arrow — previous command
        if (e.key === 'ArrowUp') {
            e.preventDefault();
            if (commandHistory.length > 0 && historyIndex > 0) {
                historyIndex--;
                input.value = commandHistory[historyIndex];
            }
            return;
        }

        // Down arrow — next command
        if (e.key === 'ArrowDown') {
            e.preventDefault();
            if (historyIndex < commandHistory.length - 1) {
                historyIndex++;
                input.value = commandHistory[historyIndex];
            } else {
                historyIndex = commandHistory.length;
                input.value = '';
            }
            return;
        }
    });

    // Keep focus on terminal input when clicking anywhere in terminal
    document.getElementById('terminalBody').addEventListener('click', function (e) {
        if (e.target === input || e.target.closest('.term-analysis')) return;
        
        // Don't steal focus if the user has selected text (to allow copying)
        if (window.getSelection().toString().length > 0) return;
        
        input.focus();
    });
});

// ─── Utility ────────────────────────────────────────────────────

function escapeHtml(str) {
    const div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}

// ─── Chat Functions ─────────────────────────────────────────────

async function sendChat() {
    const input = document.getElementById('chatInput');
    const message = input.value.trim();
    if (!message) return;

    const emptyEl = document.getElementById('chatEmpty');
    if (emptyEl) emptyEl.style.display = 'none';

    addChatMessage('user', message);
    chatHistory.push({ role: 'user', content: message });
    input.value = '';

    const btn = document.getElementById('btnSend');
    btn.disabled = true;
    btn.innerHTML = '<span class="spinner"></span>';

    try {
        const resp = await fetch('/api/chat', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ message, history: chatHistory })
        });

        // Setup streaming UI
        const container = document.getElementById('chatMessages');
        const div = document.createElement('div');
        div.className = 'chat-msg assistant';
        div.innerHTML = `
            <div class="avatar">🤖</div>
            <div class="msg-bubble" id="streaming-bubble">...</div>
        `;
        container.appendChild(div);
        container.scrollTop = container.scrollHeight;

        const bubble = div.querySelector('.msg-bubble');
        let assistantContent = "";

        const reader = resp.body.getReader();
        const decoder = new TextDecoder("utf-8");
        let buffer = "";

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;

            buffer += decoder.decode(value, { stream: true });
            let lines = buffer.split('\n');
            buffer = lines.pop();

            for (let line of lines) {
                if (line.trim() === '') continue;
                if (line.startsWith('data: ')) {
                    try {
                        const data = JSON.parse(line.substring(6));
                        if (data.error) {
                            assistantContent += "\n❌ Error: " + data.error;
                        } else if (data.text) {
                            assistantContent += data.text;
                        }
                        bubble.innerHTML = renderMarkdown(assistantContent);
                        container.scrollTop = container.scrollHeight;
                    } catch (e) {}
                }
            }
        }

        bubble.removeAttribute('id');
        chatHistory.push({ role: 'assistant', content: assistantContent });

    } catch (err) {
        addChatMessage('assistant', '❌ Network error: ' + err.message);
    } finally {
        btn.disabled = false;
        btn.innerHTML = 'Send ➤';
    }
}

function addChatMessage(role, content) {
    const container = document.getElementById('chatMessages');
    const div = document.createElement('div');
    div.className = `chat-msg ${role}`;

    const avatar = role === 'user' ? '👤' : '🤖';
    const rendered = role === 'assistant' ? renderMarkdown(content) : escapeHtml(content);

    div.innerHTML = `
        <div class="avatar">${avatar}</div>
        <div class="msg-bubble">${rendered}</div>
    `;

    container.appendChild(div);
    container.scrollTop = container.scrollHeight;
}

function sendEditorCode() {
    if (!lastCommand) return;
    const input = document.getElementById('chatInput');
    input.value = `Please analyze this MiniDB code for errors and fix any issues:\n\n${lastCommand}`;
    sendChat();
}

function askGrammar() {
    const input = document.getElementById('chatInput');
    input.value = 'Give me a concise reference of all MiniDB-Lang syntax and grammar rules with examples.';
    sendChat();
}

function clearChat() {
    chatHistory = [];
    const container = document.getElementById('chatMessages');
    container.innerHTML = `
        <div class="chat-empty" id="chatEmpty">
            <div class="chat-empty-icon">💬</div>
            <h4>How can I help you today?</h4>
            <p>
                Paste MiniDB code for debugging • Ask about SQL syntax<br>
                Or ask me anything else — I'm a general-purpose assistant too!
            </p>
        </div>
    `;
}

// ─── Simple Markdown Renderer ───────────────────────────────────

function renderMarkdown(text) {
    let html = escapeHtml(text);
    html = html.replace(/```(\w*)\n([\s\S]*?)```/g, function (match, lang, code) {
        return `<pre><code>${code.trim()}</code></pre>`;
    });
    html = html.replace(/`([^`]+)`/g, '<code>$1</code>');
    html = html.replace(/\*\*([^*]+)\*\*/g, '<strong>$1</strong>');
    html = html.replace(/\*([^*]+)\*/g, '<em>$1</em>');
    html = html.replace(/\n/g, '<br>');
    return html;
}
