#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QTableWidget;
class QTextEdit;

struct Film {
    int id = 0;
    QString nazev;
    int rok = 2000;
    QString zanr;
    double hodnoceni = 0.0;
    int delka = 0;
    QString reziser;
    QString popis;
    bool sledovano = false;
    bool oblibene = false;
    bool watchlist = false;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openAddDialog();
    void openEditDialog();
    void deleteSelectedFilm();
    void toggleFavorite();
    void toggleWatchlist();
    void markWatched();
    void showStatistics();
    void showAboutAuthor();
    void applyFilters();
    void updateDetailPanel();

private:
    void setupUi();
    void setupMenu();
    void loadSampleFilms();
    void refreshTable();
    void refreshStatusBar();
    void resetFilters();
    int selectedFilmIndex() const;

    QVector<Film> films_;
    QVector<int> filteredIndexes_;
    int nextFilmId_ = 1;

    QComboBox *sectionCombo_ = nullptr;
    QLineEdit *searchEdit_ = nullptr;
    QLineEdit *nameFilterEdit_ = nullptr;
    QLineEdit *yearFilterEdit_ = nullptr;
    QComboBox *genreFilterCombo_ = nullptr;
    QComboBox *watchFilterCombo_ = nullptr;
    QComboBox *countFilterCombo_ = nullptr;

    QTableWidget *table_ = nullptr;

    QLabel *detailTitle_ = nullptr;
    QLabel *detailMeta_ = nullptr;
    QLabel *detailBadges_ = nullptr;
    QTextEdit *detailDescription_ = nullptr;

    QLabel *statusLabel_ = nullptr;
};

#endif // MAINWINDOW_H
