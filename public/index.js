// Inicjalizacja edytora
const quill = new Quill("#editor", {
    theme: "snow"
});

// Zarządzanie widokami - logowanie/rejestrcja lub właściwa aplikacja
function checkAuth() {
    const token = localStorage.getItem("jwt_token");
    if (token) {
        document.getElementById("authSection").classList.add("hidden");
        document.getElementById("appSection").classList.remove("hidden");
    } else {
        document.getElementById("authSection").classList.remove("hidden");
        document.getElementById("appSection").classList.add("hidden");
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
            statusDiv.innerText = "Błąd serwera: " + result.error;
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
            body: JSON.stringify({username: user, password: pass}) // Konwersja obiektu JS na string JSON
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
            statusDiv.innerText = "Błąd serwera: " + result.error;
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
    document.getElementById("")

    checkAuth(); // powrót do logowania
}

async function sendNote() {
    const token = localStorage.getItem("jwt_token");
    const to = document.getElementById("toUser").value;
    const statusDiv = document.getElementById("sendStatus");
    // Wyciągnięcie sformatowanego tekstu z edytora
    const textHTML = quill.root.innerHTML;

    if (!token) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Zaloguj się!";
        return;
    }
    // Walidacja pól formularza
    if (!to || quill.getText().length === 0) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Podaj odbiorcę i treść notatki!";
        return;
    }

    statusDiv.style.color = "black";
    statusDiv.innerText = "Wysyłanie...";

    // Przygotowanie obiektu z danymi
    const payload = {
        to: to,
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
        } else {
            statusDiv.style.color = "red";
            statusDiv.innerText = "Błąd serwera: " + result.error;
        }
    } catch (error) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Błąd: Brak odpowiedzi ze strony serwera C++.";
    }
}

async function getNotes() {
    const token = localStorage.getItem('jwt_token');
    const messagesDiv = document.getElementById("messages");

    if (!token) {
        messagesDiv.innerHTML = "<p style='color: red'>Zaloguj się!</p>";
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
            messagesDiv.innerHTML = `<p style="color: red">Błąd serwera: ${result.error}</p>`;
            return;
        }

        if (result.length === 0) {
            messagesDiv.innerHTML = '<p>Twoja skrzynka odbiorcza jest pusta.</p>';
            return;
        }

        messagesDiv.innerHTML = "";
        result.forEach(note => {
            // Tworzymy nowy element div dla pojedynczej notatki
            const noteDiv = document.createElement("div");

            // Wstrzykujemy nadawcę oraz oryginalny, sformatowany kod HTML z edytora Quill
            noteDiv.innerHTML = `
                <div>Od: <b>${note.sender}</b></div>
                <div">${note.content}</div>`;

            messagesDiv.appendChild(noteDiv);
        });

    } catch (error) {
        messagesDiv.innerHTML = "<p style='color: red;'>Błąd: Nie udało się pobrać poczty.</p>";
    }
}

checkAuth(); // Uruchamiane po otawciu strony