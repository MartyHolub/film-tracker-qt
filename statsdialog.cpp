#include "statsdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <algorithm>

StatsDialog::StatsDialog(const QVector<Film> &films, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("Statistiky"));
    resize(480, 420);

    int watched = 0;
    int favorites = 0;
    int watchlist = 0;
    int totalLength = 0;
    double sumRating = 0.0;

    for (const auto &film : films) {
        watched += film.sledovano ? 1 : 0;
        favorites += film.oblibene ? 1 : 0;
        watchlist += film.watchlist ? 1 : 0;
        totalLength += film.delka;
        sumRating += film.hodnoceni;
    }

    const int total = films.size();
    const double avgRating = total > 0 ? sumRating / static_cast<double>(total) : 0.0;
    const double avgLength = total > 0 ? static_cast<double>(totalLength) / static_cast<double>(total) : 0.0;

    auto sortedFilms = films;
    std::sort(sortedFilms.begin(), sortedFilms.end(), [](const Film &a, const Film &b) {
        return a.hodnoceni > b.hodnoceni;
    });

    QStringList top;
    const qsizetype topCount = std::min<qsizetype>(5, sortedFilms.size());
    for (qsizetype i = 0; i < topCount; ++i) {
        top.append(QString::number(static_cast<int>(i + 1)) + ". " + sortedFilms[i].nazev + " (" + QString::number(sortedFilms[i].hodnoceni, 'f', 1) + ")");
    }

    auto *layout = new QVBoxLayout(this);
    auto *summary = new QLabel(this);
    summary->setText(
        tr("Celkem filmů: %1\nSledováno: %2\nOblíbené: %3\nWatchlist: %4\nPrůměrné hodnocení: %5\nPrůměrná délka: %6 min\nCelková délka: %7 h %8 min")
            .arg(total)
            .arg(watched)
            .arg(favorites)
            .arg(watchlist)
            .arg(QString::number(avgRating, 'f', 2))
            .arg(QString::number(avgLength, 'f', 1))
            .arg(totalLength / 60)
            .arg(totalLength % 60));
    summary->setWordWrap(true);

    auto *topList = new QTextEdit(this);
    topList->setReadOnly(true);
    topList->setPlainText(tr("Top 5 filmů dle hodnocení\n\n") + top.join("\n"));

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    layout->addWidget(summary);
    layout->addWidget(topList);
    layout->addWidget(buttons);
}
