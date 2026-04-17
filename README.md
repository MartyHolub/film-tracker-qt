# film-tracker-qt

Qt Widgets verze projektu [film-tracker](https://github.com/MartyHolub/film-tracker).

## Obsah projektu

- tabulka filmů
- CRUD operace (přidat / upravit / smazat)
- filtrování podle sekce, názvu, roku, žánru, sledovanosti a počtu položek
- detail vybraného filmu
- kontextové menu a klávesové zkratky
- dialog Statistiky
- levé menu **O autorovi**

## Struktura

- `film-tracker-qt.pro`
- `main.cpp`
- `mainwindow.h`, `mainwindow.cpp`
- `filmdialog.h`, `filmdialog.cpp`
- `statsdialog.h`, `statsdialog.cpp`

## Spuštění

```bash
qmake film-tracker-qt.pro
make
./film-tracker-qt
```

> V projektu nejsou použité absolutní cesty, pouze interní data v kódu.
