#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>

class QLabel;
class QPushButton;
class QComboBox;
class QSpinBox;
class QLineEdit;
class QTranslator;
class QMenu;
class QAction;
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
    void goBackDays();
    void goForwardDays();
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
    QSpinBox *m_dayOffset;
    QPushButton *m_backBtn;
    QPushButton *m_forwardBtn;
    QTranslator *m_translator;
    QString m_currentLang;
    QMenu *m_settingsMenu;
    QMenu *m_helpMenu;
    QAction *m_settingsAction;
    QAction *m_aboutAction;
};

#endif
