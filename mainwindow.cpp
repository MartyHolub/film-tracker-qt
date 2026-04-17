#include "mainwindow.h"

#include "filmdialog.h"
#include "statsdialog.h"

#include <QAction>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow() {
    setWindowTitle(tr("Film Tracker Qt"));
    resize(1200, 760);

    loadSampleFilms();
    setupUi();
    setupMenu();

    applyFilters();
}

void MainWindow::setupUi() {
    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central);

    // sectionCombo_ is kept hidden; sidebar nav buttons set its value
    sectionCombo_ = new QComboBox(this);
    sectionCombo_->addItems({tr("Moje filmy"), tr("Zhlédnuté"), tr("Oblíbené"), tr("Watchlist")});

    // ── Sidebar ──────────────────────────────────────────────────────────────
    auto *sidebarWidget = new QWidget(this);
    sidebarWidget->setMinimumWidth(190);
    sidebarWidget->setMaximumWidth(230);
    auto *sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setSpacing(6);

    auto *appTitle = new QLabel(tr("🎬 Film Tracker"), sidebarWidget);
    QFont appFont = appTitle->font();
    appFont.setBold(true);
    appFont.setPointSize(appFont.pointSize() + 2);
    appTitle->setFont(appFont);
    appTitle->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(appTitle);
    sidebarLayout->addSpacing(4);

    // Navigation buttons (mutually exclusive, checkable)
    auto *navGroup = new QGroupBox(tr("Sekce"), sidebarWidget);
    auto *navLayout = new QVBoxLayout(navGroup);
    navLayout->setSpacing(3);

    const QString navStyle = QStringLiteral(
        "QPushButton { text-align: left; padding: 6px 10px; border-radius: 4px; border: none; }"
        "QPushButton:checked { background: palette(highlight); color: palette(highlighted-text); }"
        "QPushButton:hover:!checked { background: palette(midlight); }");

    auto *allFilmsBtn    = new QPushButton(tr("🎬  Moje filmy"),  navGroup);
    auto *watchedNavBtn  = new QPushButton(tr("✓  Zhlédnuté"),    navGroup);
    auto *favoriteNavBtn = new QPushButton(tr("❤  Oblíbené"),     navGroup);
    auto *watchlistNavBtn = new QPushButton(tr("📋  Watchlist"),   navGroup);

    for (auto *btn : {allFilmsBtn, watchedNavBtn, favoriteNavBtn, watchlistNavBtn}) {
        btn->setFlat(true);
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setStyleSheet(navStyle);
        navLayout->addWidget(btn);
    }
    allFilmsBtn->setChecked(true);

    sidebarLayout->addWidget(navGroup);
    sidebarLayout->addStretch(1);

    // Action buttons
    auto *actionsGroup = new QGroupBox(tr("Akce"), sidebarWidget);
    auto *actionsLayout = new QVBoxLayout(actionsGroup);
    actionsLayout->setSpacing(4);

    auto *addButton    = new QPushButton(tr("＋  Přidat"),  actionsGroup);
    auto *editButton   = new QPushButton(tr("✏  Upravit"), actionsGroup);
    auto *deleteButton = new QPushButton(tr("🗑  Smazat"),  actionsGroup);
    auto *aboutAuthorButton = new QPushButton(tr("ℹ  O autorovi"), actionsGroup);

    actionsLayout->addWidget(addButton);
    actionsLayout->addWidget(editButton);
    actionsLayout->addWidget(deleteButton);
    actionsLayout->addWidget(aboutAuthorButton);
    sidebarLayout->addWidget(actionsGroup);

    // ── Right panel ──────────────────────────────────────────────────────────
    auto *rightWidget = new QWidget(this);
    auto *rightLayout = new QVBoxLayout(rightWidget);

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText(tr("🔍  Hledat podle názvu, žánru nebo režiséra"));
    rightLayout->addWidget(searchEdit_);

    // Filters in a 2-column grid
    auto *filterBox  = new QGroupBox(tr("Filtry"), rightWidget);
    auto *filterGrid = new QGridLayout(filterBox);
    filterGrid->setHorizontalSpacing(12);
    filterGrid->setVerticalSpacing(6);

    nameFilterEdit_ = new QLineEdit(this);
    yearFilterEdit_ = new QLineEdit(this);
    yearFilterEdit_->setPlaceholderText(tr("např. 2024"));

    genreFilterCombo_ = new QComboBox(this);
    genreFilterCombo_->addItems({tr("Vše"), tr("Akční"), tr("Drama"), tr("Komedie"), tr("Sci-Fi"), tr("Thriller"), tr("Krimi"), tr("Horor")});

    watchFilterCombo_ = new QComboBox(this);
    watchFilterCombo_->addItems({tr("Vše"), tr("Sledováno"), tr("Nesledováno")});

    countFilterCombo_ = new QComboBox(this);
    countFilterCombo_->addItems({tr("Vše"), QStringLiteral("10"), QStringLiteral("25"), QStringLiteral("50"), QStringLiteral("100")});

    // Row 0: Název  |  Rok
    filterGrid->addWidget(new QLabel(tr("Název:"),          filterBox), 0, 0);
    filterGrid->addWidget(nameFilterEdit_,                              0, 1);
    filterGrid->addWidget(new QLabel(tr("Rok:"),            filterBox), 0, 2);
    filterGrid->addWidget(yearFilterEdit_,                              0, 3);
    // Row 1: Žánr   |  Sledováno
    filterGrid->addWidget(new QLabel(tr("Žánr:"),           filterBox), 1, 0);
    filterGrid->addWidget(genreFilterCombo_,                            1, 1);
    filterGrid->addWidget(new QLabel(tr("Sledováno:"),      filterBox), 1, 2);
    filterGrid->addWidget(watchFilterCombo_,                            1, 3);
    // Row 2: Počet záznamů
    filterGrid->addWidget(new QLabel(tr("Počet záznamů:"), filterBox), 2, 0);
    filterGrid->addWidget(countFilterCombo_,                            2, 1);
    filterGrid->setColumnStretch(1, 1);
    filterGrid->setColumnStretch(3, 1);

    rightLayout->addWidget(filterBox);

    // Table + detail splitter
    auto *splitter = new QSplitter(this);

    table_ = new QTableWidget(this);
    table_->setColumnCount(6);
    table_->setHorizontalHeaderLabels({tr("Název"), tr("Rok"), tr("Žánr"), tr("Hodnocení"), tr("Délka"), tr("Stav")});
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->verticalHeader()->setVisible(false);

    auto *detailWidget = new QWidget(this);
    auto *detailLayout = new QVBoxLayout(detailWidget);

    detailTitle_ = new QLabel(tr("Vyberte film"), detailWidget);
    QFont titleFont = detailTitle_->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 2);
    detailTitle_->setFont(titleFont);

    detailMeta_ = new QLabel(tr("Rok / Žánr / Režisér"), detailWidget);
    detailMeta_->setWordWrap(true);

    detailBadges_ = new QLabel(tr("Stav"), detailWidget);
    detailDescription_ = new QTextEdit(detailWidget);
    detailDescription_->setReadOnly(true);

    auto *favoriteButton  = new QPushButton(tr("Přepnout oblíbené"),      detailWidget);
    auto *watchlistButton = new QPushButton(tr("Přepnout watchlist"),      detailWidget);
    auto *watchedButton   = new QPushButton(tr("Označit jako sledované"),  detailWidget);

    detailLayout->addWidget(detailTitle_);
    detailLayout->addWidget(detailMeta_);
    detailLayout->addWidget(detailBadges_);
    detailLayout->addWidget(detailDescription_, 1);
    detailLayout->addWidget(favoriteButton);
    detailLayout->addWidget(watchlistButton);
    detailLayout->addWidget(watchedButton);

    splitter->addWidget(table_);
    splitter->addWidget(detailWidget);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    rightLayout->addWidget(splitter, 1);

    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(rightWidget, 1);

    setCentralWidget(central);

    statusLabel_ = new QLabel(this);
    statusBar()->addPermanentWidget(statusLabel_);

    // ── Connections ──────────────────────────────────────────────────────────
    connect(addButton,    &QPushButton::clicked, this, &MainWindow::openAddDialog);
    connect(editButton,   &QPushButton::clicked, this, &MainWindow::openEditDialog);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedFilm);
    connect(aboutAuthorButton, &QPushButton::clicked, this, &MainWindow::showAboutAuthor);
    connect(favoriteButton,  &QPushButton::clicked, this, &MainWindow::toggleFavorite);
    connect(watchlistButton, &QPushButton::clicked, this, &MainWindow::toggleWatchlist);
    connect(watchedButton,   &QPushButton::clicked, this, &MainWindow::markWatched);

    // Sidebar nav → sectionCombo_ (hidden backing store for filter logic)
    connect(allFilmsBtn,    &QPushButton::clicked, this, [this]() { sectionCombo_->setCurrentText(tr("Moje filmy")); });
    connect(watchedNavBtn,  &QPushButton::clicked, this, [this]() { sectionCombo_->setCurrentText(tr("Zhlédnuté")); });
    connect(favoriteNavBtn, &QPushButton::clicked, this, [this]() { sectionCombo_->setCurrentText(tr("Oblíbené")); });
    connect(watchlistNavBtn,&QPushButton::clicked, this, [this]() { sectionCombo_->setCurrentText(tr("Watchlist")); });

    connect(sectionCombo_,      &QComboBox::currentTextChanged, this, &MainWindow::applyFilters);
    connect(searchEdit_,        &QLineEdit::textChanged,        this, &MainWindow::applyFilters);
    connect(nameFilterEdit_,    &QLineEdit::textChanged,        this, &MainWindow::applyFilters);
    connect(yearFilterEdit_,    &QLineEdit::textChanged,        this, &MainWindow::applyFilters);
    connect(genreFilterCombo_,  &QComboBox::currentTextChanged, this, &MainWindow::applyFilters);
    connect(watchFilterCombo_,  &QComboBox::currentTextChanged, this, &MainWindow::applyFilters);
    connect(countFilterCombo_,  &QComboBox::currentTextChanged, this, &MainWindow::applyFilters);
    connect(table_, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateDetailPanel);

    auto *contextMenu     = new QMenu(table_);
    auto *editAction      = contextMenu->addAction(tr("Upravit"));
    auto *favAction       = contextMenu->addAction(tr("Přepnout oblíbené"));
    auto *watchlistAction = contextMenu->addAction(tr("Přepnout watchlist"));
    auto *watchedAction   = contextMenu->addAction(tr("Označit jako sledované"));
    contextMenu->addSeparator();
    auto *deleteAction = contextMenu->addAction(tr("Smazat"));

    connect(editAction,      &QAction::triggered, this, &MainWindow::openEditDialog);
    connect(favAction,       &QAction::triggered, this, &MainWindow::toggleFavorite);
    connect(watchlistAction, &QAction::triggered, this, &MainWindow::toggleWatchlist);
    connect(watchedAction,   &QAction::triggered, this, &MainWindow::markWatched);
    connect(deleteAction,    &QAction::triggered, this, &MainWindow::deleteSelectedFilm);

    table_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table_, &QWidget::customContextMenuRequested, this, [this, contextMenu](const QPoint &pos) {
        if (table_->itemAt(pos) != nullptr) {
            contextMenu->exec(table_->viewport()->mapToGlobal(pos));
        }
    });
}

