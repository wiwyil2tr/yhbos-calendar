#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>

class QLabel;
class QPushButton;
class QComboBox;
class QLineEdit;
class QTranslator;
class CalendarWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void goPrevMonth();
    void goNextMonth();
    void goPrevYear();
    void goNextYear();
    void goToday();
    void onYearMonthChanged();
    void onDateSelected(const QDate &date);
    void jumpToDate();
    void openSettings();
    void openAbout();

private:
    void updateTitle();
    void updateInfo(const QDate &date);
    void syncYearMonthCombo();
    void retranslateUi();
    void loadLanguage(const QString &langCode);

    CalendarWidget *m_calendar;
    QLabel *m_titleLabel;
    QLabel *m_infoLabel;
    QComboBox *m_yearCombo;
    QComboBox *m_monthCombo;
    QLineEdit *m_searchInput;
    QPushButton *m_todayBtn;
    QPushButton *m_goBtn;
    QPushButton *m_jumpBtn;
    QTranslator *m_translator;
    QString m_currentLang;
};

#endif
