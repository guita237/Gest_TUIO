# TUIO Gesture Client mit OpenGL & $1-Erkenner

Eine C++-Anwendung, die TUIO-Berührungsereignisse empfängt, Gesten mit dem **$1-Erkenner** auswertet und interaktive 2D-Objekte (Rechtecke & Dreiecke) per Multitouch steuert. Die Darstellung erfolgt über **OpenGL/GLUT**.

---

## Inhaltsverzeichnis

- [Voraussetzungen](#voraussetzungen)
- [Projektstruktur](#projektstruktur)
- [Klassenübersicht](#klassenübersicht)
- [Funktionen & Gesten](#funktionen--gesten)
- [Render-Pipeline](#render-pipeline)
- [TUIO-Architektur](#tuio-architektur)
- [Verwendung](#verwendung)

---

## Voraussetzungen (Windows)

- **Visual Studio** (2014 / 2019)
- [FreeGLUT](https://freeglut.sourceforge.net/) (im Ordner `freeglut/` enthalten)
- **TUIO C++ SDK** (im Ordner `TUIO_CPP/` enthalten)
- **$1 Geometric Recognizer** (im Ordner `gest/` enthalten)
- **TUIOSimulator** zum Testen ohne physische Kamera:
  ```
  E:\Project_C++\TUIO_Gest\TUIOSimulator_win_x86_64\TUIOSimulator.exe
  ```

---

## Projektstruktur

```
(Wurzelverzeichnis)/
├── freeglut/                        # FreeGLUT-Bibliothek
├── gest/                            # $1 Geometric Recognizer
│   ├── GeometricRecognizer.h
│   └── ...
├── TUIO_CPP/                        # TUIO C++ SDK
│   ├── TuioClient.h
│   ├── TuioListener.h
│   ├── TuioCursor.h
│   └── ...
├── Debug/                           # Build-Ausgabe
├── Tuio Client Example.sln          # Visual Studio Solution
├── Tuio Client Example.suo
├── Tuio Client Example.v12.suo
│
└── Tuio Client Example/             # Projektordner
    ├── main.cpp                     # Hauptprogramm: TUIO-Client, OpenGL, Gesten
    ├── Drawable_Object.cpp          # Implementierung der Drawable_Object-Klasse
    ├── Drawable_Object.h            # Header der Drawable_Object-Klasse
    ├── Tuio Client Example.vcxproj  # Visual Studio Projektdatei
    ├── Tuio Client Example.vcxproj.filters
    ├── Tuio Client Example.vcxproj.user
    └── Debug/                       # Lokale Build-Ausgabe
```

---

## Klassenübersicht

### `Drawable_Object`
Repräsentiert ein darstellbares 2D-Objekt (Rechteck oder Dreieck) mit Transformationsfunktionen.

| Attribut / Methode | Beschreibung |
|---|---|
| `Shape` | Form: `"Rectangle"` oder `"Triangle"` |
| `x_pos, y_pos` | Mittelpunkt des Objekts |
| `box_bound_*` | Begrenzungsrahmen (Bounding Box) |
| `rotation` | Aktueller Rotationswinkel in Grad |
| `selected` | Auswahlzustand |
| `select() / deselect()` | Objekt auswählen / abwählen |
| `translate(x, y)` | Objekt verschieben (Mittelpunkt nachführen) |
| `rotate(angle)` | Objekt rotieren |
| `scale(pX, pY, pX2, pY2)` | Skalieren anhand des Abstands zweier Finger |
| `collision_detection(x, y)` | Kollisionsprüfung Finger ↔ Bounding Box |
| `compare(obj*)` | Vergleich zweier Objekte über Bounding Box |

### `Client` (erbt von `TuioListener`)
Empfängt TUIO-Ereignisse und verarbeitet Gesten:
- **`addTuioCursor`** – neuen Finger registrieren
- **`removeTuioCursor`** – Fingerpfad an $1-Erkenner übergeben → Objekt erstellen oder löschen

---

## Funktionen & Gesten

### 1-Finger-Interaktion
| Aktion | Ergebnis |
|---|---|
| Finger auf leerer Fläche bewegen + loslassen | Geste wird erkannt → neues Objekt erscheint an der Position |
| Finger auf Objekt legen | Objekt wird ausgewählt (blau) |
| Ausgewähltes Objekt ziehen | Objekt wird verschoben |

### 2-Finger-Interaktion (auf ausgewähltem Objekt)
| Aktion | Ergebnis |
|---|---|
| Zwei Finger spreizen / zusammenführen | Objekt skalieren |
| Zwei Finger drehen | Objekt rotieren |
| Erster Finger hält Objekt, zweiter zeichnet gleiche Form | Objekt löschen |

### Erkannte Gesten ($1-Erkenner)
| Geste | Ergebnis |
|---|---|
| Rechteck-Geste | Erstellt / löscht ein Rechteck |
| Dreieck-Geste | Erstellt / löscht ein Dreieck |

---

## Render-Pipeline

Jeder Frame in `draw()` durchläuft folgende Schritte:

```
Cursor-Liste von TuioClient holen
         │
         ▼
Fingerpunkte rot zeichnen (GL_POINTS)
         │
         ▼
Kollisionserkennung → Objekt auswählen
         │
         ▼
1 Finger: translate()
2 Finger: rotate() + scale()
         │
         ▼
Objekte zeichnen (Rechteck: glRectd, Dreieck: GL_TRIANGLES)
  └── ausgewählt → blau, nicht ausgewählt → rot
         │
         ▼
Fingerpunkte grün zeichnen (GL_POINTS, obenauf)
         │
         ▼
glutSwapBuffers()
```

---

## TUIO-Architektur

Der TUIO-Client läuft in einem **eigenen Thread** (`tuioThread`), damit die OpenGL-Schleife nicht blockiert wird:

```cpp
HANDLE hThread_TUIO;
hThread_TUIO = (HANDLE)_beginthread(tuioThread, 0, NULL);
```

Koordinatenumrechnung TUIO → OpenGL:
```
OpenGL X = (tuio.x - 0.5) * 2.0
OpenGL Y = (tuio.y - 0.5) * 2.0 * -1.0   // Y-Achse gespiegelt
```

---

## Verwendung

1. **TUIOSimulator starten** (zum Testen ohne Kamera):
   ```
   TUIOSimulator_win_x86_64\TUIOSimulator.exe
   ```
   Der Simulator sendet TUIO-Daten per UDP auf Port `3333`.

2. Projekt in **Visual Studio** öffnen (`Tuio Client Example.sln`).

3. Build-Konfiguration auf **x64 / Debug** oder **Release** setzen.

4. Projekt kompilieren und starten (**F5**).

5. Im OpenGL-Fenster (`520×520 px`) erscheinen beim Start zwei Objekte:
   - ein **Rechteck** bei `(0.5, 0.5)`
   - ein **Dreieck** bei `(-0.2, -0.2)`

6. Im TUIOSimulator Berührungen simulieren → Objekte interaktiv steuern.