void MainWindow::setupMenu() {
    auto *fileMenu = menuBar()->addMenu(tr("Soubor"));
    auto *addAction = fileMenu->addAction(tr("Přidat film"));
    addAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    auto *resetFiltersAction = fileMenu->addAction(tr("Reset filtrů"));
    auto *exitAction = fileMenu->addAction(tr("Ukončit"));
    exitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));

    auto *filmsMenu = menuBar()->addMenu(tr("Filmy"));
    auto *editAction = filmsMenu->addAction(tr("Upravit"));
    editAction->setShortcut(Qt::Key_F2);
    auto *deleteAction = filmsMenu->addAction(tr("Smazat vybraný"));
    deleteAction->setShortcut(QKeySequence::Delete);
    auto *statisticsAction = filmsMenu->addAction(tr("Statistiky"));

    auto *helpMenu = menuBar()->addMenu(tr("Nápověda"));
    auto *aboutAuthorAction = helpMenu->addAction(tr("O autorovi"));

    connect(addAction, &QAction::triggered, this, &MainWindow::openAddDialog);
    connect(resetFiltersAction, &QAction::triggered, this, &MainWindow::resetFilters);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(editAction, &QAction::triggered, this, &MainWindow::openEditDialog);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteSelectedFilm);
    connect(statisticsAction, &QAction::triggered, this, &MainWindow::showStatistics);
    connect(aboutAuthorAction, &QAction::triggered, this, &MainWindow::showAboutAuthor);
}

