// Inicjalizacja edytora
const quill = new Quill('#editor', {
    theme: 'snow'
});

async function sendNote() {
    const from = document.getElementById('fromUser').value;
    const to = document.getElementById('toUser').value;
    const statusDiv = document.getElementById('status');
    
    // Wyciągnięcie sformatowanego tekstu z edytora
    const textHTML = quill.root.innerHTML;

    // Walidacja pól formularza
    if (!from || !to || quill.getText().length === 0) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Wszystkie pola muszą być wypełnione!";
        return;
    }

    statusDiv.style.color = "black";
    statusDiv.innerText = "Wysyłanie...";

    // Przygotowanie obiektu z danymi
    const payload = {
        from: from,
        to: to,
        text: textHTML
    };

    try {
        const response = await fetch('/send', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json; charset=utf-8'
            },
            body: JSON.stringify(payload) // Konwersja obiektu JS na string JSON
        });

        const result = await response.json();

        if (response.ok && result.status === "success") {
            statusDiv.style.color = "green";
            statusDiv.innerText = "Sukces! Notatka została wysłana.";
            
            // Czyszczenie pól i edytora
            quill.setText('');
            document.getElementById('fromUser').value = '';
            document.getElementById('toUser').value = '';
        } else {
            statusDiv.style.color = "red";
            statusDiv.innerText = "Błąd serwera: " + (result.error || "Nie udało się zapisać zasobu.");
        }
    } catch (error) {
        statusDiv.style.color = "red";
        statusDiv.innerText = "Błąd: Brak odpowiedzi ze strony serwera C++.";
    }
}

async function getNotes() {
    const user = document.getElementById("checkUser").value;
    const messagesDiv = document.getElementById('messages');

    if (!user) {
        messagesDiv.innerHTML = '<p style="color: red;">Błąd: Podaj login!</p>';
        return;
    }

    messagesDiv.innerHTML = "<p>Pobieranie wiadomości...</p>";

    try {
        const response = await fetch(`/inbox?user=${user}`);
        
        // Zwraca tablicę JSON
        const notes = await response.json();

        if (notes.length === 0) {
            messagesDiv.innerHTML = '<p>Twoja skrzynka odbiorcza jest pusta.</p>';
            return;
        }

        // Czyszczenie komunikatu ładowania
        messagesDiv.innerHTML = "";
        notes.forEach(note => {
            // Tworzymy nowy element div dla pojedynczej notatki
            const noteDiv = document.createElement('div');

            // Wstrzykujemy nadawcę oraz oryginalny, sformatowany kod HTML z edytora Quill
            noteDiv.innerHTML = `
                <div>Od: <b>${note.sender}</b></div>
                <div">${note.content}</div>`;

            messagesDiv.appendChild(noteDiv);
        });

    } catch (error) {
        messagesDiv.innerHTML = '<p style="color: red;">Błąd: Nie udało się pobrać poczty.</p>';
    }
}