// Inicjalizacja edytora
const quill = new Quill("#editor", {
    theme: "snow",
    modules: {
    toolbar: [
      [{ header: [1, 2, false] }],
      ['bold', 'italic', 'underline'],
      ['image', 'code-block', 'link'],
      [{ list: 'ordered'}, { list: 'bullet'}]
    ],
  },
  placeholder: 'Tutaj napisz wiadomość...',
});

// Zarządzanie widokami - logowanie/rejestrcja lub właściwa aplikacja
async function checkAuth() {
    const username = await authenticate();
    if (!username) {
        document.getElementById("authSection").classList.remove("hidden");
        document.getElementById("appSection").classList.add("hidden");
    } else {
        document.getElementById("authSection").classList.add("hidden");
        document.getElementById("appSection").classList.remove("hidden");
        document.querySelector('.page-content').classList.add('app-mode');

        document.getElementById("username").innerText = username;
    }
}

async function register() {
    const user = document.getElementById("login").value;
    const pass = document.getElementById("password").value;
    const statusDiv = document.getElementById("authStatus");

    if (!user || !pass) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Podaj login oraz hasło!";
        return;
    }

    statusDiv.style.color = "black";
    statusDiv.innerText = "Rejestracja...";

    try {
        const response = await fetch("/register", {
            method: "POST",
            headers: {"Content-Type": "application/json; charset=utf-8"},
            body: JSON.stringify({username: user, password: pass}) // Konwersja obiektu JS na string JSON
        });

        const result = await response.json();

        if (response.ok && result.status === "success") {
            statusDiv.style.color = "green";
            statusDiv.innerText = "Konto utowrzone. Możesz się teraz zalogować.";
        } else {
            statusDiv.style.color = "red";
            statusDiv.innerText = "Błąd: " + result.error;
        }
    } catch (error) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Błąd: Brak odpowiedzi ze strony serwera C++.";
    }
}

async function login() {
    const user = document.getElementById("login").value;
    const pass = document.getElementById("password").value;
    const statusDiv = document.getElementById("authStatus");

    if (!user || !pass) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Podaj login oraz hasło!";
        return;
    }

    statusDiv.style.color = "black";
    statusDiv.innerText = "Logowanie...";

    try {
        const response = await fetch("/login", {
            method: "POST",
            headers: {"Content-Type": "application/json; charset=utf-8"},
            body: JSON.stringify({username: user, password: pass})
        });

        const result = await response.json();

        if (response.ok && result.status === "success") {
            localStorage.setItem("jwt_token", result.token);

            // Usunięcie danych z pól logowania
            document.getElementById("login").value = "";
            document.getElementById("password").value = "";
            statusDiv.innerText = "";
            checkAuth(); // Przełącza widok na aplikację
        } else {
            statusDiv.style.color = "red";
            statusDiv.innerText = "Błąd: " + result.error;
        }
    } catch (error) {
        statusDiv.style.color = "red";
        statusDiv.innerText = error;
    }
}

function logout() {
    localStorage.removeItem("jwt_token");
    
    // Czyszczenie pól i komunikatów
    document.getElementById("toUser").value = "";
    document.getElementById("sendStatus").innerText = "";
    // Czyszczenie edytora i skrzynki
    quill.setText("");
    document.getElementById("messages").innerHTML = "";
    // Wewnątrz funkcji logout():
document.querySelector('.page-content').classList.remove('app-mode');

    checkAuth(); // powrót do logowania
}