void MainWindow::loadSampleFilms() {
    films_ = {
        {nextFilmId_++, QStringLiteral("Inception"), 2010, QStringLiteral("Sci-Fi / Thriller"), 9.2, 148, QStringLiteral("Christopher Nolan"), QStringLiteral("Zloděj firemních tajemství dostane opačný úkol: myšlenku vložit, ne ukrást."), true, true, false},
        {nextFilmId_++, QStringLiteral("The Dark Knight"), 2008, QStringLiteral("Akční / Drama"), 9.0, 152, QStringLiteral("Christopher Nolan"), QStringLiteral("Batman bojuje s Jokerem a chaosem v Gothamu."), true, true, false},
        {nextFilmId_++, QStringLiteral("Interstellar"), 2014, QStringLiteral("Sci-Fi / Drama"), 8.7, 169, QStringLiteral("Christopher Nolan"), QStringLiteral("Expedice skrz červí díru hledá nový domov pro lidstvo."), true, true, false},
        {nextFilmId_++, QStringLiteral("Parasite"), 2019, QStringLiteral("Thriller / Drama"), 8.6, 132, QStringLiteral("Bong Joon-ho"), QStringLiteral("Dvě rodiny z opačných společenských tříd se propojí s nečekanými důsledky."), false, false, true},
        {nextFilmId_++, QStringLiteral("Dune"), 2021, QStringLiteral("Sci-Fi / Dobrodružství"), 8.0, 155, QStringLiteral("Denis Villeneuve"), QStringLiteral("Příběh Paula Atreida na planetě Arrakis."), false, false, true},
        {nextFilmId_++, QStringLiteral("The Matrix"), 1999, QStringLiteral("Sci-Fi / Akční"), 8.7, 136, QStringLiteral("Wachowski"), QStringLiteral("Programátor zjistí, že realita je simulace."), true, true, false},
        {nextFilmId_++, QStringLiteral("Fight Club"), 1999, QStringLiteral("Drama / Thriller"), 8.8, 139, QStringLiteral("David Fincher"), QStringLiteral("Nespokojenec zakládá podzemní klub."), true, false, false},
        {nextFilmId_++, QStringLiteral("Blade Runner 2049"), 2017, QStringLiteral("Sci-Fi"), 8.0, 164, QStringLiteral("Denis Villeneuve"), QStringLiteral("Blade runner odkrývá tajemství minulosti."), true, true, false},
        {nextFilmId_++, QStringLiteral("Mad Max: Fury Road"), 2015, QStringLiteral("Akční / Sci-Fi"), 8.1, 120, QStringLiteral("George Miller"), QStringLiteral("Útěk post-apokalyptickou pustinou."), false, true, false},
        {nextFilmId_++, QStringLiteral("La La Land"), 2016, QStringLiteral("Drama / Muzikál"), 8.0, 128, QStringLiteral("Damien Chazelle"), QStringLiteral("Hudební romance v Los Angeles."), true, false, false}
    };
}

void MainWindow::openAddDialog() {
    FilmDialog dialog(this);
    dialog.setWindowTitle(tr("Přidat film"));

    Film empty;
    empty.id = nextFilmId_;
    dialog.setFilm(empty);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Film newFilm = dialog.film();
    newFilm.id = nextFilmId_++;
    films_.append(newFilm);
    applyFilters();
}

void MainWindow::openEditDialog() {
    const int index = selectedFilmIndex();
    if (index < 0) {
        QMessageBox::information(this, tr("Info"), tr("Vyberte film v tabulce."));
        return;
    }

    FilmDialog dialog(this);
    dialog.setWindowTitle(tr("Upravit film"));
    dialog.setFilm(films_[index]);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Film updated = dialog.film();
    updated.id = films_[index].id;
    films_[index] = updated;
    applyFilters();

    for (int row = 0; row < filteredIndexes_.size(); ++row) {
        if (filteredIndexes_[row] == index) {
            table_->selectRow(row);
            break;
        }
    }
}

void MainWindow::deleteSelectedFilm() {
    const int index = selectedFilmIndex();
    if (index < 0) {
        QMessageBox::information(this, tr("Info"), tr("Vyberte film v tabulce."));
        return;
    }

    const auto answer = QMessageBox::question(
        this,
        tr("Smazat film"),
        tr("Opravdu chcete smazat film \"%1\"?").arg(films_[index].nazev));

    if (answer != QMessageBox::Yes) {
        return;
    }

    films_.removeAt(index);
    applyFilters();
}