async function sendNote() {
    const token = localStorage.getItem("jwt_token");
    const to = document.getElementById("toUser").value;
    const ttl = parseInt(document.getElementById("ttl").value);
    const burnOnRead = document.getElementById("burnOnRead").checked;
    const statusDiv = document.getElementById("sendStatus");
    // Wyciągnięcie sformatowanego tekstu z edytora
    const textHTML = quill.root.innerHTML;

    if (!token) {
        checkAuth();
        return;
    }
    // Walidacja pól formularza
    if (!to || quill.getText().trim().length === 0) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Podaj odbiorcę i treść notatki!";
        return;
    }

    statusDiv.style.color = "black";
    statusDiv.innerText = "Wysyłanie...";

    // Przygotowanie obiektu z danymi
    const payload = {
        to: to,
        burn: burnOnRead,
        ttl: ttl,
        text: textHTML
    };

    try {
        const response = await fetch("/send", {
            method: "POST",
            headers: {"Content-Type": "application/json; charset=utf-8", "Authorization": token},
            body: JSON.stringify(payload) // Konwersja obiektu JS na string JSON
        });

        const result = await response.json();

        if (response.ok && result.status === "success") {
            statusDiv.style.color = "green";
            statusDiv.innerText = "Sukces! Notatka została wysłana.";
            
            // Czyszczenie pól i edytora
            quill.setText("");
            document.getElementById("toUser").value = "";
            document.getElementById("burnOnRead").checked = false;
        } else {
            if (response.status === 401) logout();
            else {
            statusDiv.style.color = "red";
            statusDiv.innerText = "Błąd: " + result.error;
            }
        }
    } catch (error) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Błąd: Brak odpowiedzi ze strony serwera C++.";
    }
}

async function saveNote() {
    const statusDiv = document.getElementById("sendStatus");
    const username = document.getElementById("username").innerText;

    if (!username) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Błąd: Nie udało pobrać się loginu.";
        return;
    }

    document.getElementById("toUser").value = username;
    sendNote();
}

async function getNotes() {
    const token = localStorage.getItem('jwt_token');
    const messagesDiv = document.getElementById("messages");

    if (!token) {
        checkAuth();
        return;
    }

    messagesDiv.innerHTML = "<p>Pobieranie wiadomości...</p>";

    try {
        const response = await fetch("/inbox", {
            method: "GET",
            headers: {"Authorization": token}
        });
        
        const result = await response.json();

        if (!response.ok) {
            if (response.status === 401) logout();
            else messagesDiv.innerHTML = `<p style="color: red">Błąd: ${result.error}</p>`;
            return;
        }

        if (result.length === 0) {
            messagesDiv.innerHTML = '<p>Twoja skrzynka odbiorcza jest pusta.</p>';
            return;
        }

        result.sort((a, b) => (b.burn === true) - (a.burn === true));

        messagesDiv.innerHTML = "";
        result.forEach(note => {
            // Tworzymy nowy element div dla pojedynczej notatki
            const noteDiv = document.createElement("div");
            noteDiv.id = note.id;

            let deleteButton = "";
            if (!note.burn) {
                deleteButton = `<button class="btn btn-danger" onclick="deleteNote('${note.id}')">Usuń</button>`;
            }

            // Wstrzykujemy nadawcę oraz oryginalny, sformatowany kod HTML z edytora Quill
            noteDiv.innerHTML = `
            <div class="border rounded p-3 my-3">
                <div><b>Od: </b>${note.sender}</div>
                ${('ttl' in note) ? `<div><b>Wygasa: </b>${note.ttl}</div>` : ''} 
                ${note.burn ? `<div style='color: orange;'>BurnOnRead</div>` : ''}
                <div>${DOMPurify.sanitize(note.content)}</div>
                ${deleteButton}
            </div>`;
            messagesDiv.appendChild(noteDiv);
        });

    } catch (error) {
        messagesDiv.innerHTML = "<p style='color: red;'>Błąd: Nie udało się pobrać poczty.</p>";
    }
}

async function deleteNote(id) {
    const token = localStorage.getItem("jwt_token");

    if (!token) {
        checkAuth();
        return;
    }

    try {
        const response = await fetch("/delete", {
            method: "DELETE",
            headers: {"Content-Type": "application/json; charset=utf-8", "Authorization": token},
            body: JSON.stringify({id: id}) // Konwersja obiektu JS na string JSON
        });

        const result = await response.json();

        if (response.ok && result.status === "success") {
            const note = document.getElementById(id)
            if (note) {
                note.remove();
            }
        } else {
            if (response.status === 401) logout();
        }
    } catch (error) {
        console.log(error);
    }
}

async function authenticate() {
    const token = localStorage.getItem("jwt_token");
    
    if (!token) return "";

    try {
        const response = await fetch("/authenticate", {
            method: "GET",
            headers: {"Authorization": token}
        });
        const result = await response.json();

        if (response.ok) {
            return result.username;
        } else {
            return "";
        }
    } catch (error) {
        return "";
    }
}

checkAuth(); // Uruchamiane po otawciu strony