void MainWindow::toggleFavorite() {
    const int index = selectedFilmIndex();
    if (index < 0) {
        QMessageBox::information(this, tr("Info"), tr("Vyberte film v tabulce."));
        return;
    }

    films_[index].oblibene = !films_[index].oblibene;
    applyFilters();
}

void MainWindow::toggleWatchlist() {
    const int index = selectedFilmIndex();
    if (index < 0) {
        QMessageBox::information(this, tr("Info"), tr("Vyberte film v tabulce."));
        return;
    }

    films_[index].watchlist = !films_[index].watchlist;
    applyFilters();
}

void MainWindow::markWatched() {
    const int index = selectedFilmIndex();
    if (index < 0) {
        QMessageBox::information(this, tr("Info"), tr("Vyberte film v tabulce."));
        return;
    }

    films_[index].sledovano = true;
    applyFilters();
}

void MainWindow::showStatistics() {
    StatsDialog dialog(films_, this);
    dialog.exec();
}

void MainWindow::showAboutAuthor() {
    QMessageBox::about(this,
                       tr("O autorovi"),
                       tr("Martin Holub - HOL0601 17.4.2026."));
}

void MainWindow::applyFilters() {
    filteredIndexes_.clear();

    const QString section = sectionCombo_->currentText();
    const QString search = searchEdit_->text().trimmed().toLower();
    const QString nameFilter = nameFilterEdit_->text().trimmed().toLower();
    const QString yearFilter = yearFilterEdit_->text().trimmed();
    const QString genreFilter = genreFilterCombo_->currentText();
    const QString watchedFilter = watchFilterCombo_->currentText();

    for (int i = 0; i < films_.size(); ++i) {
        const Film &film = films_[i];

        if (section == tr("Watchlist") && !film.watchlist) {
            continue;
        }
        if (section == tr("Oblíbené") && !film.oblibene) {
            continue;
        }
        if (section == tr("Zhlédnuté") && !film.sledovano) {
            continue;
        }

        if (watchedFilter == tr("Sledováno") && !film.sledovano) {
            continue;
        }
        if (watchedFilter == tr("Nesledováno") && film.sledovano) {
            continue;
        }

        if (!search.isEmpty()) {
            const QString haystack = film.nazev.toLower() + QStringLiteral(" ") + film.zanr.toLower() + QStringLiteral(" ") + film.reziser.toLower();
            if (!haystack.contains(search)) {
                continue;
            }
        }

        if (!nameFilter.isEmpty() && !film.nazev.toLower().contains(nameFilter)) {
            continue;
        }

        if (!yearFilter.isEmpty() && !QString::number(film.rok).startsWith(yearFilter)) {
            continue;
        }

        if (genreFilter != tr("Vše") && !film.zanr.toLower().contains(genreFilter.toLower())) {
            continue;
        }

        filteredIndexes_.append(i);
    }

    const QString countFilter = countFilterCombo_->currentText();
    if (countFilter != tr("Vše")) {
        bool ok = false;
        const int maxItems = countFilter.toInt(&ok);
        if (ok && maxItems >= 0 && filteredIndexes_.size() > maxItems) {
            filteredIndexes_.resize(maxItems);
        }
    }

    refreshTable();
    refreshStatusBar();
    updateDetailPanel();
}

void MainWindow::refreshTable() {
    table_->setRowCount(filteredIndexes_.size());

    for (int row = 0; row < filteredIndexes_.size(); ++row) {
        const Film &film = films_[filteredIndexes_[row]];

        QStringList states;
        if (film.sledovano) {
            states << QStringLiteral("✓");
        }
        if (film.oblibene) {
            states << QStringLiteral("❤");
        }
        if (film.watchlist) {
            states << QStringLiteral("📋");
        }

        table_->setItem(row, 0, new QTableWidgetItem(film.nazev));
        table_->setItem(row, 1, new QTableWidgetItem(QString::number(film.rok)));
        table_->setItem(row, 2, new QTableWidgetItem(film.zanr));
        table_->setItem(row, 3, new QTableWidgetItem(QString::number(film.hodnoceni, 'f', 1)));
        table_->setItem(row, 4, new QTableWidgetItem(QString::number(film.delka)));
        table_->setItem(row, 5, new QTableWidgetItem(states.isEmpty() ? QStringLiteral("—") : states.join(QStringLiteral(" "))));
    }

    table_->resizeColumnsToContents();
    table_->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::refreshStatusBar() {
    int watched = 0;
    int favorites = 0;
    for (const auto &film : films_) {
        watched += film.sledovano ? 1 : 0;
        favorites += film.oblibene ? 1 : 0;
    }

    statusLabel_->setText(
        tr("Celkem filmů: %1 | Zobrazeno: %2 | Sledováno: %3 | Oblíbené: %4")
            .arg(films_.size())
            .arg(filteredIndexes_.size())
            .arg(watched)
            .arg(favorites));
}

void MainWindow::resetFilters() {
    sectionCombo_->setCurrentText(tr("Moje filmy"));
    searchEdit_->clear();
    nameFilterEdit_->clear();
    yearFilterEdit_->clear();
    genreFilterCombo_->setCurrentText(tr("Vše"));
    watchFilterCombo_->setCurrentText(tr("Vše"));
    countFilterCombo_->setCurrentText(tr("Vše"));
    applyFilters();
}

int MainWindow::selectedFilmIndex() const {
    const auto selectedRows = table_->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        return -1;
    }

    const int row = selectedRows.first().row();
    if (row < 0 || row >= filteredIndexes_.size()) {
        return -1;
    }

    return filteredIndexes_[row];
}

void MainWindow::updateDetailPanel() {
    const int index = selectedFilmIndex();
    if (index < 0) {
        detailTitle_->setText(tr("Vyberte film"));
        detailMeta_->setText(tr("Rok / Žánr / Režisér"));
        detailBadges_->setText(tr("Stav"));
        detailDescription_->setPlainText(tr("Vyberte film v tabulce pro zobrazení detailů."));
        return;
    }

    const Film &film = films_[index];

    detailTitle_->setText(film.nazev);
    detailMeta_->setText(tr("%1 | %2 | %3 | %4 min")
                             .arg(film.rok)
                             .arg(film.zanr)
                             .arg(film.reziser.isEmpty() ? tr("Neznámý režisér") : film.reziser)
                             .arg(film.delka));

    QStringList badges;
    if (film.sledovano) {
        badges << tr("✓ Sledováno");
    }
    if (film.oblibene) {
        badges << tr("❤ Oblíbené");
    }
    if (film.watchlist) {
        badges << tr("📋 Watchlist");
    }

    detailBadges_->setText(badges.isEmpty() ? tr("Bez štítku") : badges.join(QStringLiteral(" | ")));
    detailDescription_->setPlainText(film.popis.isEmpty() ? tr("Popis není k dispozici.") : film.popis);
}
